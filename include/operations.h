#ifndef _OPERATIONS_H
#define _OPERATIONS_H

#include <stdio.h>

#include "stream.h"
#include "global.h"
#include "vehicle.h"
#include "line.h"

/*
    Creates a vehicle table
*/
void vehicle_create_table(string csv_name, string bin_name);

/*
    Creates a line table
*/
void line_create_table(string csv_name, string bin_name);


void vehicle_insert_into(string bin_name, int amnt_regs);
void line_insert_into(string bin_name, int amnt_regs);

void func_select(string bin_name, int select);

void v_print_reg_data(vehicle *data);

#endif