#ifndef _LINE_H
#define _LINE_H

#include <stdio.h>

#include "stream.h"
#include "global.h"

/* LINE BINARY CONST CHAR FIELDS */
#define l_code_desc_t char[15]
#define l_card_desc_t char[13]
#define l_name_desc_t char[13]
#define l_color_desc_t char [24]

/* CONST 'LINE ONLY' BINARY SIZE REFERENCES */
#define L_HEADER_SIZE 82
#define L_CONST_REG_SIZE 13


/* CARD OPTIONS */
#define S 'S' 
#define N 'N' 
#define F 'F' 

/* LINE CSV TOKEN INDEXES */
enum {
    CODE,
    CARD,
    NAME,
    COLOR
};

/* LINE REG STRUCTURE */
struct _line_reg {
    int reg_size;
    int code;
    char card_opt;
    char *name;
    char *color;
};


/* GENERATE BINARY LINE FILE FUNCTIONS */
/*
    Inserts all 'line only' info of a new line datareg
*/
void l_insert_datareg(FILE *bin, string *tokens);

/*
    Initializes all ' only' info of a vehicle header
*/
void l_header_init(struct _g_files *files);


/* 'SELECT WHERE' LINE FUNCTIONS */
struct _line_reg *l_select_where(FILE *bin, int offset, string field, string value);


#endif
