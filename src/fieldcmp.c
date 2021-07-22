/*
** Comparator functions module for qsort and matching (not implemented yet).

**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fieldcmp.h"

int v_prefix_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->prefix, (*(vehicle **)b)->prefix);
}

int v_date_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->date, (*(vehicle **)b)->date);
}

int v_seat_field_comparator(const void *a, const void *b) {
    return (*(vehicle **)a)->seats - (*(vehicle **)b)->seats;
}

int v_line_field_comparator(const void *a, const void *b) {
    return (*(vehicle **)a)->line - (*(vehicle **)b)->line;
}

int v_model_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->model, (*(vehicle **)b)->model);
}

int v_category_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->category, (*(vehicle **)b)->category);
}

int l_code_field_comparator(const void *a, const void *b) {
    return (*(line **)a)->code - (*(line **)b)->code;
}

int l_card_field_comparator(const void *a, const void *b) {
    return (*(line **)a)->card - (*(line **)b)->card;
}

int l_name_field_comparator(const void *a, const void *b) {
    return strcmp((*(line **)a)->name, (*(line **)b)->name);
}

int l_color_field_comparator(const void *a, const void *b) {
    return strcmp((*(line **)a)->color, (*(line **)b)->color);
}

