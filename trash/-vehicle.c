#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global_header_macros.h"
#include "vehicle_header_macros.h"
#include "global_datareg_macros.h"
#include "vehicle_datareg_macros.h"
#include "vehicle.h"

struct _v_files {
    FILE *bin;
    FILE *csv;
};

struct _v_reg_update {
    g_removed_t is_removed;
    g_reg_size_t reg_size;
};

#define _vdr_is_null(token) (strcmp(token, "NULO") == 0 ? 1 : 0)
#define _vdr_is_removed(prefix) (prefix[0] == '*' ? RMV : NRM)

static struct _v_files *_v_open_files(string csv_name, string bin_name);
static void _vh_global_init(struct _v_files *files);
static void _vehicle_write_csv_header(struct _v_files *files);
static void _vh_init(struct _v_files *files);

static struct _v_files *_v_open_files(string csv_name, string bin_name) {
    struct _v_files *files = malloc(sizeof(*files));
    assert(files);

    files->csv = open_file(csv_name, "r");
    files->bin = open_file(bin_name, "w+b");
    
    return files;
}

static void _vh_global_init(struct _v_files *files) {
    g_status_t stats = INC_STAT;
    fwrite(&stats, sizeof(g_status_t), 1, files->bin);

    g_offset_t off_next_reg = V_HEADER_SIZE;
    fwrite(&off_next_reg, sizeof(g_offset_t), 1, files->bin);

    g_amnt_reg_t n_reg = 0;
    fwrite(&n_reg, sizeof(g_amnt_reg_t), 1, files->bin);

    g_amnt_rmv_t amnt_removed_reg = 0;
    fwrite(&amnt_removed_reg, sizeof(g_amnt_rmv_t), 1, files->bin);
}

static void _vh_desc_init(struct _v_files *files) {
    string line = readline(files->csv);
    string *csv_header = str_get_tokens(line, (struct _delim_t) {.amnt_delim=2,.delim=csv_delim});

    fwrite(csv_header[PREFIX], sizeof(v_prefix_desc_t), 1, files->bin);
    fwrite(csv_header[DATE], sizeof(v_date_desc_t), 1, files->bin);
    fwrite(csv_header[SEAT], sizeof(v_seat_desc_t), 1, files->bin);
    fwrite(csv_header[LINE], sizeof(v_line_desc_t), 1, files->bin);
    fwrite(csv_header[MODEL], sizeof(v_model_desc_t), 1, files->bin);
    fwrite(csv_header[CATEGORY], sizeof(v_category_desc_t), 1, files->bin);
 
    for (string *aux = csv_header; *aux; aux++) free(*aux);
    free(csv_header);
    free(line);
}

static void _vh_init(struct _v_files *files) {
    _vh_global_init(files);
    _vh_desc_init(files);
}

int _reg_get_size(string *tokens, string model, string category) {
    int model_len = strlen(model);
    int category_len = strlen(category);

    return model_len + category_len + v_reg_static_size;
}

static struct _v_reg_update *vdr_write_new_reg(FILE *bin, g_offset_t offset, string *tokens) {
    fseek(bin, offset, SEEK_SET);
    struct _v_reg_update *update = malloc(sizeof(*update));
    assert(update);

    string date = _vdr_is_null(tokens[DATE]) ? "\0@@@@@@@@@" : tokens[DATE];
    int amnt_seats = _vdr_is_null(tokens[SEAT]) ? -1 : atoi(tokens[SEAT]);
    int line_code = _vdr_is_null(tokens[LINE]) ? -1 : atoi(tokens[LINE]);
    
    int model_len = _vdr_is_null(tokens[MODEL]) ? 0 : strlen(tokens[MODEL]);
    string model = (model_len == 0) ? "" : tokens[MODEL];
    
    int category_len = _vdr_is_null(tokens[CATEGORY]) ? 0 : strlen(tokens[CATEGORY]);
    string category = (category_len) == 0 ? "" : tokens[CATEGORY];
     
    update->is_removed = _vdr_is_removed(tokens[PREFIX]);
    update->reg_size = _reg_get_size(tokens, model, category);
   
    fwrite(&(update->is_removed), sizeof(g_removed_t), 1, bin);
    fwrite(&(update->reg_size), sizeof(g_reg_size_t), 1, bin);
    
    fwrite(tokens[PREFIX], sizeof(v_prefix_t), 1, bin);
    fwrite(date, sizeof(v_date_t), 1, bin);
    fwrite(&amnt_seats, sizeof(v_amnt_seats_t), 1, bin);
    fwrite(&line_code, sizeof(v_code_t), 1, bin);
    
    fwrite(&model_len, sizeof(v_model_size_t), 1, bin);
    fwrite(model, sizeof(char), model_len, bin);
    
    fwrite(&category_len, sizeof(v_category_size_t), 1, bin);
    fwrite(category, sizeof(char), category_len, bin);

    return update;
}

static g_offset_t _vh_update(FILE *bin, g_status_t stats, struct _v_reg_update *update, g_offset_t l_offset, int amnt_reg, int amnt_rmv) {
    fseek(bin, 0, SEEK_SET);

    fwrite(&stats, sizeof(g_status_t), 1, bin);
    if (stats == CON_STAT) return 0; 

    g_offset_t next_reg = l_offset + update->reg_size + v_reg_info_size;
    fwrite(&next_reg, sizeof(g_offset_t), 1, bin);

    fwrite(&amnt_reg, sizeof(g_amnt_reg_t), 1, bin);

    fwrite(&amnt_rmv, sizeof(g_amnt_rmv_t), 1, bin);

    return next_reg;
}

static void _vdr_insert_regs_from_csv(struct _v_files *files) {
    g_offset_t offset = V_HEADER_SIZE; 
    int amnt_reg = 0;
    int amnt_rmv = 0;

    string line;
    while ((line = readline(files->csv)) != NULL) {
        string *tokens = str_get_tokens(line,  
            (struct _delim_t) {.amnt_delim=2, .delim=csv_delim});
        
        struct _v_reg_update *update = vdr_write_new_reg(files->bin, offset, tokens);

        if (update->is_removed == RMV) amnt_rmv++;
        else amnt_reg++;

        offset = _vh_update(files->bin, INC_STAT, update, offset, amnt_reg, amnt_rmv);
   
        for (string *t = tokens; *t; ++t) free(*t);
        free(tokens);    
        free(line);
        free(update);
    }

    _vh_update(files->bin, CON_STAT, NULL, 0, 0, 0);
}

void vehicle_create_table(string csv_name, string bin_name) {
    struct _v_files *files = _v_open_files(csv_name, bin_name);

    _vh_init(files);  
    _vdr_insert_regs_from_csv(files);

    fclose(files->bin);
    fclose(files->csv);
    free(files);  
}