#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"

struct _finfo {
    int amnt_const;
    int const_size;
    int reg_info_size;
    g_offset_t header_size;
    void (*insert_funct)(FILE *, string *);
    void (*header_funct)(struct _g_files *);
};

static void _header_init(struct _g_files *files, const struct _finfo finfo) {
    g_header_init(files->bin, finfo.header_size);
    finfo.header_funct(files);
}

static struct _reg_update *_insert_datareg(FILE *bin, g_offset_t offset,  string *tokens, const struct _finfo finfo) {
    fseek(bin, offset, SEEK_SET);
   
    struct _reg_update *update = g_insert_datareg(bin, tokens, finfo.amnt_const, finfo.reg_info_size);
    finfo.insert_funct(bin, tokens);

    return update;
}

static void _insert_dataregs_from_csv(struct _g_files *files, const struct _finfo finfo) {
    g_offset_t offset = finfo.header_size;
    int amnt_reg = 0;
    int amnt_rmv = 0;

    string line;
    while ((line = readline(files->csv)) != NULL) {
        string *tokens = str_get_tokens(line,  
            (struct _delim_t) {.amnt_delim=2, .delim=csv_delim});
        
        struct _reg_update *update = _insert_datareg(files->bin, offset, tokens, finfo);

        if (update->is_removed == RMV) amnt_rmv++;
        else amnt_reg++;

        offset = g_header_update(files->bin, INC_STAT, update, offset, amnt_reg, amnt_rmv, finfo.reg_info_size);

        for (string *t = tokens; *t; ++t) free(*t);
        free(tokens);    
        free(line);
        free(update);
    }

    g_header_update(files->bin, CON_STAT, NULL, 0, 0, 0, 0);
}

void vehicle_create_table(string csv_name, string bin_name) {
    struct _g_files *files = g_open_files(csv_name, bin_name);
    struct _finfo vinfo = {.amnt_const = 4, .const_size = v_reg_static_size, .header_funct = v_header_init, .header_size = V_HEADER_SIZE, .insert_funct = v_insert_datareg, .reg_info_size = v_reg_info_size};

    _header_init(files, vinfo);
    _insert_dataregs_from_csv(files, vinfo);

    fclose(files->bin);
    fclose(files->csv);
    free(files);  
}