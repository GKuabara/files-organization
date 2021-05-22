#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include "stream.h"


/* TODO: Maybe put in a file handler (?) */
struct _g_files { 
    FILE *bin;
    FILE *csv;
};

struct _reg_update {
    char is_removed;
    int reg_size;
};

typedef struct {
    char removed;
    int reg_size;
} data_header;

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


/* Removed options */
#define RMV '0' /* Removed */
#define NRM '1' /* Not Removed */

/* Status options */
#define INC_STAT '0' /* Inconsistent file */
#define CON_STAT '1' /* Consistent file */

#define VEHICLE_FILE 1
#define LINE_FILE 2

#define _g_is_null(token) (strcmp(token, "NULO") == 0 ? 1 : 0)
#define _g_is_rmv(prefix) (prefix[0] == '*' ? RMV : NRM)

#define NULL_FIELD_ERROR_SIZE 21

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

data_header *_g_read_reg_header(FILE *fp);

string g_read_var_field(FILE *fp, int field_size);


#endif