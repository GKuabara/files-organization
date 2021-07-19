#ifndef _LINE_H
#define _LINE_H

#include <stdio.h>

#include "stream.h"
#include "global.h"
#include "btree.h"
#include "funcao_fornecida.h"
#include "file_handler.h"

/* LINE BINARY CONST CHAR FIELDS */
#define l_code_desc_t char[15]
#define l_card_desc_t char[13]
#define l_name_desc_t char[13]
#define l_color_desc_t char [24]

/* CARD OPTIONS */
#define S 'S' 
#define N 'N' 
#define F 'F' 

/* CONST 'LINE ONLY' BINARY SIZE REFERENCES */
#define L_HEADER_SIZE 82
#define L_REG_CODE_OFFSET 5
#define L_CONST_REG_SIZE 13
#define L_AMNT_REG_CONST 2


/* LINE CSV TOKEN INDEXES */
enum {
    CODE,
    CARD,
    NAME,
    COLOR
};

/* LINE REG STRUCTURE */
typedef struct{
    int code;
    char card;
    int name_size;
    char *name;
    int color_size;
    char *color;
} line;

/*
    Inserts all 'line only' info of a new line datareg
*/
void l_insert_datareg(FILE *bin, string *tokens);

/*
    Initializes all ' only' info of a vehicle header
*/
void l_header_init(files_t *files);

/* 
    Free struct and its elements
*/
void l_free_reg_data(line *data);

/*
    Reads/Loads to memory the reg content to struct
*/
line *l_read_reg_data(FILE *fp);

/*
    Print reg information from struct
*/
void l_print_reg_data(line *data);

/*
    Fourth functionality, prints every valid register
*/
boolean l_select(FILE *fp, int last_byte);

/*
    Print registers containing 'value' in the requested 'field'
*/
boolean l_select_where(FILE *bin, string field, string value, long end_of_file);

/*
    Create Index File from the Line Data File
*/
void l_create_index_file(FILE *reg_bin, FILE *index, long end_of_file);

/*
    Loads and print a register from Line Data File given its offset
*/
void l_get_reg(FILE *bin, long offset);

/*
    Copy the header of the "original" to a new file
*/
void l_copy_header(FILE *original, FILE *copy);

/*
    Selects whar field we want to use to sort the file
*/
line  **l_sort_by_field(FILE *original, string field, long end_of_file, int amnt_regs);

/*
    Frees all read dataregs 
*/
void l_free_all_regs(line **regs, int amnt_regs);

/*
    Write in the 'bin' file, all line registers
*/
long l_write_all_regs(FILE *bin, line **regs, int amnt_regs);

/*
    Reads all data regs from the line bin file
*/
line **l_read_all_regs(FILE *bin, long end_of_file, int amnt_regs);

#endif
