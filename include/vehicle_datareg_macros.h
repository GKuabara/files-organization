#ifndef _VEHIVLE_DATAREG_MACROS_H
#define _VEHIVLE_DATAREG_MACROS_H

#include "global_datareg_macros.h"

#define v_prefix_t char[5]
#define v_date_t char[10]
#define v_amnt_seats_t int
#define v_code_t int

#define v_model_t char *
#define v_category_t char *

#define v_model_size_t int
#define v_category_size_t int

#define v_reg_static_size 31
#define v_reg_info_size 31

enum {
    PREFIX=0,
    DATE,
    SEAT,
    LINE,
    MODEL,
    CATEGORY
};

#endif
