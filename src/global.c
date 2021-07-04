/*
** Module reponsible for handlig data common to both vehicle and line 
** binary and csv files.

**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"


static int _g_reg_get_size(string *var_fields, int const_size);
static void _g_update_amnt_reg(FILE *bin, int new_regs);


/*
    Gets the size of the new reg to be inserted
*/
static int _g_reg_get_size(string *var_fields, int const_size) {
    int size = 0;
    for (string *aux = var_fields; *aux; ++aux)  
        size += _g_is_null(*aux) ? 0 : strlen(*aux);

    return size + const_size;
}

static void _g_update_amnt_reg(FILE *bin, int new_regs) {
    fseek(bin, 0, SEEK_CUR);
    int total_amnt_reg;

    /* fread & Error handling */
    file_read(&total_amnt_reg, sizeof(int), 1, bin);
    
    total_amnt_reg += new_regs;
    fseek(bin, -sizeof(int), SEEK_CUR);
    file_write(&total_amnt_reg, sizeof(int), 1, bin);
}

/*
    Global start of an header. Can be used for both vehicle and line files
    given their total `header_size`.
*/
void g_header_init(FILE *bin, long header_size) {
    char stats = INC_STAT;
    int amnt_reg = 0;
    int amnt_rmv = 0;

    /* fwrite & Error handling */
    file_write(&stats, sizeof(char), 1, bin);
    file_write(&header_size, sizeof(long), 1, bin);
    file_write(&amnt_reg, sizeof(int), 1, bin);
    file_write(&amnt_rmv, sizeof(int), 1, bin);
}

/*
    Updates the 'global' part of a header. Can be used for both vehicle and line files given `finfo`.
*/
void g_header_update(FILE *bin, char stats, int amnt_reg, int amnt_rmv) {
    long next_reg = ftell(bin);
    fseek(bin, 0, SEEK_SET);

    /* fwrite & Error handling */
    file_write(&stats, sizeof(char), 1, bin);
    if (stats == INC_STAT) return; 

    file_write(&next_reg, sizeof(long), 1, bin);

    _g_update_amnt_reg(bin, amnt_reg);
    _g_update_amnt_reg(bin, amnt_rmv);
}

/*
    Global part of a new datareg insertion (remotion and reg size). Can be user for both vehicle and line files given `finfo`.
*/
_reg_update_t *g_insert_datareg_header(FILE *bin, string *tokens, int amnt_const, int const_size) {
    _reg_update_t *update = malloc(sizeof(*update));
    assert(update);
    
    update->is_removed = _g_is_rmv((*tokens));
    update->reg_size = _g_reg_get_size(tokens + amnt_const, const_size);

    /* fwrite & Error Handling */
    file_write(&(update->is_removed), sizeof(char), 1, bin);
    file_write(&(update->reg_size), sizeof(int), 1, bin);

    return update;
}


/*
    Reads first two fields of a register that starts at position pointed by fp
*/
_reg_update_t *_g_read_reg_header(FILE *fp) {
    _reg_update_t *reg_header = malloc(sizeof(*reg_header));

    
    file_read(&reg_header->is_removed, sizeof(char), 1, fp);
    file_read(&reg_header->reg_size, sizeof(int), 1, fp);
        
    return reg_header;
}

/* 
    For fields with variable size, use this funcion to read the field.
    Also used for strings with fixed size to put "\0" to print easily
*/
string g_read_str_field(FILE *fp, int field_size) {
    string str = NULL;

    if (field_size == 0) str = strdup("campo com valor nulo"); // In case is a mepty field
    
    else {
        str = realloc(str, sizeof(char) * (field_size + 1));
        file_read(str, sizeof(char), field_size, fp);
        str[field_size] = '\0';
    }

    return str;
}

/*
    Checks if file is consistent or not
*/
boolean check_consistency(FILE *bin) {
    if (!bin) return False;
   
    char status;
    fseek(bin, 0, SEEK_SET);
    file_read(&status, sizeof(char), 1, bin);

    return (status == CON_STAT) ? True : False;
}

/*
    Checks of parameters from terminal are correctly formatted 
*/
boolean check_terminal_parameters(string field, string value) {
    if (!field || !value) {
        printf("Quantidade de parâmetros inválida\n");
        return False;
    }

    return True;
}