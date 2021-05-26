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
    int total_amnt_reg;

    fseek(bin, 0, SEEK_CUR);
    if (fread(&total_amnt_reg, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");
    
    total_amnt_reg += new_regs;
    fseek(bin, -sizeof(int), SEEK_CUR);
    if (fwrite(&total_amnt_reg, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");

}

/*
    Global start of an header. Can be used for both vehicle and line files
    given their total `header_size`.
*/
void g_header_init(FILE *bin, long header_size) {
    char stats = INC_STAT;
    int amnt_reg = 0;
    int amnt_rmv = 0;

    /* fwrite Error handling */
    if (fwrite(&stats, sizeof(char), 1, bin) != 1) 
        printf("Falha no processamento do arquivo.\n");
    if (fwrite(&header_size, sizeof(long), 1, bin) != 1) 
        printf("Falha no processamento do arquivo.\n");
    if (fwrite(&amnt_reg, sizeof(int), 1, bin) != 1) 
        printf("Falha no processamento do arquivo.\n");
    if (fwrite(&amnt_rmv, sizeof(int), 1, bin) != 1) 
        printf("Falha no processamento do arquivo.\n");
}

/*
    Updates the 'global' part of a header. Can be used for both vehicle and line files given `finfo`.
*/
void g_header_update(FILE *bin, char stats, int amnt_reg, int amnt_rmv) {
    long next_reg = ftell(bin);
    fseek(bin, 0, SEEK_SET);

    /* fwrite Error handling */
    if (fwrite(&stats, sizeof(char), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");
    if (stats == INC_STAT) return; 

    if (fwrite(&next_reg, sizeof(long), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");

    _g_update_amnt_reg(bin, amnt_reg);
    _g_update_amnt_reg(bin, amnt_rmv);
}

/*
    Global part of a new datareg insertion. Can be user for both vehicle and line files given `finfo`.
*/
_reg_update_t *g_insert_datareg(FILE *bin, string *tokens, int amnt_const, int const_size) {
    _reg_update_t *update = malloc(sizeof(*update));
    assert(update);
    
    update->is_removed = _g_is_rmv((*tokens));
    update->reg_size = _g_reg_get_size(tokens + amnt_const, const_size);

    /* fwrite Error Handling */
    if (fwrite(&(update->is_removed), sizeof(char), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");
    if (fwrite(&(update->reg_size), sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");

    return update;
}


/*
    Reads first two fields of a register that starts at position pointed by fp
*/
_reg_update_t *_g_read_reg_header(FILE *fp) {
    _reg_update_t *reg_header = malloc(sizeof(*reg_header));

    if (fread(&reg_header->is_removed, sizeof(char), 1, fp) != 1) {
        free(reg_header);
        return NULL;
    }
    
    if (fread(&reg_header->reg_size, sizeof(int), 1, fp) != 1) 
        printf("Falha no processamento do arquivo.\n"); 
        
    return reg_header;
}

/* 
    For fields with variable size, use this funcion to read the field.
    Also used for strings with fixed size to put "\0" to print easily
*/
string g_read_var_field(FILE *fp, int field_size) {
    string str = NULL;

    if (field_size == 0) {
        str = realloc(str, sizeof(char) * NULL_FIELD_ERROR_SIZE);
        strcpy(str, "campo com valor nulo");
    }
    else {
        str = realloc(str, sizeof(char) * (field_size + 1));

        fread(str, sizeof(char), field_size, fp);
        str[field_size] = '\0';
    }

    return str;
}

/*
    Checks if file is consistent or not
*/
boolean check_bin_consistency(FILE *bin) {
    fseek(bin, 0, SEEK_SET);
    
    char status;
    fread(&status, sizeof(char), 1, bin);

    boolean cons = (status == CON_STAT) ? True : False;
    return cons;     
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