#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

static g_reg_size_t _g_reg_get_size(string *var_fields, g_reg_size_t const_size);


static g_reg_size_t _g_reg_get_size(string *var_fields, g_reg_size_t const_size) {
    int size = 0;
    for (string *aux = var_fields; *aux; ++aux) 
        size += strlen(*aux);

    return size + const_size;
}

struct _g_files *g_open_files(string csv_name, string bin_name) {
    struct _g_files *files = malloc(sizeof(*files));
    assert(files); // In case of error

    files->csv = open_file(csv_name, "r");
    files->bin = open_file(bin_name, "w+b");

    return files;
}

void g_header_init(FILE *bin, g_offset_t header_size) {
    g_status_t stats = INC_STAT;
    g_amnt_reg_t n_reg = 0;
    g_amnt_rmv_t amnt_removed_reg = 0;

    /* Error handling */
    if (fwrite(&stats, sizeof(g_status_t), 1, bin) != 1);
    if (fwrite(&header_size, sizeof(g_offset_t), 1, bin) != 1);
    if (fwrite(&stats, sizeof(g_amnt_reg_t), 1, bin) != 1);
    if (fwrite(&stats, sizeof(g_amnt_rmv_t), 1, bin) != 1);
}

g_offset_t g_header_update(FILE *bin, g_status_t stats, struct _reg_update *update, g_offset_t offset, int amnt_reg, int amnt_rmv, int reg_info_size) {
    g_offset_t next_reg = ftell(bin);
    fseek(bin, 0, SEEK_SET);

    if (fwrite(&stats, sizeof(g_status_t), 1, bin) != 1);
    if (stats == CON_STAT) return 0; 

    if (fwrite(&next_reg, sizeof(g_offset_t), 1, bin) != 1);
    
    if (fwrite(&amnt_reg, sizeof(g_amnt_reg_t), 1, bin) != 1);
    if (fwrite(&amnt_rmv, sizeof(g_amnt_rmv_t), 1, bin) != 1);

    return next_reg;
}

struct _reg_update *g_insert_datareg(FILE *bin, string *tokens, int amnt_const, g_reg_size_t const_size) {
    struct _reg_update *update = malloc(sizeof(*update));
    assert(update);
    
    update->is_removed = _g_is_rmv((*tokens));
    update->reg_size = _g_reg_get_size(tokens + amnt_const, const_size);

    if (fwrite(&(update->is_removed), sizeof(g_removed_t), 1, bin) != 1);
    if (fwrite(&(update->reg_size), sizeof(g_reg_size_t), 1, bin) != 1);    

    return update;
}

