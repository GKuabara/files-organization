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
boolean vehicle_create_table(string csv_name, string bin_name);

/*
    Creates a line table
*/
boolean line_create_table(string csv_name, string bin_name);

/*
    Inserts new reg into vehicle binary file
*/
void vehicle_insert_into(string bin_name, int amnt_regs);

/*
    Inserts new reg into line binary file
*/
void line_insert_into(string bin_name, int amnt_regs);

/*
    Third and Fourth funcionality "SELECT WHERE"
*/
boolean func_select(string bin_name, int select);

/*
    Sixth funcionality "SELECT WHERE"
*/
void line_select_where(string bin_name, string field, string value);

/*
    Sixth funcionality "SELECT WHERE"
*/
void vehicle_select_where(string bin_name, string field, string value);


#endif