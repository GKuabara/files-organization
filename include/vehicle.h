#ifndef _VEHICLE_H
#define _VEHICLE_H

#include <stdio.h>

#include "stream.h"
#include "global.h"
#include "btree.h"
#include "fieldcmp.h"
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
    Inserts all 'vehicle only' info of a new vehicle datareg
*/
void v_store_reg(FILE *bin, string *tokens);

/*
    Reads/Loads to memory a vehicle reg
*/
vehicle *v_load_reg_data(FILE *bin);

/*
    Print reg information from struct
*/
void v_print_reg_data(vehicle *data);

/*
    Selects/prints all non removed vehicle regs from a bin file
*/
boolean v_select_regs(FILE *bin, long eof);

/*
    Selects/prints all non removed vehicle regs from a bin file
*/
boolean v_select_regs_where(FILE *bin, string field, string value, long eof);

/*
    Create Index File from the Vehicle Data File
*/
void v_index_file_init(FILE *reg_bin, FILE *index, long eof);

/*
    Selects which field we want to use to sort the file
*/
vehicle **v_sort_regs_by_field(FILE *original, string field, long eof, int amnt_regs);

/*
    Reads all data regs from the vehicle bin file
*/
vehicle **v_load_regs(FILE *bin, long eof, int amnt_regs);

/*
    Frees all read dataregs 
*/
void v_free_regs_data(vehicle **regs, int amnt_regs);

/*
    Write in the 'bin' file, all vehicle registers
*/
long v_store_regs(FILE *bin, vehicle **regs, int amnt_regs);

/*
    Free reg struct pointers
*/
void v_free_reg_data(vehicle *data);

/*
    Loads and print a register from Vehicle Data File given its offset
*/
void v_print_reg_from_offset(FILE *bin, long offset);

/*
    Copy the header of the "original" to a new file
*/
void v_copy_header(FILE *original, FILE *copy);

#endif