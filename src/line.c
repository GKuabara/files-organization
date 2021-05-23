#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "line.h"
#include "funcao-fornecida.h"

/* GENERATE BINARY FILE AUX FUNCTIONS */
static void _l_write_code(FILE *bin, string code);
static void _l_write_card_opt(FILE *bin, string card_opt);
static void _l_write_var_field(FILE *bin, string field);

/* 'SELECT WHERE' AUX FUNCTIONS */
static int _l_which_selected_field(string field);


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

static void print_card(char card) {
    if (card == S) printf("Aceita cartao: PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR\n");
    else if (card == N) printf("Aceita cartao: PAGAMENTO EM CARTAO E DINHEIRO\n");
    else if (card == F) printf("Aceita cartao: PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n");
}

static void _l_print_reg_data(line *data) {
    printf("Codigo da linha: %d\n", data->code);
    printf("Nome da linha: %s\n", data->line_name);
    printf("Cor que descreve a linha: %s\n", data->color);
    print_card(data->card);
    printf("\n");
}

void line_select(FILE *fp, int last_byte) {
    while(ftell(fp) < last_byte) {

        data_header *header = _g_read_reg_header(fp);
        //printf("removed: %c regsize: %d\n", header->removed, header->reg_size);

        if(header->removed == RMV) {
            int next_reg = ftell(fp) + header->reg_size;
            fseek(fp, next_reg, SEEK_SET);
            free(header);
            continue;
        }

        line *data = _l_read_reg_data(fp);

        _l_print_reg_data(data);

        free(header);
        free(data->line_name);
        free(data->color);
        free(data);
    }
}

string *l_read_tokens_from_terminal() {
    string *tokens = malloc(sizeof(*tokens) * L_AMNT_TOKENS);

    for (int i = 0; i < L_AMNT_TOKENS; ++i) {
        tokens[i] = calloc(sizeof(char), PAGE_SIZE);
        scan_quote_string(tokens[i]);
        tokens[i] = realloc(tokens[i], strlen(tokens[i]) + 1);
    }
    
    return tokens;
}


static void _l_free_reg_data(line *data) {
    free(data->line_name);
    free(data->color);

    free(data);    
}

static line *_l_get_selected_reg(FILE *bin, int offset, string field, string value) {
    data_header *header = _g_read_reg_header(bin);

    if (!header) return NULL;

    if (header->removed == RMV) {
        fseek(bin, header->reg_size, SEEK_CUR);
        free(header);
        
        return NULL;
    }
    free(header);
    

    
    fseek(bin, L_REG_CODE_OFFSET, offset);
    line *data = _l_read_reg_data(bin);

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
        printf("Campo inexistente\n"); // Error handling
        break;
    }

    _l_free_reg_data(data);
    return NULL;
}

void l_select_where(FILE *bin, string field, string value) {
    fseek(bin, 0, SEEK_END);
    long end_of_file = ftell(bin);
    
    fseek(bin, L_HEADER_SIZE, SEEK_SET);

    long offset;
    while ((offset = ftell(bin)) < end_of_file) {
        line *data = _l_get_selected_reg(bin, offset, field, value);

        if (data) {
            _l_print_reg_data(data);
            _l_free_reg_data(data);
        }
    }
}