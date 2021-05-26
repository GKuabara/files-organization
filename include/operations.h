#ifndef _OPERATIONS_H
#define _OPERATIONS_H

#include <stdio.h>

#include "stream.h"
#include "global.h"
#include "vehicle.h"
#include "line.h"


boolean vehicle_create_table(string csv_name, string bin_name);
boolean line_create_table(string csv_name, string bin_name);

boolean vehicle_select(string bin_name);
boolean line_select(string bin_name);


boolean vehicle_select_where(string bin_name, string field, string value);
boolean line_select_where(string bin_name, string field, string value);


boolean vehicle_insert_into(string bin_name, int amnt_regs);
boolean line_insert_into(string bin_name, int amnt_regs);

#endif