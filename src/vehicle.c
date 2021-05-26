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
    
    /* fwrite Error Handling */
    if (fwrite(aux, sizeof(*prefix), 5, bin) != 5)
        printf("Falha no processamento do arquivo.\n"); 
}

/*
    Writes the date of a new vehicle reg
*/
static void _v_write_date(FILE *bin, string date) {
    string aux = _g_is_null(date) ? "\0@@@@@@@@@" : date;
    
    /* fwrite Error Handling */
    if (fwrite(aux, sizeof(v_date_t), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n"); 
}

/*
    Writes the int fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_int_fields(FILE *bin, string num_field) {
    int num = _g_is_null(num_field) ? -1 : atoi(num_field);

    /* fwrite Error Handling */
    if (fwrite(&num, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n"); 
}

/*
    Writes any variable fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_var_fields(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);

    /* fwrite Error Handling */
    if (fwrite(&len, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n"); 
    if (fwrite(field, sizeof(*field), len, bin) != len)
        printf("Falha no processamento do arquivo.\n"); 
}

/*
    Reads/Loads to memory a vehicle reg
*/
static vehicle *_v_read_reg_data(FILE *bin) {
    vehicle *data = malloc(sizeof(*data));

    data->prefix = g_read_var_field(bin, 5);
    data->date = g_read_var_field(bin, 10);

    if (fread(&data->seats, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n"); 
    if (fread(&data->line, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n"); 
    
    if (fread(&data->model_size, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n"); 

    data->model = g_read_var_field(bin, data->model_size);

    if (fread(&data->category_size, sizeof(int), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n"); 
    data->category = g_read_var_field(bin, data->category_size);

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

    string month_name = NULL;
    for (int i = 0; i < 12; i++) {
        if (strcmp(str, month.numbers[i]) == 0) {
            month_name = realloc(month_name, strlen(month.names[i]) + 1);
            strcpy(month_name, month.names[i]);
        }
    }

    return month_name;
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
    
    // Error and removed regs handling
    if (!header) return NULL;
    if (header->is_removed == RMV) {
        fseek(bin, header->reg_size, SEEK_CUR);
        free(header);
        
        return NULL;
    }
    free(header);
    
    // Struct receives register content
    fseek(bin, V_REG_PREFIX_OFFSET, offset);
    vehicle *data = _v_read_reg_data(bin);

    // Gets the field we need to search for 'value', if the
    // register contains 'value', return struct with all reg data
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

    /* Error handling */
    if (fwrite(tokens[PREFIX], sizeof(v_prefix_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[DATE], sizeof(v_date_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[SEAT], sizeof(v_seat_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[LINE], sizeof(v_line_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[MODEL], sizeof(v_model_desc_t), 1, files->bin) != 1);
    if (fwrite(tokens[CATEGORY], sizeof(v_category_desc_t), 1, files->bin) != 1);
    
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
    fseek(bin, V_HEADER_SIZE, SEEK_SET);
    
    boolean has_reg = False;
    while (ftell(bin) < last_byte) {
        _reg_update_t *header = _g_read_reg_header(bin);

        if (header == NULL) {
            free(header);
            return False;
        }

        // If reg is removed, jump for the next register
        if (header->is_removed == RMV) {
            fseek(bin, header->reg_size, SEEK_CUR);
            free(header);
            continue;
        }

        has_reg = True;
        vehicle *data = _v_read_reg_data(bin);
        _v_print_reg_data(data);

        free(header);
        _v_free_reg_data(data);
    }

    return has_reg;
}

/*
    Print registers containing 'value' in the requested 'field'
*/
boolean v_select_where(FILE *bin, string field, string value, long end_of_file) {
    fseek(bin, V_HEADER_SIZE, SEEK_SET);

    if (_v_which_selected_field(field) == -1) {
        printf("Não existe esse campo no arquivo\n");
        return True;
    }

    long offset;
    boolean has_reg = False;
    while ((offset = ftell(bin)) < end_of_file) {
        vehicle *data = _v_get_selected_reg(bin, offset, field, value);

        if (data) {
            _v_print_reg_data(data);
            _v_free_reg_data(data);
            has_reg = True;
        }
    }

    return has_reg;
}
