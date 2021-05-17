#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>

#include "global_header_macros.h"
#include "vehicle_header_macros.h"
#include "global_datareg_macros.h"
#include "vehicle_datareg_macros.h"
#include "stream.h"

struct _g_files { 
    FILE *bin;
    FILE *csv;
};

struct _reg_update {
    g_removed_t is_removed;
    g_reg_size_t reg_size;
};


#define _g_is_null(token) (strcmp(token, "NULO") == 0 ? 1 : 0)
#define _g_is_rmv(prefix) (prefix[0] == '*' ? RMV : NRM)


struct _g_files *g_open_files(string csv_name, string bin_name);

void g_header_init(FILE *bin, g_offset_t header_size);
g_offset_t g_header_update(FILE *bin, g_status_t stats, struct _reg_update *update, g_offset_t offset, int amnt_reg, int amnt_rmv, int reg_info_size);

struct _reg_update *g_insert_datareg(FILE *bin, string *tokens, int amnt_const, g_reg_size_t const_size);


#endif