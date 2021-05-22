#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vehicle.h"
#include "funcao-fornecida.h"

static void _v_write_prefix(FILE *bin, string prefix);
static void _v_write_date(FILE *bin, string date);
static void _v_write_int_fields(FILE *bin, string num_field);
static void _v_write_var_fields(FILE *bin, string field);

/*
    Writes the prefix of a new vehicle reg
*/
static void _v_write_prefix(FILE *bin, string prefix) {
    string aux = _g_is_rmv(prefix) == RMV ? prefix + 1 : prefix;
    
    /* fwrite Error Handling */
    if (fwrite(aux, sizeof(*prefix), 5, bin) != 5);
}

/*
    Writes the date of a new vehicle reg
*/
static void _v_write_date(FILE *bin, string date) {
    string aux = _g_is_null(date) ? "\0@@@@@@@@@" : date;
    
    /* fwrite Error Handling */
    if (fwrite(aux, sizeof(v_date_t), 1, bin) != 1);
}

/*
    Writes the int fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_int_fields(FILE *bin, string num_field) {
    int num = _g_is_null(num_field) ? -1 : atoi(num_field);

    /* fwrite Error Handling */
    if (fwrite(&num, sizeof(int), 1, bin) != 1);
}

/*
    Writes any variable fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_var_fields(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);

    /* fwrite Error Handling */
    if (fwrite(&len, sizeof(int), 1, bin) != 1);
    if (fwrite(field, sizeof(*field), len, bin) != len);
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
    Initializes all 'vehicle only' info of a vehicle header
*/
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
    
    for (string *t = tokens; *t; t++) free(*t); //TODO: make a function for this
    free(tokens);
    free(header);
}

static vehicle *_v_read_reg_data(FILE *fp) {

    vehicle *data = malloc(sizeof(*data));

    data->prefix = g_read_var_field(fp, 5);
    data->date = g_read_var_field(fp, 10);

    fread(&data->seats, sizeof(int), 1, fp);
    fread(&data->line, sizeof(int), 1, fp);
    
    fread(&data->model_size, sizeof(int), 1, fp);
    data->model = g_read_var_field(fp, data->model_size);

    fread(&data->category_size, sizeof(int), 1, fp);
    data->category = g_read_var_field(fp, data->category_size);

    return data;
}

static string get_month_name(string str) {
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

static void print_date(string date) {
    if(date[0] == '\0'){
        printf("Data de entrada do veiculo na frota: campo com valor nulo\n");
        return;
    }

    string *tokens = str_get_tokens(date, (struct _delim_t) {.amnt_delim=1,.delim=(char *[]){"-"}});

    string month_name = get_month_name(tokens[1]);
    printf("Data de entrada do veiculo na frota: %s de %s de %s\n", tokens[2], month_name, tokens[0]);

    for (string *aux = tokens; *aux; aux++) free(*aux);
    free(tokens);
    free(month_name);
}

static void print_seats(int seats) {
    if(seats == -1)
        printf("Quantidade de lugares sentados disponiveis: campo com valor nulo\n");
    else
        printf("Quantidade de lugares sentados disponiveis: %d\n", seats);

    return;
}

void vehicle_select(FILE *fp, int last_byte) {
    while(ftell(fp) < last_byte) {

        data_header *header = _g_read_reg_header(fp);
        //printf("removed: %c regsize: %d\n", header->removed, header->reg_size);

        if(header->removed == RMV) {
            int next_reg = ftell(fp) + header->reg_size;
            fseek(fp, next_reg, SEEK_SET);
            free(header);
            continue;
        }

        vehicle *data = _v_read_reg_data(fp);

        v_print_reg_data(data);

        free(header);
        free(data->date);
        free(data->prefix);
        free(data->model);
        free(data->category);
        free(data);
    }
}


/*
    Reads the num of the field to be select
*/
static int _v_which_selected_field(FILE *bin, string field, int offset) {
    if (strcmp(field, "prefixo") == 0) return PREFIX;
    if (strcmp(field, "data") == 0) return DATE;
    if (strcmp(field, "quantidadeLugares") == 0) return SEAT; 
    if (strcmp(field, "linha") == 0) return SEAT; 
    if (strcmp(field, "modelo") == 0) return MODEL;
    if (strcmp(field, "categoria") == 0) return CATEGORY;

    return -1; // Error handling
}

// struct _vehicle_reg *v_select_where(FILE *bin, int offset, string field, string value) {
//     ////////////////////////////////////////////////////////////////////////////
//     // if (g_read_rmv_field(bin, offset) == RMV) return NULL;
//     // 
//     // fseek(bin, V_REG_INFO_SIZE, offset);
//     // // struct _vehicle_reg *vehicle = malloc(sizeof(*vehicle)); // This is the 'select' part 
//     // vehicle->prefix = strdup(_read_prefix(bin, offset));
//     // vehicle->date = strdup(_read_date(bin, offset));
//     // vehicle->seats = _read_seats(bin, offset);
//     // vehicle->line = _read_line(bin, offset);
//     // vehicle->model = strdup(_read_model(bin, offset));
//     // vehicle->category = strdup(_read_category(bin, offset));
//     ////////////////////////////////////////////////////////////////////////////

//     switch (_which_selected_field(bin, field, offset)) {
//     case PREFIX:
//         if (strcmp(value, vehicle->prefix) == 0) return vehicle;
//         break;
//     case DATE:
//         if (strcmp(value, vehicle->date) == 0) return vehicle;
//         break;
//     case SEAT:
//         if (vehicle->seats == atoi(value)) return vehicle;
//         break;
//     case MODEL:
//         if (strcmp(value, vehicle->model) == 0) return vehicle;
//         break;
//     case CATEGORY:
//         if (strcmp(value, vehicle->category) == 0) return vehicle;
//         break;
//     default:
//         printf("Campo inexistente\n"); // Error handling
//         break;
//     }

//     // free(vehicle);
//     return NULL;
// }


// TODO: WTF IS THIS
string *v_read_tokens_from_terminal() {
    string *tokens = malloc(sizeof(*tokens) * V_AMNT_TOKENS);

    for (int i = 0; i < V_AMNT_TOKENS; ++i) {
        tokens[i] = calloc(sizeof(char), PAGE_SIZE);
        scan_quote_string(tokens[i]);
        tokens[i] = realloc(tokens[i], strlen(tokens[i]) + 1);
    }
    
    return tokens;
}