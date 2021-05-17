#ifndef _VEHICLE_H
#define _VEHICLE_H

#include <stdio.h>

#include "global_header_macros.h"
#include "vehicle_header_macros.h"
#include "global_datareg_macros.h"
#include "vehicle_datareg_macros.h"
#include "stream.h"
#include "global.h"

void v_header_init(struct _g_files *files);
void v_insert_datareg(FILE *bin, string *tokens);

#endif