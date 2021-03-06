#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include "stream.h"
#include "file_handler.h"

/* Contains the firsts two fields of both vehicle and line data registers */
typedef struct  {
    char is_removed;
    int reg_size;
} _reg_update_t;

/* Boolean type */
#define boolean int
#define True 1
#define False 0

/* Removed options */
#define RMV '0' /* Removed */
#define NRM '1' /* Not Removed */

/* Status options */
#define INC_STAT '0' /* Inconsistent file */
#define CON_STAT '1' /* Consistent file */

/* Identifies files */
#define VEHICLE_FILE 1
#define LINE_FILE 2

#define G_CONST_REG_SIZE 5 
#define G_HEADER_SIZE 17

/* Check is field is null and removed */
#define _g_is_null(token) (strcmp(token, "NULO") == 0 ? 1 : 0)
#define _g_is_rmv(prefix) (prefix[0] == '*' ? RMV : NRM)

/* Size of string "campo com valor nulo" */
#define NULL_FIELD_ERROR_SIZE 21

/*
    Global start of an header. Can be used for both vehicle and line files
    given their total `header_size`.
*/
void g_header_init(FILE *bin, long header_size);

/*
    Updates the 'global' part of a header. Can be used for both vehicle and line files given `finfo`.
*/
void g_header_update(FILE *bin, char stats, int amnt_reg, int amnt_rmv);

/*
    Global part of a new regs insertion (remotion and reg size). Can be user for both vehicle and line files given `finfo`.
*/
_reg_update_t *g_insert_datareg_header(FILE *bin, string *tokens, int amnt_const, int const_size);

/*
    Reads first two fields of a register that starts at position pointed by fp
*/
_reg_update_t *_g_read_reg_header(FILE *fp);

/* 
    For fields with variable size, use this funcion to read the field.
    Also used for strings with fixed size to put "\0" to print easily
*/
string g_read_str_field(FILE *fp, int field_size);


/*
    Reads the amount of register in a binary file
*/
int g_header_read_amnt_regs(FILE *bin);

/*
    Checks if file is consistent or not
*/
boolean g_check_consistency(FILE *bin);

#endif