#ifndef _VEHICLE_H
#define _VEHICLE_H

#include <stdio.h>

#include "stream.h"
#include "global.h"

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
#define V_REG_INFO_SIZE 5 
#define V_AMNT_TOKENS 6
// #define V_REG_PREFIX_OFFSET 5
// #define V_REG_DATE_OFFSET 10
// #define V_REG_SEAT_OFFSET 20
// #define V_REG_LINE_OFFSET 24
// #define V_REG_VAR_FIELDS_OFFSET 28

/* VEHICLE CSV TOKEN INDEXES */
enum {
    PREFIX=0,
    DATE,
    SEAT,
    LINE,
    MODEL,
    CATEGORY
};

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

/* GENERATE BINARY VEHICLE FILE FUNCTIONS */
/*
    Initializes all 'vehicle only' info of a vehicle header
*/
void v_header_init(struct _g_files *files);

/*
    Inserts all 'vehicle only' info of a new vehicle datareg
*/
void v_insert_datareg(FILE *bin, string *tokens);

/* 'SELECT WHERE' VEHICLE FUNCTIONS */
vehicle *v_select_where(FILE *bin, int offset, string field, string value);

string *v_read_tokens_from_terminal();

void vehicle_select(FILE *fp, int last_byte);

#endif