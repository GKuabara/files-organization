#ifndef _OPERATIONS_H
#define _OPERATIONS_H

#include <stdio.h>

#include "stream.h"
#include "global.h"
#include "vehicle.h"
#include "line.h"
#include "btree.h"
#include "funcao_fornecida.h"

/*
    First and Second Functionalities
*/
boolean vehicle_create_table(string csv_name, string bin_name);
boolean line_create_table(string csv_name, string bin_name);

/*
    Third and Fourth Functionalities
*/
boolean vehicle_select(string bin_name);
boolean line_select(string bin_name);

/*
    Fifth and Sixth Functionalities
*/
boolean vehicle_select_where(string bin_name, string index_file, string field, string value);
boolean line_select_where(string bin_name, string index_file, string field, string value);

/*
    Seventh and Eigth Functionalities
*/
boolean vehicle_insert_into(string reg_file, string index_file, int amnt_regs);
boolean line_insert_into(string bin_name, string index_file, int amnt_regs);

/*
    Ninth and Tenth Functionalities
*/
boolean vehicle_create_index(string bin_name, string index_name);
boolean line_create_index(string bin_name, string index_name);

/*
    Fifteenth and sixteenth Functionalities
*/
boolean select_from(string vehicle_file, string line_file, string index_file, string v_field, string l_field);

/*
    Seventeenth and Eighteenth Functionalities
*/
boolean vehicle_create_sorted_file(string original_name, string sorted_name, string field);
boolean line_create_sorted_file(string original_name, string sorted_name, string field);

/* 
    Nineteenth Functionality
*/
boolean merge_files_by_field(string v_name, string l_name, string v_field, string l_field);


#endif