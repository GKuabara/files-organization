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

static void _l_store_code(FILE *bin, string code);
static void _l_store_card_opt(FILE *bin, string card_opt);
static void _l_store_var_field(FILE *bin, string field);
static void _l_print_card(char card);
static int _l_which_field(string field);
static line *_l_get_selected_reg(FILE *bin, int offset, string field, string value);


/*
    Writes/Stores the code of a new line reg
*/
static void _l_store_code(FILE *bin, string code) {
    int aux = _g_is_rmv(code) == RMV ? atoi(code + 1) : atoi(code);
    file_write(&aux, sizeof(int), 1, bin);
}

/*
    Writes/Stores the card option of a new line reg
*/
static void _l_store_card_opt(FILE *bin, string card_opt) {
    char opt = (_g_is_null(card_opt))? '\0': *card_opt;
    file_write(&opt, sizeof(char), 1, bin);
}

/*
    Writes/Stores any variable fields of a new line reg. TODO: can be made global
*/
static void _l_store_var_field(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);
    file_write(&len, sizeof(int), 1, bin);
    file_write(field, sizeof(*field), len, bin);
}

/*
    Print string according with card
*/
static void _l_print_card(char card) {
    if (card == S) printf("Aceita cartao: PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR\n");
    else if (card == N) printf("Aceita cartao: PAGAMENTO EM CARTAO E DINHEIRO\n");
    else if (card == F) printf("Aceita cartao: PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n");
}

/*
    Gets the num of the line field to be select
*/
static int _l_which_field(string field) {
    if (strcmp(field, "codLinha") == 0) return CODE;
    if (strcmp(field, "aceitaCartao") == 0) return CARD;
    if (strcmp(field, "nomeLinha") == 0) return NAME; 
    if (strcmp(field, "corLinha") == 0) return COLOR; 

    return -1; // Error handling
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
    line *data = l_load_reg_data(bin);
    
    /* Gets the field we need to search for 'value', if the
    register contains 'value', return struct with all reg data */
    switch (_l_which_field(field)) {
    case CODE:
        if (atoi(value) == data->code) return data;
        break;
    case CARD:
        if (data->card == value[0]) return data;
        break;
    case NAME:
        if (strcmp(value, data->name) == 0) return data;
        break;
    case COLOR:
        if (strcmp(value, data->color) == 0) return data;
        break;
    default:
        break;
    }

    /* In case is not the value searched */
    l_free_reg_data(data);
    return NULL;
}

/*
    Initializes all ' only' info of a line header
*/
void l_header_init(files_t *files) {
    /* Gets the header line of the csv */
    string header = readline(files->csv);
    string *tokens = str_get_tokens(header, .amnt_delim=2,.delim=csv_delim);

    file_write(tokens[CODE], sizeof(l_code_desc_t), 1, files->bin);
    file_write(tokens[CARD], sizeof(l_card_desc_t), 1, files->bin);
    file_write(tokens[NAME], sizeof(l_name_desc_t), 1, files->bin);
    file_write(tokens[COLOR], sizeof(l_color_desc_t), 1, files->bin);
    
    /* Frees all alloc'ed memory */
    for (string *t = tokens; *t; t++) free(*t);
    free(tokens);
    free(header);
}

/*
    Inserts all 'line only' info of a new line datareg
*/
void l_store_reg(FILE *bin, string *tokens) {
    _l_store_code(bin, tokens[CODE]);
    _l_store_card_opt(bin, tokens[CARD]);
    _l_store_var_field(bin, tokens[NAME]);
    _l_store_var_field(bin, tokens[COLOR]);
}

/*
    Reads/Loads to memory the reg content to struct
*/
line *l_load_reg_data(FILE *fp) {
    line *data = malloc(sizeof(*data));

    file_read(&data->code, sizeof(int), 1, fp);
    file_read(&data->card, sizeof(char), 1, fp);

    file_read(&data->name_size, sizeof(int), 1, fp);
    data->name = g_read_str_field(fp, data->name_size);

    file_read(&data->color_size, sizeof(int), 1, fp);
    data->color = g_read_str_field(fp, data->color_size);

    return data;
}

/*
    Print reg information from struct
*/
void l_print_reg_data(line *data) {
    printf("Codigo da linha: %d\n", data->code);
    printf("Nome da linha: %s\n", data->name);
    printf("Cor que descreve a linha: %s\n", data->color);
    _l_print_card(data->card);
    printf("\n");
}

/*
    Loads and print a register from Line Data File given its offset
*/
void l_print_reg_from_offset(FILE *bin, long offset) {    
    fseek(bin, offset + G_CONST_REG_SIZE, SEEK_SET);
    
    line *data = l_load_reg_data(bin);
    l_print_reg_data(data);
  
    l_free_reg_data(data);
}

/* 
    Free struct and its elements
*/
void l_free_reg_data(line *data) {
    free(data->name);
    free(data->color);
    free(data);    
}

/*
    Prints every valid reg from a vehicle reg file
*/
boolean l_select_regs(FILE *bin, long last_byte) {
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
        line *data = l_load_reg_data(bin);
        l_print_reg_data(data);
        
        l_free_reg_data(data);
        free(reg_header);
    }

    return has_reg;
}

/*
    Print registers containing 'value' in the requested 'field'
*/
boolean l_select_regs_where(FILE *bin, string field, string value, long eof) {    
    fseek(bin, L_HEADER_SIZE, SEEK_SET); // Goes to the first reg

    /* In case a invalid field is given */
    if (_l_which_field(field) == -1) return False;
    
    boolean has_reg = False;
    long offset;
    while ((offset = ftell(bin)) < eof) {
        line *data = _l_get_selected_reg(bin, offset, field, value);

        if (data) {
            l_print_reg_data(data);
            l_free_reg_data(data);
            has_reg = True; // If at least 1 valid field is given
        }
    }

    return has_reg;
}

/*
    Creates/Initializes index file from the line bin file
*/
void l_index_file_init(FILE *reg_bin, FILE *index, long eof) {
    fseek(reg_bin, L_HEADER_SIZE, SEEK_SET); // Goes to te first reg in the vehicle bin file

    bt_header_t *header = bt_header_init(index); // Initializes the btree header

   // Loops through every reg in the vehicle file
    long p_r = -1;
    while ((p_r = ftell(reg_bin)) < eof) { 
        _reg_update_t *reg_header = _g_read_reg_header(reg_bin);

        /* Error and removed regs handling */
        if (!reg_header) continue;
        if (reg_header->is_removed == RMV) {
            fseek(reg_bin, reg_header->reg_size, SEEK_CUR);
            free(reg_header);
            
            continue;
        }

        line *data = l_load_reg_data(reg_bin);
        
        // Creates a new bt_key_t to be inserted in the btree
        bt_key_t *new_key = bt_node_key_init(data->code, p_r);
        bt_insert_key(index, header, new_key);
  
        l_free_reg_data(data);
        free(reg_header);
    }

    header->status = CON_STAT;
    bt_header_store(index, header);
    free(header);
}

/*
    Selects whar field we want to use to sort the file
*/
line  **l_sort_regs_by_field(FILE *original, string field, long eof, int amnt_regs) {
    int sort_field = _l_which_field(field);
    if (sort_field == -1) return NULL;

    line **regs = l_load_regs(original, eof, amnt_regs);

    switch (sort_field) {
        case CODE:
            qsort(regs, amnt_regs, sizeof(line*), l_code_field_comparator);
            break;
        case CARD:
            qsort(regs, amnt_regs, sizeof(line*), l_card_field_comparator);
            break;
        case NAME:
            qsort(regs, amnt_regs, sizeof(line*), l_name_field_comparator); 
            break;
        case COLOR:
            qsort(regs, amnt_regs, sizeof(line*), l_color_field_comparator); 
            break;
    }

    return regs;
}

/*
    Reads all data regs from the line bin file
*/
line **l_load_regs(FILE *bin, long eof, int amnt_regs) {
    line **regs = malloc(sizeof(*regs) * amnt_regs);
    
    fseek(bin, L_HEADER_SIZE, SEEK_SET);
    
    long offset;
    for (int i = 0; (offset = ftell(bin)) < eof;) {
        _reg_update_t *reg_header = _g_read_reg_header(bin);

        /* Error and removed regs handling */
        if (!reg_header) continue;
        if (reg_header->is_removed == RMV) {
            fseek(bin, reg_header->reg_size, SEEK_CUR);
            free(reg_header);
            
            continue;
        }

        regs[i++] = l_load_reg_data(bin);
        free(reg_header);
    }
    

    return regs;
}

/*
    Write in the 'bin' file, all line registers
*/
long l_store_regs(FILE *bin, line **regs, int amnt_regs) {
    fseek(bin, L_HEADER_SIZE, SEEK_SET);

    for (int i = 0; i < amnt_regs; ++i) {
        char is_rmv = NRM;
        int reg_size = L_CONST_REG_SIZE + regs[i]->name_size + regs[i]->color_size;

        file_write(&is_rmv, sizeof(char), 1, bin);
        file_write(&reg_size, sizeof(int), 1, bin);
        
        file_write(&regs[i]->code, sizeof(int), 1, bin);
        file_write(&regs[i]->card, sizeof(char), 1, bin);

        file_write(&regs[i]->name_size, sizeof(int), 1, bin);
        file_write(regs[i]->name, sizeof(char), regs[i]->name_size, bin);    

        file_write(&regs[i]->color_size, sizeof(int), 1, bin);
        file_write(regs[i]->color, sizeof(char), regs[i]->color_size, bin);
    }
} 

/*
    Frees all read dataregs 
*/
void l_free_regs_data(line **regs, int amnt_regs) {
    for (int i = 0; i < amnt_regs; i++) l_free_reg_data(regs[i]);
    free(regs);
}

/*
    Copy the header of the "original" to a new file
*/
void l_copy_header(FILE *original, FILE *copy) {
    g_header_init(copy, L_HEADER_SIZE); //Initializes the copy's data

    char code_desc[15];
    char card_desc[13]; 
    char name_desc[13];
    char color_desc[24];

    /* Reads and then copys the header descriptions */
    fseek(original, G_HEADER_SIZE, SEEK_SET);
    file_read(code_desc, sizeof(char), 15, original);
    file_read(card_desc, sizeof(char), 13, original);
    file_read(name_desc, sizeof(char), 13, original);
    file_read(color_desc, sizeof(char), 24, original);

    fseek(copy, G_HEADER_SIZE, SEEK_SET);
    file_write(code_desc, sizeof(char), 15, copy);
    file_write(card_desc, sizeof(char), 13, copy);
    file_write(name_desc, sizeof(char), 13, copy);
    file_write(color_desc, sizeof(char), 24, copy);
}