#ifndef _VEHICLE_H
#define _VEHICLE_H

#include <stdio.h>

#include "stream.h"
#include "global.h"

/* Vehicle const char fields */
#define v_prefix_t char[5]
#define v_date_t char[10]
#define v_prefix_desc_t char[18]
#define v_date_desc_t char[35]
#define v_seat_desc_t char[42]
#define v_line_desc_t char[26]
#define v_model_desc_t char[17]
#define v_category_desc_t char[20]

/* Const size 'vehicle only' */
#define V_CONST_REG_SIZE 31
#define V_HEADER_SIZE 175

/* vehicle.csv fields */
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
void v_header_init(struct _g_files *files);

/*
    Inserts all 'vehicle only' info of a new vehicle datareg
*/
void v_insert_datareg(FILE *bin, string *tokens);

void vehicle_select(FILE *fp, int last_byte);

#endif