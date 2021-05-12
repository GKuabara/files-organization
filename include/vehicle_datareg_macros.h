#ifndef _VEHIVLE_DATAREG_MACROS_H
#define _VEHIVLE_DATAREG_MACROS_H

#include "global_datareg_macros.h"

#define v_prefix_t char[5]
#define v_date_t char[10]
#define v_amnt_seats_t int
#define v_code_t int

#define v_model_t char *
#define v_category_t char*

#define v_model_size_t int
#define v_category_size_t int



struct _v_reg_update {
    g_removed_t is_removed;
    g_reg_size_t reg_size;
    v_model_size_t model_size;
    v_category_size_t category_size;
};



#endif
