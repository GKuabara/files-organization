#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include "stream.h"

/* TODO: Maybe put in a file handler (?) */
struct _g_files { 
    FILE *bin;
    FILE *csv;
};

/* Contains the firsts two fields of both vehicle and line data registers */
struct _reg_update {
    char is_removed;
    int reg_size;
};

/* Contains the header of the binary files, including two pointer to function */
struct _finfo {
    int amnt_const;
    int const_size;
    long header_size;
    long next_reg_offset;
    int amnt_reg;
    int amnt_rmv;
    void (*insert_funct)(FILE *, string *);
    void (*header_funct)(struct _g_files *);
};

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

/* Check is field is null and removed */
#define _g_is_null(token) (strcmp(token, "NULO") == 0 ? 1 : 0)
#define _g_is_rmv(prefix) (prefix[0] == '*' ? RMV : NRM)

/* Size of string "campo com valor nulo" */
#define NULL_FIELD_ERROR_SIZE 21

/* GLOBAL GENERATE BINARY FILE FUNCTIONS */
/*
    Opens both files. TODO: Maybe put in a file handler module (?)
*/
struct _g_files *g_open_files(string csv_name, string bin_name);

/*
    Global start of an header. Can be used for both vehicle and line files
    given their total `header_size`.
*/
void g_header_init(FILE *bin, long header_size);

/*
    Updates the 'global' part of a header. Can be used for both vehicle and line files given `finfo`.
*/
void g_header_update(FILE *bin, char stats, struct _reg_update *, struct _finfo *);

/*
    Global part of a new datareg insertion. Can be user for both vehicle and line files given `finfo`.
*/
struct _reg_update *g_insert_datareg(FILE *bin, string *tokens, struct _finfo *);

/* GLOBAL READ BINARY FUNCTIONS */
/*
    Reads the first byte (rmv status) of a reg. 
*/ 
int g_read_reg_rmv_stats(FILE *bin);

/*
    Reads the reg_size byte of a reg.
*/
int g_read_reg_size(FILE *bin);

/*
    Reads file's header fields: offset of next reg, #valid and #removed regs
*/
void g_read_header(FILE *bin, struct _finfo *finfo);

/*
    Reads first two fields of a register that starts at position pointed by fp
*/
struct _reg_update *_g_read_reg_header(FILE *fp);

/* 
    For fields with variable size, use this funcion to read the field.
    Also used for strings with fixed size to put "\0" to print easily
*/
string g_read_var_field(FILE *fp, int field_size);

/*
    Checks if file is consistent or not
*/
boolean check_bin_consistency(FILE *fp);

/*
    Checks of parameters from terminal are correctly formatted 
*/
boolean check_terminal_parameters(string field, string value) ;

#endif