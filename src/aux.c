#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aux.h"
#include "global.h"
#include "line.h"
#include "vehicle.h"
#include "funcao_fornecida.h"
#include "file_handler.h"

static void _insert_index(FILE *index, bt_header_t *header, int c, long p_r);
static bt_header_t *_index_header_init(FILE *index);
static void _index_header_update(FILE *index, bt_header_t *header);
static _reg_update_t *aux_insert_reg(FILE *bin, string *tokens,  void (*insert_funct)(FILE *, string *), long next_reg, int amnt_const, int const_size);
static boolean aux_compare_regs(FILE *l_file, long l_eof, vehicle *v_data);
static boolean aux_compare_regs_index(FILE *l_file, FILE *index, vehicle *v_data);


/* 
    Inserts a new reg. Can be used for both line and vehicle files given
    their respective `insert_funct`.
*/
static _reg_update_t *aux_insert_reg(FILE *bin, string *tokens,  void (*insert_funct)(FILE *, string *), long next_reg, int amnt_const, int const_size) {
    fseek(bin, next_reg, SEEK_SET);
    _reg_update_t *update = g_insert_datareg_header(bin, tokens, amnt_const, const_size);
    insert_funct(bin, tokens);

    return update;
}

/*
    Creates a new key pair and inserts it in the index file.
*/
static void _insert_index(FILE *index, bt_header_t *header, int c, long p_r) {
    if (!index || !header || c == -1) return;

    bt_key_t *new_key = bt_node_key_init(c, p_r);
    bt_insert_key(index, header, new_key);
}

/*
    Create and write a header in the index file
*/
static bt_header_t *_index_header_init(FILE *index) {
    if (!index) return NULL;
    
    bt_header_t *header = bt_header_load(index);
    header->status = INC_STAT;
    bt_header_store(index, header);

    return header;
}

/*
    Update data in the header of index file, usually in the end of all insertions
*/
static void _index_header_update(FILE *index, bt_header_t *header) {
    if (!index || !header) return;

    header->status = CON_STAT;
    bt_header_store(index, header);
    
    free(header);
}

/*
    Iterates through all line file regs checking if any reg code
    matches the line field in vehicle reg.
*/
static boolean aux_compare_regs(FILE *l_file, long l_eof, vehicle *v_data) {
    fseek(l_file, L_HEADER_SIZE, SEEK_SET);

    while (ftell(l_file) < l_eof) {
        _reg_update_t *l_reg_header = _g_read_reg_header(l_file);

        /* Error and removed regs handling */
        if (!l_reg_header) continue;
        if (l_reg_header->is_removed == RMV) {
            fseek(l_file, l_reg_header->reg_size, SEEK_CUR);
            free(l_reg_header);
            
            continue;
        }

        line *l_data = l_load_reg_data(l_file);

        /* If the reg from line file matches the vehicle reg */
        if (v_data->line == l_data->code){
            v_print_reg_data(v_data);
            l_print_reg_data(l_data);

            l_free_reg_data(l_data);
            free(l_reg_header);
            return True;
        }

        l_free_reg_data(l_data);
        free(l_reg_header);
    }
    
    return False;
}

/*
    Searches for a match between a vehicle reg
    and a line reg through fields Line and codLinha, respectively, using a btree.
*/
static boolean aux_compare_regs_index(FILE *l_file, FILE *index, vehicle *v_data) {
    fseek(index, 0, SEEK_SET);

    bt_header_t *index_header = bt_header_load(index);

    /* Searches for a line reg with codLinha field 
       that matches field line of vehicle reg */
    long reg_byteoff_set = bt_search_key(index, index_header, v_data->line);

    /* In case no reg is found */
    if (reg_byteoff_set == -1) { 
        free(index_header);
        return False; 
    }

    /* Prints vehicle and line regs */
    v_print_reg_data(v_data);
    l_print_reg_from_offset(l_file, reg_byteoff_set);
    
    free(index_header);
    return True;
}


/*
    Gets the offset of the last byte of a file. 
*/
long aux_get_end_of_file(FILE *bin) {
    fseek(bin, 0, SEEK_END);
    long eof = ftell(bin);

    return eof;
} 

/*
    Checks the consistency/status of multiple files.
*/
boolean aux_check_consistency_in_files(int n, ...) {
    va_list fps;
    va_start(fps, n);

    while (n--) {
        FILE *fp = va_arg(fps, FILE *);
        if (g_check_consistency(fp) == False && fp != NULL) {
            printf("Falha no processamento do arquivo.\n");
            va_end(fps);
            return False;
        }
    }
    
    va_end(fps);
    return True;
}

/*
    Checks if parameters from terminal are correctly formatted .
*/
boolean aux_check_terminal_parameters(string field, string value) {
    if (!field || !value) {
        printf("Quantidade de parâmetros inválida\n");
        return False;
    }

    return True;
}

/*
    Check if strings from terminal are "codLinha".
*/
boolean aux_check_field_parameters(string v_field, string l_field) {
    if (!v_field || !l_field) {
        printf("Quantidade de parâmetros inválida\n");
        return False;
    }

    if (strcmp(v_field, "codLinha") != 0 || strcmp(l_field, "codLinha") != 0) {
        printf("Campo inválido\n");
        return False;
    }
    return True;
}

/*
    Initializes a binary header. Can be used for both line and vehicle files
    given their respective `header_funct`.
*/
void aux_header_init(files_t *files, long h_size, void (*header_funct)(files_t *)) {
    g_header_init(files->bin, h_size);
    header_funct(files);
}

/*
    Reads a csv file and inserts all dataregs from it. Can be used for both
    line and vehicle files given their respective `insert_funct`.
*/
void aux_insert_regs_from_csv(files_t *files, void (*insert_funct)(FILE *, string *), long h_size, int amnt_const, int const_size) {
    g_header_update(files->bin, INC_STAT, 0, 0); // Makes the file inconsistent at the start

    int amnt_reg = 0;
    int amnt_rmv = 0;
    long next_reg = h_size;
    fseek(files->bin, next_reg, SEEK_SET);
    
    string line;
    while ((line = readline(files->csv)) != NULL) {
        /* Gets the csv line */
        string *tokens = str_get_tokens(line, .amnt_delim=2, .delim=csv_delim);

        _reg_update_t *update = aux_insert_reg(files->bin, tokens, insert_funct, next_reg, amnt_const, const_size);
        update->is_removed == RMV ? amnt_rmv++ : amnt_reg++;
        next_reg += update->reg_size + G_CONST_REG_SIZE;

        /* Frees all aux structures */
        str_free_tokens(tokens);       
        free(update);
        free(line);
    }

    g_header_update(files->bin, CON_STAT, amnt_reg, amnt_rmv); // Update status only
}

/*
    Reads new regs from terminal and appends them to the bin file.
*/
void aux_insert_dataregs_from_terminal(FILE *bin, FILE *index, int amnt_regs, void (*insert_funct)(FILE *, string *), long offset, int amnt_const, int const_size) {
    g_header_update(bin, INC_STAT, 0, 0); // Makes the file inconsistent at the start

    bt_header_t *bt_header = _index_header_init(index);
    int amnt_reg = 0;
    int amnt_rmv = 0;
    long next_reg = offset;


    for (int i = 0; i < amnt_regs; ++i) {
        /* Gets line from terminal */
        string line = readline(stdin);
        string *tokens = str_get_tokens(line, .amnt_delim=1, .delim=(char *[]){" "});

        _reg_update_t *update = aux_insert_reg(bin, tokens, insert_funct, next_reg, amnt_const, const_size);

        if (update->is_removed == NRM) {
            amnt_reg++;
            int c = (insert_funct == v_store_reg) ? convertePrefixo(tokens[PREFIX]) : atoi(tokens[CODE]);
            _insert_index(index, bt_header, c, next_reg);
        } else amnt_rmv++;
    
        next_reg += update->reg_size + G_CONST_REG_SIZE;

        /* Frees all aux structures */
        str_free_tokens(tokens);
        free(update);
        free(line);
    }

    g_header_update(bin, CON_STAT, amnt_reg, amnt_rmv);
    _index_header_update(index, bt_header);
}

/*
    Searches for an given reg in the index file given its key and
    prints if found. Returns True if the operations was sucessfull
    and False otherwise.
*/
boolean aux_select_index(FILE *bin, FILE *index, int c, void (*print_from_offset)(FILE*, long)) {
    boolean has_reg = False;
    
    bt_header_t *header = bt_header_load(index);
    long p_r = bt_search_key(index, header, c); 
    
    /* In case the key was not found in the index file */
    if (p_r != -1) {
        print_from_offset(bin, p_r);
        has_reg = True;
    }

    free(header);    
    return has_reg;
}

/*
    For each reg in the vehicle file, searches for 
    regs with the same field (parameter) value in the line file
*/
boolean aux_select_from(FILE *v_file, FILE *l_file, long v_eof, long l_eof) {
    fseek(v_file, V_HEADER_SIZE, SEEK_SET); // Goes to te first reg

    boolean has_reg = False;

    while (ftell(v_file) < v_eof) {
        _reg_update_t *v_reg_header = _g_read_reg_header(v_file);

        /* Error and removed regs handling */
        if (!v_reg_header) continue;
        if (v_reg_header->is_removed == RMV) {
            fseek(v_file, v_reg_header->reg_size, SEEK_CUR);
            free(v_reg_header);
            
            continue;
        }

        vehicle *v_data = v_load_reg_data(v_file);

        /* Compare vehicle reg with all reg of the line file */
        boolean temp = aux_compare_regs(l_file, l_eof, v_data);

        if (!has_reg && temp) has_reg = True;

        v_free_reg_data(v_data);
        free(v_reg_header);
    }
    
    return has_reg;
}

/*
    For each reg in the vehicle file, searches for 
    regs with the same field (parameter) value in the line file
    using index the btree file
*/
boolean aux_select_from_index(FILE *v_file, FILE *l_file, FILE *index, long v_end_of_file) {
    fseek(v_file, V_HEADER_SIZE, SEEK_SET); // Goes to te first reg
    boolean has_reg = False;

    while (ftell(v_file) < v_end_of_file) {
        _reg_update_t *v_reg_header = _g_read_reg_header(v_file);
        
        /* Error and removed regs handling */
        if (!v_reg_header) continue;
        if (v_reg_header->is_removed == RMV) {
            fseek(v_file, v_reg_header->reg_size, SEEK_CUR);
            free(v_reg_header);
            
            continue;
        }
        
        vehicle *v_data = v_load_reg_data(v_file);
        
        /* Compare vehicle reg with all reg of the line file */
        boolean temp = aux_compare_regs_index(l_file, index, v_data);

        if (temp) has_reg = True;

        v_free_reg_data(v_data);
        free(v_reg_header);
    }
    
    return has_reg;
}

/*
    Sorts all regs from both files and prints any matches. Returns True if there was at least 1 match and False otherwise.
*/
boolean aux_match_files(FILE *vehicle_bin, FILE *line_bin, string v_field, string l_field) {
    /* Reads and sorts all regs from both files */
    long v_eof = aux_get_end_of_file(vehicle_bin);
    int v_amnt_regs = g_header_read_amnt_regs(vehicle_bin);
    vehicle **v_regs = v_sort_regs_by_field(vehicle_bin, v_field, v_eof, v_amnt_regs);

    long l_eof = aux_get_end_of_file(line_bin);
    int l_amnt_regs = g_header_read_amnt_regs(line_bin);
    line **l_regs = l_sort_regs_by_field(line_bin, l_field, l_eof, l_amnt_regs);

    /* Matching algorithm */
    boolean has_reg = False;
    for (int i = 0, j = 0; i < v_amnt_regs && j < l_amnt_regs; ) {
        if (v_regs[i]->line == l_regs[j]->code) {
            has_reg = True;
            v_print_reg_data(v_regs[i]);
            l_print_reg_data(l_regs[j]);
        }
        (v_regs[i]->line <= l_regs[j]->code)? i++ : j++;
    }

    v_free_regs_data(v_regs, v_amnt_regs);
    l_free_regs_data(l_regs, l_amnt_regs);
    
    return has_reg;
}