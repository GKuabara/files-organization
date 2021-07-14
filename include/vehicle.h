#ifndef _VEHICLE_H
#define _VEHICLE_H

#include <stdio.h>

#include "stream.h"
#include "global.h"
#include "btree.h"
#include "file_handler.h"
#include "funcao_fornecida.h"

/* VEHICLE BINARY CONST CHAR FIELDS */
#define v_prefix_t char[5]
#define v_date_t char[10]
#define v_prefix_desc_t char[18]
#define v_date_desc_t char[35]
#define v_seat_desc_t char[42]
#define v_line_desc_t char[26]
#define v_model_desc_t char[17]
#define v_category_desc_t char[20]

/* CONST 'VEHICLE ONLY' BINARY SIZE REFERENCES */
#define V_CONST_REG_SIZE 31
#define V_HEADER_SIZE 175
#define V_REG_PREFIX_OFFSET 5
#define V_AMNT_REG_CONST 4


/* VEHICLE CSV TOKEN INDEXES */
enum {
    PREFIX=0,
    DATE,
    SEAT,
    LINE,
    MODEL,
    CATEGORY
};

/* USED TO FIND A MONTH NUMBER */
typedef struct {
    string *names;
    string *numbers;
} months;

/* VEHICLE REG STRUCTURE */
typedef struct{
    char *prefix;
    char *date;
    int seats;
    int line;
    int model_size;
    char *model;
    int category_size;
    char *category;
} vehicle;

/*
    Initializes all 'vehicle only' info of a vehicle header
*/
void v_header_init(files_t *files);

/*
    Free reg struct pointers
*/
void v_free_reg_data(vehicle *data);

/*
    Inserts all 'vehicle only' info of a new vehicle datareg
*/
void v_insert_datareg(FILE *bin, string *tokens);

/*
    Reads/Loads to memory a vehicle reg
*/
vehicle *v_read_reg_data(FILE *bin);

/*
    Print reg information from struct
*/
void v_print_reg_data(vehicle *data);

/*
    Selects/prints all non removed vehicle regs from a bin file
*/
boolean v_select(FILE *fp, int last_byte);

/*
    Print registers containing 'value' in the requested 'field'
*/
boolean v_select_where(FILE *bin, string field, string value, long end_of_file);

/*
    Create Index File from the Vehicle Data File
*/
void v_create_index_file(FILE *bin, FILE *index, long end_of_file);

/*
    Loads and print a register from Vehicle Data File given its offset
*/
void v_get_reg(FILE *bin, long offset);

#endif