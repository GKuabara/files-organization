/*
** Vehicle binary and csv files handling module.

**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vehicle.h"

static void _v_write_prefix(FILE *bin, string prefix);
static void _v_write_date(FILE *bin, string date);
static void _v_write_int_fields(FILE *bin, string num_field);
static void _v_write_var_fields(FILE *bin, string field);
static vehicle *_v_read_reg_data(FILE *bin);
static string _v_get_month_name(string str);
static void _v_print_date(string date);
static void _v_print_seats(int seats);
static int _v_which_selected_field(string field);
static void _v_free_reg_data(vehicle *data);
static vehicle *_v_get_selected_reg(FILE *bin, int offset, string field, string value);
static void _v_print_reg_data(vehicle *data);


/*
    Writes the prefix of a new vehicle reg
*/
static void _v_write_prefix(FILE *bin, string prefix) {
    string aux = _g_is_rmv(prefix) == RMV ? prefix + 1 : prefix;
    
    /* fwrite & Error Handling */
    if (fwrite(aux, sizeof(*prefix), 5, bin) != 5)
        file_error("Falha no processamento do arquivo");
}

/*
    Writes the date of a new vehicle reg
*/
static void _v_write_date(FILE *bin, string date) {
    string aux = _g_is_null(date) ? "\0@@@@@@@@@" : date;
    
    /* fwrite & Error Handling */
    if (fwrite(aux, sizeof(v_date_t), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
}

/*
    Writes the int fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_int_fields(FILE *bin, string num_field) {
    int num = _g_is_null(num_field) ? -1 : atoi(num_field);

    /* fwrite & Error Handling */
    if (fwrite(&num, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
}

/*
    Writes any variable fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_var_fields(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);

    /* fwrite & Error Handling */
    if (fwrite(&len, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(field, sizeof(*field), len, bin) != len)
        file_error("Falha no processamento do arquivo");
}

/*
    Reads/Loads to memory a vehicle reg
*/
static vehicle *_v_read_reg_data(FILE *bin) {
    vehicle *data = malloc(sizeof(*data));
    assert(data);   

    /* fwrite & Error handling */
    data->prefix = g_read_str_field(bin, 5);
    data->date = g_read_str_field(bin, 10);

    if (fread(&data->seats, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fread(&data->line, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    
    if (fread(&data->model_size, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");
    data->model = g_read_str_field(bin, data->model_size);

    if (fread(&data->category_size, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    data->category = g_read_str_field(bin, data->category_size);

    return data;
}

/*
    Converts the month's number to the month's name 
*/
static string _v_get_month_name(string str) {
    months month = {.names = (char *[]) {"janeiro", "fevereiro", "março", "abril", "maio", \
                "junho", "julho", "agosto", "setembro", "outubro", "novembro", "dezembro"}, \
                .numbers = (char *[]) {"01", "02", "03", "04", "05", "06", "07", "08", \
                "09", "10", "11", "12"}};

    for (int i = 0; i < 12; i++) {
        if (strcmp(str, month.numbers[i]) == 0) return strdup(month.names[i]);
    }
}

/*
    Prints the date vehicle reg
*/
static void _v_print_date(string date) {
    if (date[0] == '\0' ){
        printf("Data de entrada do veiculo na frota: campo com valor nulo\n");
        return;
    }

    string *tokens = str_get_tokens(date, .amnt_delim=1,.delim=(char *[]){"-"});

    string month_name = _v_get_month_name(tokens[1]);
    printf("Data de entrada do veiculo na frota: %s de %s de %s\n", tokens[2], month_name, tokens[0]);

    str_free_tokens(tokens);
    free(month_name);
}

/*
    Prints the amount of seats
*/
static void _v_print_seats(int seats) {
    if (seats == -1)
        printf("Quantidade de lugares sentados disponiveis: campo com valor nulo\n");
    else
        printf("Quantidade de lugares sentados disponiveis: %d\n", seats);

    return;
}

/*
    Selects what field was requested and return its number identifier
*/
static int _v_which_selected_field(string field) {
    if (strcmp(field, "prefixo") == 0) return PREFIX;
    if (strcmp(field, "data") == 0) return DATE;
    if (strcmp(field, "quantidadeLugares") == 0) return SEAT; 
    if (strcmp(field, "linha") == 0) return SEAT; 
    if (strcmp(field, "modelo") == 0) return MODEL;
    if (strcmp(field, "categoria") == 0) return CATEGORY;

    return -1; // Error handling
}

/*
    Print reg information from struct
*/
static void _v_print_reg_data(vehicle *data) {
    printf("Prefixo do veiculo: %s\n", data->prefix);
    printf("Modelo do veiculo: %s\n", data->model);
    printf("Categoria do veiculo: %s\n", data->category);
    
    _v_print_date(data->date);
    _v_print_seats(data->seats);
    printf("\n");
}

/*
    Free reg struct pointers
*/
static void _v_free_reg_data(vehicle *data) {
    free(data->prefix);
    free(data->date);
    free(data->model);
    free(data->category);
    free(data);    
}

static vehicle *_v_get_selected_reg(FILE *bin, int offset, string field, string value) {
    _reg_update_t *header = _g_read_reg_header(bin);
    
    /* Error and removed regs handling */
    if (!header) return NULL;
    if (header->is_removed == RMV) {
        fseek(bin, header->reg_size, SEEK_CUR);
        free(header);
        
        return NULL;
    }
    free(header);
    
    /* Loads new reg to memory */
    fseek(bin, V_REG_PREFIX_OFFSET, offset);
    vehicle *data = _v_read_reg_data(bin);

    /* Gets the field we need to search for 'value', if the
       register contains 'value', return struct with all reg data */
    switch (_v_which_selected_field(field)) {
    case PREFIX:
        if (strcmp(value, data->prefix) == 0) return data;
        break;
    case DATE:
        if (strcmp(value, data->date) == 0) return data;
        break;
    case SEAT:
        if (data->seats == atoi(value)) return data;
        break;
    case MODEL:
        if (strcmp(value, data->model) == 0) return data;
        break;
    case CATEGORY:
        if (strcmp(value, data->category) == 0) return data;
        break;
    default:
        break;
    }

    _v_free_reg_data(data);
    return NULL;
}

/*
    Initializes all 'vehicle only' info of a vehicle header
*/
void v_header_init(_files_t *files) {
    /* Gets the header line of the csv */
    string header = readline(files->csv);
    string *tokens = str_get_tokens(header, .amnt_delim=2,.delim=csv_delim);

    /* fwrite & Error handling */
    if (fwrite(tokens[PREFIX], sizeof(v_prefix_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[DATE], sizeof(v_date_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[SEAT], sizeof(v_seat_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[LINE], sizeof(v_line_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[MODEL], sizeof(v_model_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(tokens[CATEGORY], sizeof(v_category_desc_t), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    
    /* Frees all alloc'ed structures */
    str_free_tokens(tokens);
    free(header);
}

/*
    Inserts all 'vehicle only' info of a new vehicle datareg
*/
void v_insert_datareg(FILE *bin, string *tokens) {
    _v_write_prefix(bin, tokens[PREFIX]);
    _v_write_date(bin, tokens[DATE]);
    _v_write_int_fields(bin, tokens[SEAT]);
    _v_write_int_fields(bin, tokens[LINE]);
    _v_write_var_fields(bin, tokens[MODEL]);
    _v_write_var_fields(bin, tokens[CATEGORY]);
}

/*
    Selects/prints all non removed vehicle regs from a bin file
*/
boolean v_select(FILE *bin, int last_byte) {
    fseek(bin, V_HEADER_SIZE, SEEK_SET); // Goes to te first reg
    
    boolean has_reg = False;
    while (ftell(bin) < last_byte) {
        _reg_update_t *reg_header = _g_read_reg_header(bin);

        /* Error and removed regs handling */
        if (!reg_header) continue;
        if (reg_header->is_removed == RMV) {
            fseek(bin, reg_header->reg_size, SEEK_CUR);
            free(reg_header);
            
            continue;
        }

        vehicle *data = _v_read_reg_data(bin);
        _v_print_reg_data(data);
        has_reg = True; // If the is at leat 1 valid reg in the file
  
        _v_free_reg_data(data);
        free(reg_header);
    }

    return has_reg;
}

/*
    Print registers containing 'value' in the requested 'field'
*/
boolean v_select_where(FILE *bin, string field, string value, long end_of_file) {
    fseek(bin, V_HEADER_SIZE, SEEK_SET); // Goes to te first reg

    if (_v_which_selected_field(field) == -1) return False; // Error handling
    
    long offset;
    boolean has_reg = False;
    while ((offset = ftell(bin)) < end_of_file) {
        vehicle *data = _v_get_selected_reg(bin, offset, field, value);

        if (data) {
            _v_print_reg_data(data);
            _v_free_reg_data(data);
            has_reg = True; // If the is at leat 1 valid reg in the file
        }
    }

    return has_reg;
}


void v_create_index_file(FILE *reg_bin, FILE *index, long end_of_file) {
    bt_header_init(index); // Initializes the btree header
    
    fseek(reg_bin, V_HEADER_SIZE, SEEK_SET); // Goes to te first reg in the vehicle bin file
    
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

        vehicle *data = _v_read_reg_data(reg_bin);
        
        // Creates a new key_pair to be inserted in the btree
        key_pair *new_key = malloc(sizeof(*new_key));
        new_key->c = convertePrefixo(data->prefix);
        new_key->p_r = p_r; 

        bt_insert_key(index, &root_rrn, &next_reg, new_key);
  
        free(new_key);
        _v_free_reg_data(data);
        free(reg_header);
    }

    
    bt_header_update(index, CON_STAT, root_rrn, next_reg);
}