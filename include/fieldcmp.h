#ifndef _FIELDCMP_H
#define _FIELDCMP_H

#include "vehicle.h"
#include "line.h"

int v_prefix_field_comparator(const void *a, const void *b);
int v_date_field_comparator(const void *a, const void *b);
int v_seat_field_comparator(const void *a, const void *b);
int v_line_field_comparator(const void *a, const void *b);
int v_model_field_comparator(const void *a, const void *b);
int v_category_field_comparator(const void *a, const void *b);

int l_code_field_comparator(const void *a, const void *b);
int l_card_field_comparator(const void *a, const void *b);
int l_name_field_comparator(const void *a, const void *b);
int l_color_field_comparator(const void *a, const void *b);


#endif