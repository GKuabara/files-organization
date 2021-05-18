#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "line.h"

static void _l_write_code(FILE *bin, string code);
static void _l_write_card_opt(FILE *bin, string card_opt);
static void _l_write_var_field(FILE *bin, string field);

/*
    Initializes all ' only' info of a vehicle header
*/
void l_header_init(struct _g_files *files) {
    /* Gets the header line of the csv */
    string header = readline(files->csv);
    string *tokens = str_get_tokens(header, (struct _delim_t) {.amnt_delim=2,.delim=csv_delim});

    /* Error handling */
    if (fwrite(tokens[CODE], sizeof(l_code_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[CARD], sizeof(l_card_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[NAME], sizeof(l_name_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[COLOR], sizeof(l_color_desc_t), 1, files->bin) != 1);
    
    for (string *t = tokens; *t; t++) free(*t); //TODO: make a function for this
    free(tokens);
    free(header);
}

/*
    Writes the code of a new line reg
*/
static void _l_write_code(FILE *bin, string code) {
    int aux = _g_is_rmv(code) == RMV ? atoi(code + 1) : atoi(code);

    /* fwrite Error Handling */
    if (fwrite(&aux, sizeof(int), 1, bin) != 1);
}

/*
    Writes the card option of a new line reg
*/
static void _l_write_card_opt(FILE *bin, string card_opt) {
    char opt = *card_opt;

    /* fwrite Error Handling */
    if (fwrite(&opt, sizeof(char), 1, bin) != 1);
}

/*
    Writes any variable fields of a new line reg. TODO: can be made global
*/
static void _l_write_var_field(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);

    /* fwrite Error Handling */
    if (fwrite(&len, sizeof(int), 1, bin) != 1);
    if (fwrite(field, sizeof(*field), len, bin) != len);
}

/*
    Inserts all 'line only' info of a new line datareg
*/
void l_insert_datareg(FILE *bin, string *tokens) {
    _l_write_code(bin, tokens[CODE]);
    _l_write_card_opt(bin, tokens[CARD]);
    _l_write_var_field(bin, tokens[NAME]);
    _l_write_var_field(bin, tokens[COLOR]);
}


