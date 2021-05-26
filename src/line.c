#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "line.h"
#include "funcao-fornecida.h"

static void _l_write_code(FILE *bin, string code);
static void _l_write_card_opt(FILE *bin, string card_opt);
static void _l_write_var_field(FILE *bin, string field);
static int _l_which_selected_field(string field);
static line *_l_read_reg_data(FILE *fp);
static void print_card(char card);
static void _l_print_reg_data(line *data);
static void _l_free_reg_data(line *data);
static line *_l_get_selected_reg(FILE *bin, int offset, string field, string value);


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
    char opt = (_g_is_null(card_opt))? '\0': *card_opt;


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
    Gets the num of the line field to be select
*/
static int _l_which_selected_field(string field) {
    if (strcmp(field, "codLinha") == 0) return CODE;
    if (strcmp(field, "aceitaCartao") == 0) return CARD;
    if (strcmp(field, "nomeLinha") == 0) return NAME; 
    if (strcmp(field, "corLinha") == 0) return COLOR; 

    return -1; // Error handling
}

/*
    Reads register content to struct
*/
static line *_l_read_reg_data(FILE *fp) {
    line *data = malloc(sizeof(*data));

    fread(&data->code, sizeof(int), 1, fp);
    fread(&data->card, sizeof(char), 1, fp);
    
    fread(&data->name_size, sizeof(int), 1, fp);
    data->line_name = g_read_var_field(fp, data->name_size);

    fread(&data->color_size, sizeof(int), 1, fp);
    data->color = g_read_var_field(fp, data->color_size);

    return data;
}

/*
    Print string according with card
*/
static void print_card(char card) {
    if (card == S) printf("Aceita cartao: PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR\n");
    else if (card == N) printf("Aceita cartao: PAGAMENTO EM CARTAO E DINHEIRO\n");
    else if (card == F) printf("Aceita cartao: PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n");
}

/*
    Print reg information from struct
*/
static void _l_print_reg_data(line *data) {
    printf("Codigo da linha: %d\n", data->code);
    printf("Nome da linha: %s\n", data->line_name);
    printf("Cor que descreve a linha: %s\n", data->color);
    print_card(data->card);
    printf("\n");
}

/* free struct and its elements */
static void _l_free_reg_data(line *data) {
    free(data->line_name);
    free(data->color);

    free(data);    
}

/*
    Reads reg and checks if it contains 'value' in 'field'
*/
static line *_l_get_selected_reg(FILE *bin, int offset, string field, string value) {
    _reg_update_t *header = _g_read_reg_header(bin);

    // Error and removed regs handling
    if (!header) return NULL;
    if (header->is_removed == RMV) {
        fseek(bin, header->reg_size, SEEK_CUR);
        free(header);
        
        return NULL;
    }
    free(header);
    
    // Struct receives register content
    fseek(bin, L_REG_CODE_OFFSET, offset);
    line *data = _l_read_reg_data(bin);
    
    // Gets the field we need to search for 'value', if the
    // register contains 'value', return struct with all reg data
    switch (_l_which_selected_field(field)) {
    case CODE:
        if (atoi(value) == data->code) return data;
        break;
    case CARD:
        if (data->card == value[0]) return data;
        break;
    case NAME:
        if (strcmp(value, data->line_name) == 0) return data;
        break;
    case COLOR:
        if (strcmp(value, data->color) == 0) return data;
        break;
    default:
        break;
    }

    _l_free_reg_data(data);
    return NULL;
}

/*
    Initializes all ' only' info of a vehicle header
*/
void l_header_init(_files_t *files) {
    /* Gets the header line of the csv */
    string header = readline(files->csv);
    string *tokens = str_get_tokens(header, .amnt_delim=2,.delim=csv_delim);

    /* Error handling */
    if (fwrite(tokens[CODE], sizeof(l_code_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[CARD], sizeof(l_card_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[NAME], sizeof(l_name_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[COLOR], sizeof(l_color_desc_t), 1, files->bin) != 1);
    
    for (string *t = tokens; *t; t++) free(*t);
    free(tokens);
    free(header);
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

/*
    Fourth functionality, prints every valid register
*/
boolean l_select(FILE *bin, int last_byte) {
    fseek(bin, L_HEADER_SIZE, SEEK_SET);
    boolean has_reg = False;
    
    while(ftell(bin) < last_byte) {
        _reg_update_t *header = _g_read_reg_header(bin);

        if (header == NULL) {
            free(header);
            return False;
        }

        // If reg is removed, jump for the next register
        if(header->is_removed == RMV) {
            int next_reg = ftell(bin) + header->reg_size;
            fseek(bin, next_reg, SEEK_SET);
            free(header);
            continue;
        }

        has_reg = True;
        line *data = _l_read_reg_data(bin);
        _l_print_reg_data(data);

        free(header);
        _l_free_reg_data(data);
    }

    return has_reg;
}

/*
    Print registers containing 'value' in the requested 'field'
*/
boolean l_select_where(FILE *bin, string field, string value, long end_of_file) {    
    fseek(bin, L_HEADER_SIZE, SEEK_SET);

    if (_l_which_selected_field(field) == -1) {
        printf("Não existe esse campo no arquivo\n");
        return True;
    }

    boolean has_reg = False;
    long offset;
    while ((offset = ftell(bin)) < end_of_file) {
        line *data = _l_get_selected_reg(bin, offset, field, value);

        if (data) {
            _l_print_reg_data(data);
            _l_free_reg_data(data);
            has_reg = True;
        }
    }

    return has_reg;
}