#ifndef _OPERATIONS_H
#define _OPERATIONS_H

#include <stdio.h>

#include "stream.h"
#include "global.h"
#include "vehicle.h"
#include "line.h"
#include "btree.h"
#include "aux.h"
#include "funcao_fornecida.h"

/*
    First functionality: creates a vehicle table.
*/
boolean vehicle_create_table(string _csv, string _bin);

/*
    Second functionality: creates a line table
*/
boolean line_create_table(string _csv, string _bin);

/*
    Third functionality: prints all dataregs from a vehicle file
*/
void vehicle_select(string _bin);

/*
    Forth functionality: prints all dataregs from a line file
*/
void line_select(string _bin);

/*
    Fifth | Eleventh funcionality: "SELECT WHERE" for vehicle files. Has been adapted suport a index file based selection.  
*/
void vehicle_select_where(string _reg, string _index, string field, string value);

/*
    Sixth | Twelfth funcionality: "SELECT WHERE" for line files.
    Has been adapted suport a index file based selection.
*/
void line_select_where(string _reg, string _index, string field, string value);

/*
    Seventh | Tirteenth functionality: inserts new reg into vehicle binary 
    file. Has been adapted suport a index file based selection.
*/
boolean vehicle_insert_into(string _reg, string _index, int amnt_regs);

/*
    Eighth | Fourteenth functionality: inserts new reg into line binary file.
    Has been adapted suport a index file based selection.
*/
boolean line_insert_into(string _bin, string _index, int amnt_regs);

/*
    Ninth functionality: creates an index file (B-Tree) from 
    vehicle bin
*/
boolean vehicle_create_index(string _bin, string _index);

/*
    Tenth functionality: creates Index File (B-Tree) from line Data File
*/
boolean line_create_index(string _bin, string _index);

/*
    Fifteenth | Sixteenth Functionality: selects and prints registers that matches line field. Has been adapted suport a index file based selection.
*/
void select_from(string _vehicle, string _line, string v_field, string l_field, string _index);

/*
    Seventeenth Functionality: from a binary vehicle file, creates a sorted one
*/
boolean vehicle_create_sorted_file(string _original, string _sorted, string field);

/*
    Eighteenth Functionality: from a binary line file, creates a sorted one
*/
boolean line_create_sorted_file(string _original, string _sorted, string field);

/* 
    Nineteenth Functionality: Matches and prints the data from bin files given a target field.
*/
void match_files(string _vehicle, string _line, string v_field, string l_field);

#endif