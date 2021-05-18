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

#endif