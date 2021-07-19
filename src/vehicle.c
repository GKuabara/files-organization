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


static int _v_prefix_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->prefix, (*(vehicle **)b)->prefix);
}

static int _v_date_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->date, (*(vehicle **)b)->date);
}

static int _v_seat_field_comparator(const void *a, const void *b) {
    return (*(vehicle **)a)->seats - (*(vehicle **)b)->seats;
}

static int _v_line_field_comparator(const void *a, const void *b) {
    return (*(vehicle **)a)->line - (*(vehicle **)b)->line;
}

static int _v_model_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->model, (*(vehicle **)b)->model);
}

static int _v_category_field_comparator(const void *a, const void *b) {
    return strcmp((*(vehicle **)a)->category, (*(vehicle **)b)->category);
}

/*
    Writes the prefix of a new vehicle reg
*/
static void _v_write_prefix(FILE *bin, string prefix) {
    string aux = _g_is_rmv(prefix) == RMV ? prefix + 1 : prefix;    
    file_write(aux, sizeof(*prefix), 5, bin);
}

/*
    Writes the date of a new vehicle reg
*/
static void _v_write_date(FILE *bin, string date) {
    string aux = _g_is_null(date) ? "\0@@@@@@@@@" : date;    
    file_write(aux, sizeof(v_date_t), 1, bin);
}

/*
    Writes the int fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_int_fields(FILE *bin, string num_field) {
    int num = _g_is_null(num_field) ? -1 : atoi(num_field);
    file_write(&num, sizeof(int), 1, bin);
}

/*
    Writes any variable fields of a new vehicle reg. TODO: can be made global
*/
static void _v_write_var_fields(FILE *bin, string field) {
    int len = _g_is_null(field) ? 0 : strlen(field);
    file_write(&len, sizeof(int), 1, bin);
    file_write(field, sizeof(*field), len, bin);
}

/*
    Reads/Loads to memory a vehicle reg
*/
static vehicle *_v_read_reg_data(FILE *bin) {
    vehicle *data = malloc(sizeof(*data));
    assert(data);   

    data->prefix = g_read_str_field(bin, 5);
    data->date = g_read_str_field(bin, 10);

    file_read(&data->seats, sizeof(int), 1, bin);
    file_read(&data->line, sizeof(int), 1, bin);
   
    file_read(&data->model_size, sizeof(int), 1, bin);
    data->model = g_read_str_field(bin, data->model_size);

    file_read(&data->category_size, sizeof(int), 1, bin);
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
    if (strcmp(field, "codLinha") == 0) return LINE; 
    if (strcmp(field, "modelo") == 0) return MODEL;
    if (strcmp(field, "categoria") == 0) return CATEGORY;

    return -1; // Error handling
}

/*
    Print reg information from struct
*/
void v_print_reg_data(vehicle *data) {
    printf("Prefixo do veiculo: %s\n", data->prefix);
    printf("Modelo do veiculo: %s\n", data->model);
    printf("Categoria do veiculo: %s\n", data->category);
    
    _v_print_date(data->date);
    _v_print_seats(data->seats);
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

/*
    Reads reg and checks if it contains 'value' in 'field'
*/
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

long v_write_all_regs(FILE *bin, vehicle **regs, int amnt_regs) {
    fseek(bin, V_HEADER_SIZE, SEEK_SET);

    for (int i = 0; i < amnt_regs; ++i) {
        char is_rmv = NRM;
        int reg_size = V_CONST_REG_SIZE + regs[i]->model_size + regs[i]->category_size;

        file_write(&is_rmv, sizeof(char), 1, bin);
        file_write(&reg_size, sizeof(int), 1, bin);
        
        file_write(regs[i]->prefix, sizeof(char), 5, bin);
        file_write(regs[i]->date, sizeof(char), 10, bin);
        file_write(&regs[i]->seats, sizeof(int), 1, bin);
        file_write(&regs[i]->line, sizeof(int), 1, bin);

        file_write(&regs[i]->model_size, sizeof(int), 1, bin);
        file_write(regs[i]->model, sizeof(char), regs[i]->model_size, bin);    

        file_write(&regs[i]->category_size, sizeof(int), 1, bin);
        file_write(regs[i]->category, sizeof(char), (int)regs[i]->category_size, bin);
    }
} 


/*
    Reads all data regs from the vehicle bin file
*/
vehicle **v_read_all_regs(FILE *bin, long end_of_file, int amnt_regs) {
    vehicle **regs = malloc(sizeof(*regs) * amnt_regs);
    
    fseek(bin, V_HEADER_SIZE, SEEK_SET);
    
    long offset;
    for (int i = 0; (offset = ftell(bin)) < end_of_file;) {
        _reg_update_t *reg_header = _g_read_reg_header(bin);

        /* Error and removed regs handling */
        if (!reg_header) continue;
        if (reg_header->is_removed == RMV) {
            fseek(bin, reg_header->reg_size, SEEK_CUR);
            free(reg_header);
            
            continue;
        }

        regs[i++] = _v_read_reg_data(bin);
        free(reg_header);
    }
    

    return regs;
}

/*
    Frees all read dataregs 
*/
void v_free_all_regs(vehicle **regs, int amnt_regs) {
    for (int i = 0; i < amnt_regs; i++) _v_free_reg_data(regs[i]);
    free(regs);
}

/*
    Initializes all 'vehicle only' info of a vehicle header
*/
void v_header_init(files_t *files) {
    /* Gets the header line of the csv */
    string header = readline(files->csv);
    string *tokens = str_get_tokens(header, .amnt_delim=2,.delim=csv_delim);

    /* fwrite & Error handling */
    file_write(tokens[PREFIX], sizeof(v_prefix_desc_t), 1, files->bin);
    file_write(tokens[DATE], sizeof(v_date_desc_t), 1, files->bin);
    file_write(tokens[SEAT], sizeof(v_seat_desc_t), 1, files->bin);
    file_write(tokens[LINE], sizeof(v_line_desc_t), 1, files->bin);
    file_write(tokens[MODEL], sizeof(v_model_desc_t), 1, files->bin);
    file_write(tokens[CATEGORY], sizeof(v_category_desc_t), 1, files->bin);
    
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
        v_print_reg_data(data);
        printf("\n");
    
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
            v_print_reg_data(data);
            printf("\n");
            _v_free_reg_data(data);
            has_reg = True; // If the is at leat 1 valid reg in the file
        }
    }

    return has_reg;
}

/*
    Create Index File from the Vehicle Data File
*/
void v_create_index_file(FILE *reg_bin, FILE *index, long end_of_file) {
    fseek(reg_bin, V_HEADER_SIZE, SEEK_SET); // Goes to te first reg in the vehicle bin file

    bt_header_t *header = bt_header_init(index); 
   
   // Loops through every reg in the vehicle file
    long p_r = -1;
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
        
        // Creates a new bt_key_t to be inserted in the btree
        bt_key_t *new_key = bt_node_key_init(convertePrefixo(data->prefix), p_r);
        bt_insert_key(index, header, new_key);
  
        _v_free_reg_data(data);
        free(reg_header);
    }

    header->status = CON_STAT;
    bt_header_store(index, header);
    free(header);
}

/*
    Loads and print a register from Vehicle Data File given its offset
*/
void v_get_reg(FILE *bin, long offset) {    
    fseek(bin, offset + G_CONST_REG_SIZE, SEEK_SET);
    
    vehicle *data = _v_read_reg_data(bin);
    v_print_reg_data(data);
    printf("\n");
  
    _v_free_reg_data(data);
}

vehicle **v_sort_by_field(FILE *original, string field, long end_of_file, int amnt_regs) {
    int sort_field = _v_which_selected_field(field);
    if (sort_field == -1) False;

    vehicle **regs = v_read_all_regs(original, end_of_file, amnt_regs);

    switch (sort_field) {
        case PREFIX:
            qsort(regs, amnt_regs, sizeof(vehicle*), _v_prefix_field_comparator);
            break;
        case DATE:
            qsort(regs, amnt_regs, sizeof(vehicle*), _v_date_field_comparator);
            break;
        case SEAT:
            qsort(regs, amnt_regs, sizeof(vehicle*), _v_seat_field_comparator); 
            break;
        case LINE:
            qsort(regs, amnt_regs, sizeof(vehicle*), _v_line_field_comparator); 
            break;
        case MODEL:
            qsort(regs, amnt_regs, sizeof(vehicle*), _v_model_field_comparator);
            break;
        case CATEGORY:
            qsort(regs, amnt_regs, sizeof(vehicle*), _v_category_field_comparator);
            break;
    }

    return regs;
}

void v_copy_header(FILE *original, FILE *copy) {
    char stats;
    long next_reg;
    int amnt_reg;
    int amnt_rmv;
    char prefix_desc[18];
    char date_desc[35];
    char seat_desc[42];
    char line_desc[26];
    char model_desc[17];
    char category_desc[20];

    fseek(original, 0, SEEK_SET);
    file_read(&stats, sizeof(char), 1, original);
    file_read(&next_reg, sizeof(long), 1, original);
    file_read(&amnt_reg, sizeof(int), 1, original);
    file_read(&amnt_rmv, sizeof(int), 1, original);

    file_read(prefix_desc, sizeof(char), 18, original);
    file_read(date_desc, sizeof(char), 35, original);
    file_read(seat_desc, sizeof(char), 42, original);
    file_read(line_desc, sizeof(char), 26, original);
    file_read(model_desc, sizeof(char), 17, original);
    file_read(category_desc, sizeof(char), 20, original);


    fseek(copy, 0, SEEK_SET);
    file_write(&stats, sizeof(char), 1, copy);
    file_write(&next_reg, sizeof(long), 1, copy);
    file_write(&amnt_reg, sizeof(int), 1, copy);

    amnt_rmv = 0;
    file_write(&amnt_rmv, sizeof(int), 1, copy);

    file_write(prefix_desc, sizeof(char), 18, copy);
    file_write(date_desc, sizeof(char), 35, copy);
    file_write(seat_desc, sizeof(char), 42, copy);
    file_write(line_desc, sizeof(char), 26, copy);
    file_write(model_desc, sizeof(char), 17, copy);
    file_write(category_desc, sizeof(char), 20, copy);
}