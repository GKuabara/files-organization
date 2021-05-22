#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

static int _g_reg_get_size(string *var_fields, int const_size);

/*
    Gets the size of the new reg to be inserted
*/
static int _g_reg_get_size(string *var_fields, int const_size) {
    int size = 0;
    for (string *aux = var_fields; *aux; ++aux)  
        size += _g_is_null(*aux) ? 0 : strlen(*aux);

    return size + const_size;
}

/*
    Opens both files. TODO: Maybe put in a file handler module (?)
*/
struct _g_files *g_open_files(string csv_name, string bin_name) {
    struct _g_files *files = malloc(sizeof(*files));
    assert(files); // In case of error

    files->csv = open_file(csv_name, "r");
    files->bin = open_file(bin_name, "w+b");

    return files;
}

/*
    Global start of an header. Can be used for both vehicle and line files
    given their total `header_size`.
*/
void g_header_init(FILE *bin, long header_size) {
    char stats = INC_STAT;
    int n_reg = 0;
    int amnt_removed_reg = 0;

    /* fwrite Error handling */
    if (fwrite(&stats, sizeof(char), 1, bin) != 1);
    if (fwrite(&header_size, sizeof(long), 1, bin) != 1);
    if (fwrite(&stats, sizeof(int), 1, bin) != 1);
    if (fwrite(&stats, sizeof(int), 1, bin) != 1);
}

/*
    Updates the 'global' part of a header. Can be used for both vehicle and line files given `finfo`.
*/
void g_header_update(FILE *bin, char stats, struct _reg_update *update, struct _finfo *finfo) {
    long next_reg = ftell(bin);
    fseek(bin, 0, SEEK_SET);

    /* fwrite Error handling */
    if (fwrite(&stats, sizeof(char), 1, bin) != 1);
    if (stats == CON_STAT) return; 

    if (fwrite(&next_reg, sizeof(long), 1, bin) != 1);
    if (fwrite(&(finfo->amnt_reg), sizeof(int), 1, bin) != 1);
    if (fwrite(&(finfo->amnt_rmv), sizeof(int), 1, bin) != 1);

    finfo->next_reg_offset = next_reg;
}

/*
    Global part of a new datareg insertion. Can be user for both vehicle and line files given `finfo`.
*/
struct _reg_update *g_insert_datareg(FILE *bin, string *tokens, struct _finfo *finfo) {
    struct _reg_update *update = malloc(sizeof(*update));
    assert(update);
    
    update->is_removed = _g_is_rmv((*tokens));
    update->reg_size = _g_reg_get_size(tokens + finfo->amnt_const, finfo->const_size);

    /* fwrite Error Handling */
    if (fwrite(&(update->is_removed), sizeof(char), 1, bin) != 1);
    if (fwrite(&(update->reg_size), sizeof(int), 1, bin) != 1);    

    return update;
}


/*
    Reads the first byte (rmv status) of a reg. 
*/ 
int g_read_reg_rmv_stats(FILE *bin) {
    char is_removed;
    if (fread(&is_removed, sizeof(char), 1, bin) != 1);
    
    return is_removed;
}

/*
    Reads the reg_size byte of a reg.
*/
int g_read_reg_size(FILE *bin) {
    int reg_size;
    if (fread(&reg_size, sizeof(char), 1, bin) != 1);
    
    return reg_size;
}