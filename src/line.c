/*
** Line binary and csv files handling module.

**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/

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

    /* fwrite & Error Handling */
    if (fwrite(&aux, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
}

/*
    Writes the card option of a new line reg
*/
static void _l_write_card_opt(FILE *bin, string card_opt) {
    char opt = (_g_is_null(card_opt))? '\0': *card_opt;

    /* fwrite & Error Handling */
    if (fwrite(&opt, sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
}

/*
    Writes any variable fields of a new line reg. TODO: can be made global
*/
static void _l_write_var_field(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);

    /* fwrite & Error Handling */
    if (fwrite(&len, sizeof(int), 1, bin) != 1) 
        file_error("Falha no processamento do arquivo");
    if (fwrite(field, sizeof(*field), len, bin) != len) 
        file_error("Falha no processamento do arquivo");
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
    Reads/Loads to memory the reg content to struct
*/
static line *_l_read_reg_data(FILE *fp) {
    line *data = malloc(sizeof(*data));

    /* fwrite & Error handling */
    if (fread(&data->code, sizeof(int), 1, fp) != 1)
        file_error("Falha no processamento do arquivo");
    if (fread(&data->card, sizeof(char), 1, fp) != 1)
        file_error("Falha no processamento do arquivo");

    if (fread(&data->name_size, sizeof(int), 1, fp) != 1)
        file_error("Falha no processamento do arquivo");
    data->line_name = g_read_str_field(fp, data->name_size);

    if (fread(&data->color_size, sizeof(int), 1, fp) != 1)
        file_error("Falha no processamento do arquivo");
    data->color = g_read_str_field(fp, data->color_size);

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

/* Free struct and its elements */
static void _l_free_reg_data(line *data) {
    free(data->line_name);
    free(data->color);
    free(data);    
}

/*
    Reads reg and checks if it contains 'value' in 'field'
*/
static line *_l_get_selected_reg(FILE *bin, int offset, string field, string value) {
    _reg_update_t *reg_header = _g_read_reg_header(bin);

    /* Error and removed regs handling */
    if (!reg_header) return NULL;
    if (reg_header->is_removed == RMV) {
        fseek(bin, reg_header->reg_size, SEEK_CUR);
        
        free(reg_header);
        return NULL;
    }
    free(reg_header);
    
    /* Loads reg content to memory */
    fseek(bin, L_REG_CODE_OFFSET, offset);
    line *data = _l_read_reg_data(bin);
    
    /* Gets the field we need to search for 'value', if the
    register contains 'value', return struct with all reg data */
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

    /* In case is not the value searched */
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

    /* fwrite & Error handling */
    if (fwrite(tokens[CODE], sizeof(l_code_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[CARD], sizeof(l_card_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[NAME], sizeof(l_name_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[COLOR], sizeof(l_color_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    
    /* Frees all alloc'ed memory */
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
    Prints every valid reg from a vehicle reg file
*/
boolean l_select(FILE *bin, int last_byte) {
    fseek(bin, L_HEADER_SIZE, SEEK_SET); // Goes to the first reg
    boolean has_reg = False;
    
    while(ftell(bin) < last_byte) { 
        _reg_update_t *reg_header = _g_read_reg_header(bin);

        /* Error and removed regs handling */
        if (!reg_header) continue;
        if (reg_header->is_removed == RMV) {
            fseek(bin, reg_header->reg_size, SEEK_CUR);
            free(reg_header);
            
            continue;
        }

        has_reg = True; // If there is at leat 1 valid reg
        line *data = _l_read_reg_data(bin);
        _l_print_reg_data(data);
        
        _l_free_reg_data(data);
        free(reg_header);
    }

    return has_reg;
}

/*
    Print registers containing 'value' in the requested 'field'
*/
boolean l_select_where(FILE *bin, string field, string value, long end_of_file) {    
    fseek(bin, L_HEADER_SIZE, SEEK_SET); // Goes to the first reg

    /* In case a invalid field is given */
    if (_l_which_selected_field(field) == -1) return False;
    
    boolean has_reg = False;
    long offset;
    while ((offset = ftell(bin)) < end_of_file) {
        line *data = _l_get_selected_reg(bin, offset, field, value);

        if (data) {
            _l_print_reg_data(data);
            _l_free_reg_data(data);
            has_reg = True; // If at least 1 valid field is given
        }
    }

    return has_reg;
}

void l_create_index_file(FILE *reg_bin, FILE *index, long end_of_file) {
    bt_header_init(index); // Initializes the btree header
    
    fseek(reg_bin, L_HEADER_SIZE, SEEK_SET); // Goes to te first reg in the vehicle bin file
    
    int root_rrn = -1;
    int next_reg = 0;
    long p_r = -1;
   
   // Loops through every reg in the vehicle file
    while ((p_r = ftell(reg_bin)) < end_of_file) { 
        _reg_update_t *reg_header = _g_read_reg_header(reg_bin);

        /* Error and removed regs handling */
        if (!reg_header) continue;
        if (reg_header->is_removed == RMV) {
            fseek(reg_bin, reg_header->reg_size, SEEK_CUR);
            free(reg_header);
            
            continue;
        }

        line *data = _l_read_reg_data(reg_bin);
        
        // Creates a new key_pair to be inserted in the btree
        key_pair *new_key = malloc(sizeof(*new_key));
        new_key->c = data->code;
        // printf("chave: %d \t %x\n", new_key->c, new_key->c);
        new_key->p_r = p_r; 

        bt_insert_key(index, &root_rrn, &next_reg, new_key);
  
        _l_free_reg_data(data);
        free(reg_header);
    }

    
    bt_header_update(index, CON_STAT, root_rrn, next_reg);
}

void l_load_reg(FILE *bin, int offset) {    
    fseek(bin, offset, SEEK_SET);
    
    line *data = _l_read_reg_data(bin);
    _l_print_reg_data(data);
  
    _l_free_reg_data(data);
}