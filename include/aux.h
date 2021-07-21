#ifndef _AUX_H
#define _AUX_H

#include <stdio.h>

#include "global.h"
#include "line.h"
#include "vehicle.h"
#include "funcao_fornecida.h"
#include "file_handler.h"

/*
    Gets the offset of the last byte of a file. 
*/
long aux_get_end_of_file(FILE *bin);

/*
    Check files consistency
*/
boolean aux_check_consistency_in_files(int n, ...);

/*
    Checks of parameters from terminal are correctly formatted 
*/
boolean aux_check_terminal_parameters(string field, string value);

/*
    Check if strings from terminal are "codLinha"
*/
boolean aux_check_field_parameters(string v_field, string l_field);

/*
    Initializes a binary header. Can be used for both line and vehicle files
    given their respective `header_funct` via `finfo`. 
*/
void aux_header_init(files_t *files, long h_size, void (*header_funct)(files_t *));

/*
    Reads a csv file and inserts all dataregs from it. Can be used for both
    line and vehicle files given their respective `insert_funct` via `finfo`.
*/
void aux_insert_regs_from_csv(files_t *files, void (*insert_funct)(FILE *, string *), long h_size, int amnt_const, int const_size);

/*
    Reads new regs from terminal to insert in the end of the binary files
*/
void aux_insert_dataregs_from_terminal(FILE *bin, FILE *index, int amnt_regs, void (*insert_funct)(FILE *, string *), long offset, int amnt_const, int const_size);

/*
    Given a key c, searches this key in the index file and
    get the byte offset of a register to print through a pointer to function
*/
boolean aux_select_index(FILE *bin, FILE *index, int c, void (*print_from_offset)(FILE*, long));

/*
    For each register in the vehicle file, searches for 
    registers with the same field (parameter) value in the line file
    using index the btree file
*/
boolean aux_select_from_index(FILE *v_file, FILE *l_file, FILE *index, long v_end_of_file);

/*
    For each register in the vehicle file, searches for 
    registers with the same field (parameter) value in the line file
*/
boolean aux_select_from(FILE *v_file, FILE *l_file, long v_eof, long l_eof);

/*
    Reads and sorts all regs from both files and print if they match their v_field
*/
boolean aux_match_files(FILE *vehicle_bin, FILE *line_bin, string v_field, string l_field);



#endif