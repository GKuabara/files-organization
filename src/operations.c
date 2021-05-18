#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"

/*
    Initializes a binary header. Can be used for both line and vehicle files
    given their respective `header_funct` via `finfo`. 
*/
static void _header_init(struct _g_files *files, struct _finfo *finfo) {
    g_header_init(files->bin, finfo->header_size);
    finfo->header_funct(files);
}

/* 
    Inserts a new datareg. Can be used for both line and vehicle files given
    their respective `insert_funct` via `finfo` 
*/
static struct _reg_update *_insert_datareg(FILE *bin, string *tokens, struct _finfo *finfo) {
    fseek(bin, finfo->next_reg_offset, SEEK_SET);
   
    struct _reg_update *update = g_insert_datareg(bin, tokens, finfo);
    finfo->insert_funct(bin, tokens);

    return update;
}

/*
    Reads an csv file and inserts all dataregs from it. Can be used for both
    line and vehicle files given their respective `insert_funct` via `finfo`.
*/
static void _insert_dataregs_from_csv(struct _g_files *files, struct _finfo *finfo) {
    string line;

    while ((line = readline(files->csv)) != NULL) {
        string *tokens = str_get_tokens(line,  
            (struct _delim_t) {.amnt_delim=2, .delim=csv_delim});
        
        struct _reg_update *update = _insert_datareg(files->bin, tokens, finfo);

        if (update->is_removed == RMV) finfo->amnt_rmv++;
        else finfo->amnt_reg++;

        g_header_update(files->bin, INC_STAT, update, finfo); // Updates the header

        /* Frees all aux structures */
        for (string *t = tokens; *t; ++t) free(*t); //TODO: Make a function for this
        free(tokens);       
        free(line);
        free(update);
    }

    g_header_update(files->bin, CON_STAT, NULL, finfo); // Update header status only
}

/*
    Creates a vehicle table
*/
void vehicle_create_table(string csv_name, string bin_name) {
    struct _g_files *files = g_open_files(csv_name, bin_name);
    struct _finfo vinfo = {.amnt_const = 4, 
                           .amnt_reg = 0,
                           .amnt_rmv = 0,
                           .next_reg_offset = V_HEADER_SIZE,
                           .const_size = V_CONST_REG_SIZE, 
                           .header_funct = v_header_init, 
                           .header_size = V_HEADER_SIZE, 
                           .insert_funct = v_insert_datareg};

    
    _header_init(files, &vinfo);
    _insert_dataregs_from_csv(files, &vinfo);

    fclose(files->bin); //TODO: Make a function for this
    fclose(files->csv);
    free(files);  
}

/*
    Creates a line table
*/
void line_create_table(string csv_name, string bin_name) {
    struct _g_files *files = g_open_files(csv_name, bin_name);
    struct _finfo linfo = {.amnt_const = 2, 
                           .amnt_reg = 0,
                           .amnt_rmv = 0,
                           .next_reg_offset = L_HEADER_SIZE,
                           .const_size = L_CONST_REG_SIZE, 
                           .header_funct = l_header_init, 
                           .header_size = L_HEADER_SIZE, 
                           .insert_funct = l_insert_datareg};

    
    _header_init(files, &linfo);
    _insert_dataregs_from_csv(files, &linfo);

    fclose(files->bin); //TODO: Make a function for this
    fclose(files->csv);
    free(files);  
}