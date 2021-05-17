#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global_header_macros.h"
#include "vehicle_header_macros.h"
#include "global_datareg_macros.h"
#include "vehicle_datareg_macros.h"
#include "vehicle.h"


struct _g_files { // global.c
    FILE *bin;
    FILE *csv;
};

struct _reg_update {
    char is_removed;
    int reg_size;
};

// global.c
#define _g_is_null(token) (strcmp(token, "NULO") == 0 ? 1 : 0)
#define _g_is_rmv(prefix) (prefix[0] == '*' ? RMV : NRM)

// global.c
struct _g_files *g_open_files(string csv_name, string bin_name) {
    struct _g_files *files = malloc(sizeo(*files));
    assert(files); // In case of error

    files->csv = open_file(csv_name, "r");
    files->bin = open_file(bin_name, "w+b");

    return files;
}

// global.c
void g_header_init(FILE *bin, long header_size) {
    char stats = INC_STAT;
    int n_reg = 0;
    int amnt_removed_reg = 0;

    /* Error handling */
    if (fwrite(&stats, sizeof(char), 1, bin) != 1);
    if (fwrite(&header_size, sizeof(long), 1, bin) != 1);
    if (fwrite(&stats, sizeof(int), 1, bin) != 1);
    if (fwrite(&stats, sizeof(int), 1, bin) != 1);
}

// vehicle.c
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

// operations.c
static void _header_init(struct _g_files *files, long header_size, void (*header_funct)(struct _g_files *)) {
    g_header_init(files->bin, header_size);
    header_funct(files);
}

// global.c
static int _g_reg_get_size(string *var_fields, int const_size) {
    int size = 0;
    for (string *aux = var_fields; *aux; ++aux) 
        size += strlen(*aux);

    return size + const_size;
}

// global.c 
struct _reg_update *g_insert_datareg(FILE *bin, string *tokens, int amnt_const, int const_size) {
    struct _reg_update *update = malloc(sizeof(*update));
    assert(update);
    
    update->is_removed = _g_is_rmv((*tokens));
    update->reg_size = _g_reg_get_size(tokens + amnt_const, const_size);

    if (fwrite(&(update->is_removed), sizeof(char), 1, bin) != 1);
    if (fwrite(&(update->reg_size), sizeof(int), 1, bin) != 1);    

    return update;
}

// vehicle.c
static void _v_write_prefix(FILE *bin, string prefix) {
    if (_g_is_rmv(prefix) == RMV) {
        if (fwrite(prefix + 1, sizeof(*prefix), 4, bin) != 4);
    } else {
        if (fwrite(prefix, sizeof(*prefix), 5, bin) != 5);
    }
}

// vehicle.c
static void _v_write_date(FILE *bin, string date) {
    string aux = _g_is_null(date) ? "\0@@@@@@@@@" : date;
    
    if (fwrite(aux, sizeof(v_date_t), 1, bin) != 1);
}

// vehicle.c
static void _v_write_num_fields(FILE *bin, string num_field, size_t field_size) {
    int num = _g_is_null(num_field) ? -1 : strtol(num_field, NULL, 10);

    if (fwrite(&num, field_size, 1, bin) != 1);
}

// vehicle.c
static void _v_write_var_fields(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);

    if (fwrite(field, sizeof(*field), len, bin) != len);
}

// vehicle.c
void v_insert_datareg(FILE *bin, string *tokens) {
    _v_write_prefix(bin, tokens[PREFIX]);
    _v_write_date(bin, tokens[DATE]);
    _v_reg_write_num_fields(bin, tokens[SEAT], sizeof(int));
    _v_reg_write_num_fields(bin, tokens[LINE], sizeof(int));
    _v_write_var_fields(bin, tokens[MODEL]);
    _v_write_var_fields(bin, tokens[CATEGORY]);
}

// operations.c
static struct _reg_update *_insert_datareg(FILE *bin, long offset,  string *tokens, int amnt_const, int const_size, void (*insert_funct)(FILE *, string *)) {
    fseek(bin, offset, SEEK_SET);
   
    struct _reg_update *update = g_insert_datareg(bin, tokens, amnt_const, const_size);
    insert_funct(bin, tokens);

    return update;
}

// global.c
long g_header_update(FILE *bin, char stats, struct _reg_update *update, long offset, int amnt_reg, int amnt_rmv, int reg_info_size) {
    fseek(bin, 0, SEEK_SET);

    if (fwrite(&stats, sizeof(char), 1, bin) != 1);
    if (stats == CON_STAT) return 0; 

    long next_reg = update->reg_size + reg_info_size;
    if (fwrite(&next_reg, sizeof(long), 1, bin) != 1);
    
    if (fwrite(&amnt_reg, sizeof(int), 1, bin) != 1);
    if (fwrite(&amnt_rmv, sizeof(int), 1, bin) != 1);

    return next_reg;
}

