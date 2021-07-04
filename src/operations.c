/*
** Database operations handling module.

**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "operations.h"

static void _header_init(files_t *files, long h_size, void (*header_funct)(files_t *));
static _reg_update_t *_insert_datareg(FILE *bin, string *tokens,  void (*insert_funct)(FILE *, string *), long next_reg, int amnt_const, int const_size);
static void _insert_dataregs_from_csv(files_t *files, void (*insert_funct)(FILE *, string *), long h_size, int amnt_const, int const_size);
static void _insert_dataregs_from_terminal(FILE *bin, FILE *index, int amnt_regs, void (*insert_funct)(FILE *, string *), long offset, int amnt_const, int const_size);
static boolean _select_index(FILE *reg_bin, FILE *index, int c, void (*load_funct)(FILE*, long));


/*
    Initializes a binary header. Can be used for both line and vehicle files
    given their respective `header_funct` via `finfo`. 
*/
static void _header_init(files_t *files, long h_size, void (*header_funct)(files_t *)) {
    g_header_init(files->bin, h_size);
    header_funct(files);
}

/* 
    Inserts a new datareg. Can be used for both line and vehicle files given
    their respective `insert_funct` via `finfo` 
*/
static _reg_update_t *_insert_datareg(FILE *bin, string *tokens,  void (*insert_funct)(FILE *, string *), long next_reg, int amnt_const, int const_size) {
    fseek(bin, next_reg, SEEK_SET);
    _reg_update_t *update = g_insert_datareg_header(bin, tokens, amnt_const, const_size);
    insert_funct(bin, tokens);

    return update;
}

/*
    Inserts a new key in a index file.
*/
static void _insert_index(FILE *index, bt_header_t *header, int c, long p_r) {
    if (!index || !header || c == -1) return;

    bt_key_t *new_key = bt_node_key_init(c, p_r);
    bt_insert_key(index, header, new_key);
}

static bt_header_t *_index_header_init(FILE *index) {
    if (!index) return NULL;
    
    bt_header_t *header = bt_header_load(index);
    header->status = INC_STAT;
    bt_header_store(index, header);

    return header;
}

static void _index_header_update(FILE *index, bt_header_t *header) {
    if (!index || !header) return;

    header->status = CON_STAT;
    bt_header_store(index, header);
    
    free(header);
}

/*
    Reads a csv file and inserts all dataregs from it. Can be used for both
    line and vehicle files given their respective `insert_funct` via `finfo`.
*/
static void _insert_dataregs_from_csv(files_t *files, void (*insert_funct)(FILE *, string *), long h_size, int amnt_const, int const_size) {
    g_header_update(files->bin, INC_STAT, 0, 0); // Makes the file inconsistent at the start

    int amnt_reg = 0;
    int amnt_rmv = 0;
    long next_reg = h_size;
    
    string line;
    while ((line = readline(files->csv)) != NULL) {
        /* Gets the csv line */
        string *tokens = str_get_tokens(line, .amnt_delim=2, .delim=csv_delim);

        _reg_update_t *update = _insert_datareg(files->bin, tokens, insert_funct, next_reg, amnt_const, const_size);
        update->is_removed == RMV ? amnt_rmv++ : amnt_reg++;
        next_reg += update->reg_size + G_CONST_REG_SIZE;

        /* Frees all aux structures */
        str_free_tokens(tokens);       
        free(update);
        free(line);
    }

    g_header_update(files->bin, CON_STAT, amnt_reg, amnt_rmv); // U´status only
}

/*
    Reads new regs from terminal to insert in the end of the binary files
*/
static void _insert_dataregs_from_terminal(FILE *bin, FILE *index, int amnt_regs, void (*insert_funct)(FILE *, string *), long offset, int amnt_const, int const_size) {
    g_header_update(bin, INC_STAT, 0, 0); // Makes the file inconsistent at the start

    bt_header_t *bt_header = _index_header_init(index);
    int amnt_reg = 0;
    int amnt_rmv = 0;
    long next_reg = offset;


    for (int i = 0; i < amnt_regs; ++i) {
        /* Gets line from terminal */
        string line = readline(stdin);
        string *tokens = str_get_tokens(line, .amnt_delim=1, .delim=(char *[]){" "});

        _reg_update_t *update = _insert_datareg(bin, tokens, insert_funct, next_reg, amnt_const, const_size);

        if (update->is_removed == NRM) {
            amnt_reg++;
            int c = (insert_funct == v_insert_datareg) ? convertePrefixo(tokens[PREFIX]) : atoi(tokens[CODE]);
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

static boolean _check_consistency_in_files(int n, ...) {
    va_list fps;
    va_start(fps, n);

    while (n--) {
        FILE *fp = va_arg(fps, FILE *);
        if (check_consistency(fp) == False && fp  != NULL) {
            printf("Falha no processamento do arquivo.\n");
            va_end(fps);
            return False;
        }
    }
    
    va_end(fps);
    return True;
}

/*
    Gets the offset of the last byte of a file. 
*/
static long _get_end_of_file(FILE *bin) {
    fseek(bin, 0, SEEK_END);
    long end_of_file = ftell(bin);

    return end_of_file;
} 

static boolean _select_index(FILE *reg_bin, FILE *index, int c, void (*load_funct)(FILE*, long)) {
    boolean has_reg = False;
    
    bt_header_t *header = bt_header_load(index);
    long p_r = bt_search_key(index, header, c); 
    
    if (p_r != -1) {
        load_funct(reg_bin, p_r);
        has_reg = True;
    }

    free(header);    
    return has_reg;
}

/*
    First functionality: creates a vehicle table.
*/
boolean vehicle_create_table(string csv_name, string bin_name) {
    files_t files = {.bin = file_open(bin_name, "w+b"),
                      .csv = file_open(csv_name, "r")};

    /* Error handling */
    if (!files.bin || !files.csv) {
        files_close(2, files.bin, files.csv);
        return False; 
    }

    _header_init(&files, V_HEADER_SIZE, v_header_init);
    _insert_dataregs_from_csv(&files, v_insert_datareg, V_HEADER_SIZE, V_AMNT_REG_CONST, V_CONST_REG_SIZE);
    
    files_close(2, files.bin, files.csv);
    return True;
}

/*
    Second functionality: creates a line table
*/
boolean line_create_table(string csv_name, string bin_name) {
    files_t files = {.bin = file_open(bin_name, "w+b"),
                      .csv = file_open(csv_name, "r")};

    /* Error handling */
    if (!files.bin || !files.csv) {
        files_close(2, files.bin, files.csv);
        return False; 
    }
    
    _header_init(&files, L_HEADER_SIZE, l_header_init);
    _insert_dataregs_from_csv(&files, l_insert_datareg, L_HEADER_SIZE, L_AMNT_REG_CONST, L_CONST_REG_SIZE);

    
    files_close(2, files.bin, files.csv);
    return True;
}

/*
    Third functionality: prints all dataregs from a vehicle file
*/
boolean vehicle_select(string bin_name) {
    FILE *bin = file_open(bin_name, "rb");

    /* Error handling */
    if (!bin || _check_consistency_in_files(1, bin) == False) {
        files_close(1, bin);
        return False;
    }
    
    long end_of_file = _get_end_of_file(bin);
    boolean has_reg = v_select(bin, end_of_file);

    if (has_reg == False) printf("Registro inexistente.\n");

    fclose(bin);
    return has_reg;
}

/*
    Forth functionality: prints all dataregs from a line file
*/
boolean line_select(string bin_name) {
    FILE *bin = file_open(bin_name, "rb");
    
    /* Error handling */
    if (!bin || _check_consistency_in_files(1, bin) == False) {
        files_close(1, bin);
        return False;
    }

    long end_of_file = _get_end_of_file(bin);
    boolean has_reg = l_select(bin, end_of_file);

    if (has_reg == False) printf("Registro inexistente.\n");
    
    fclose(bin);
    return has_reg;
}

/*
    Fifth funcionality: "SELECT WHERE" for vehicle files
*/
boolean vehicle_select_where(string reg_file, string index_file, string field, string value) {
    FILE *bin = file_open(reg_file, "rb");
    FILE *index = index_file ? file_open(index_file, "rb") : NULL;

    /* Error handling */
    if (!bin || (!index && index_file) || _check_consistency_in_files(2, bin, index) == False ||
        check_terminal_parameters(field, value) == False) {
        return False;
    }

    long end_of_file = _get_end_of_file(bin);
    boolean has_reg; 
    
    if (index) 
        has_reg = _select_index(bin, index, convertePrefixo(value), v_get_reg);
    else 
        has_reg = v_select_where(bin, field, value, end_of_file);
    
    if (has_reg == False) printf("Registro inexistente.\n");

    files_close(2, bin, index);
    return has_reg;
}

/*
    Sixth funcionality: "SELECT WHERE" for line files
*/
boolean line_select_where(string reg_file, string index_file, string field, string value) {
    FILE *bin = file_open(reg_file, "rb");
    FILE *index = index_file ? file_open(index_file, "rb") : NULL;

    if (!bin || (!index && index_file) || _check_consistency_in_files(2, bin, index) == False ||
        check_terminal_parameters(field, value) == False) {
        files_close(2, bin, index);
        return False;
    }

    boolean has_reg = False;
    long end_of_file = _get_end_of_file(bin);
    
    if (index) 
        has_reg = _select_index(bin, index, atoi(value), l_get_reg);
    else 
        has_reg = l_select_where(bin, field, value, end_of_file);

    if (has_reg == False) printf("Registro inexistente.\n");

    files_close(2, bin, index);
    return has_reg;
}

/*
    Seventh functionality: inserts new reg into vehicle binary file
*/
boolean vehicle_insert_into(string reg_file, string index_file, int amnt_regs) {
    FILE *bin = file_open(reg_file, "r+b");
    FILE *index = index_file ? file_open(index_file, "r+b") : NULL;

    /* Error handling */
    if (!bin || !(index && index_file) || _check_consistency_in_files(2, bin, index) == False) {
        files_close(2, bin, index);
        return False;
    }

    /* Tries to get the next reg offset */
    long next_reg;
    if (fread(&next_reg, sizeof(long), 1, bin) != 1) {
        printf("Falha no processamento do arquivo.\n");
        return False;
    }

    _insert_dataregs_from_terminal(bin, index, amnt_regs, v_insert_datareg, next_reg, V_AMNT_REG_CONST, V_CONST_REG_SIZE);
    
    files_close(2, bin, index);
    return True;
}

/*
    Eighth functionality: inserts new reg into line binary file
*/
boolean line_insert_into(string bin_name, string index_file, int amnt_regs) {
    FILE *bin = file_open(bin_name, "r+b");
    FILE *index = index_file ? file_open(index_file, "r+b") : NULL;

    /* Error handling */
    if (!bin || !(index && index_file) || _check_consistency_in_files(2, bin, index) == False) {
        files_close(2, bin, index);
        return False;
    }

    /* Tries to get the next reg offset */
    long next_reg;
    if (fread(&next_reg, sizeof(long), 1, bin) != 1) {
        printf("Falha no processamento do arquivo.\n");
        return False;
    }

    _insert_dataregs_from_terminal(bin, index, amnt_regs, l_insert_datareg, next_reg, 
    
    L_AMNT_REG_CONST, L_CONST_REG_SIZE);

    files_close(2, bin, index);    
    return True;
}


boolean vehicle_create_index(string bin_name, string index_name) {
    FILE *reg_bin = file_open(bin_name, "rb");
    FILE *index = file_open(index_name, "w+b");

    if (!reg_bin || !index || _check_consistency_in_files(1, reg_bin) == False ) {
        files_close(2, reg_bin, index);
        return False;
    }

    long end_of_file = _get_end_of_file(reg_bin);
        
    if (end_of_file < V_HEADER_SIZE)  {
        fclose(reg_bin);
        fclose(index);
        return False; 
    }   

    v_create_index_file(reg_bin, index, end_of_file);

    fclose(reg_bin);
    fclose(index);
 
    return True;
}

boolean line_create_index(string bin_name, string index_name) {
    FILE *reg_bin = file_open(bin_name, "rb");
    FILE *index = file_open(index_name, "w+b");

    if (!reg_bin || !index || _check_consistency_in_files(1, reg_bin) == False ) {
        files_close(2, reg_bin, index);
        return False;
    }

    long end_of_file = _get_end_of_file(reg_bin);
    
    if (end_of_file < L_HEADER_SIZE)  {
        fclose(reg_bin);
        fclose(index);
        return False; 
    }   

    l_create_index_file(reg_bin, index, end_of_file);

    fclose(reg_bin);
    fclose(index);

    return True;
}


