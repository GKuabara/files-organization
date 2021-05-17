#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vehicle.h"

static void _v_write_prefix(FILE *bin, string prefix);
static void _v_write_date(FILE *bin, string date);
static void _v_write_num_fields(FILE *bin, string num_field, size_t field_size);
static void _v_write_var_fields(FILE *bin, string field);

static void _v_write_prefix(FILE *bin, string prefix) {
    if (_g_is_rmv(prefix) == RMV) {
        if (fwrite(prefix + 1, sizeof(*prefix), 5, bin) != 5);
    } else {
        if (fwrite(prefix, sizeof(*prefix), 5, bin) != 5);
    }
}

static void _v_write_date(FILE *bin, string date) {
    string aux = _g_is_null(date) ? "\0@@@@@@@@@" : date;
    
    if (fwrite(aux, sizeof(v_date_t), 1, bin) != 1);
}

static void _v_write_num_fields(FILE *bin, string num_field, size_t field_size) {
    int num = _g_is_null(num_field) ? -1 : atoi(num_field);

    if (fwrite(&num, field_size, 1, bin) != 1);
}

static void _v_write_var_fields(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);

    if (fwrite(&len, sizeof(int), 1, bin) != 1);
    if (fwrite(field, sizeof(*field), len, bin) != len);
}

void v_insert_datareg(FILE *bin, string *tokens) {
    _v_write_prefix(bin, tokens[PREFIX]);
    _v_write_date(bin, tokens[DATE]);
    _v_write_num_fields(bin, tokens[SEAT], sizeof(v_amnt_seats_t));
    _v_write_num_fields(bin, tokens[LINE], sizeof(v_code_t));
    _v_write_var_fields(bin, tokens[MODEL]);
    _v_write_var_fields(bin, tokens[CATEGORY]);
}

void v_header_init(struct _g_files *files) {
    /* Gets the header line of the csv */
    string header = readline(files->csv);
    string *tokens = str_get_tokens(header, (struct _delim_t) {.amnt_delim=2,.delim=csv_delim});

    /* Error handling */
    if (fwrite(tokens[PREFIX], sizeof(v_prefix_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[DATE], sizeof(v_date_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[SEAT], sizeof(v_seat_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[LINE], sizeof(v_line_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[MODEL], sizeof(v_model_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[CATEGORY], sizeof(v_category_desc_t), 1, files->bin) != 1);
    
    for (string *t = tokens; *t; t++) free(*t); // Fazer uma função pra
    free(tokens);
    free(header);
}