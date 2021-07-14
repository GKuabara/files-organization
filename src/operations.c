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
    Reads a csv file and inserts all dataregs from it. Can be used for both
    line and vehicle files given their respective `insert_funct` via `finfo`.
*/
static void _insert_dataregs_from_csv(files_t *files, void (*insert_funct)(FILE *, string *), long h_size, int amnt_const, int const_size) {
    g_header_update(files->bin, INC_STAT, 0, 0); // Makes the file inconsistent at the start

    int amnt_reg = 0;
    int amnt_rmv = 0;
    long next_reg = h_size;
    fseek(files->bin, next_reg, SEEK_SET);
    
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

    g_header_update(files->bin, CON_STAT, amnt_reg, amnt_rmv); // Update status only
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

/*
    Check files consistency
*/
static boolean _check_consistency_in_files(int n, ...) {
    va_list fps;
    va_start(fps, n);

    while (n--) {
        FILE *fp = va_arg(fps, FILE *);
        if (check_consistency(fp) == False && fp != NULL) {
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

/*
    Given a key c, searches this key in the index file and
    get the byte offset of a register to print through a pointer to function
*/
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
    Iterates through all line file registers comparing if any register code
    matches the line field in vehicle register
*/
static boolean _compare_all_registers(FILE *l_file, long l_end_of_file, vehicle *v_data) {
    fseek(l_file, L_HEADER_SIZE, SEEK_SET);

    while (ftell(l_file) < l_end_of_file) {
        _reg_update_t *l_reg_header = _g_read_reg_header(l_file);

        /* Error and removed regs handling */
        if (!l_reg_header) continue;
        if (l_reg_header->is_removed == RMV) {
            fseek(l_file, l_reg_header->reg_size, SEEK_CUR);
            free(l_reg_header);
            
            continue;
        }

        line *l_data = l_read_reg_data(l_file);

        // checks if a register from line file matches the vehicle register
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
    For each register in the vehicle file, searches for 
    registers with the same field (parameter) value in the line file
*/
static boolean _select_from(FILE *v_file, FILE *l_file, long v_end_of_file, long l_end_of_file) {
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

        vehicle *v_data = v_read_reg_data(v_file);

        // Compare vehicle register with all register of the line file
        boolean temp = _compare_all_registers(l_file, l_end_of_file, v_data);

        if (!has_reg && temp) has_reg = True;

        v_free_reg_data(v_data);
        free(v_reg_header);
    }
    
    return has_reg;
}

/*
    Using the btree, it searches for a match between a vehicle register
    and a line register through fields Line and codLinha, respectively
*/
static boolean _compare_all_registers_index(FILE *l_file, FILE *index, vehicle *v_data) {
    fseek(index, 0, SEEK_SET);

    bt_header_t *index_header = bt_header_load(index);

    // searches for a line register with codLinha field 
    // that matches field line of vehicle register
    long reg_byteoff_set = bt_search_key(index, index_header, v_data->line);

    // any register found
    if (reg_byteoff_set == -1) {
        free(index_header);
        return False; 
    }

    // printing vehicle and line registers
    v_print_reg_data(v_data);
    l_get_reg(l_file, reg_byteoff_set);
    
    free(index_header);
    return True;
}

/*
    For each register in the vehicle file, searches for 
    registers with the same field (parameter) value in the line file
    using index the btree file
*/
static boolean _select_from_index(FILE *v_file, FILE *l_file, FILE *index, long v_end_of_file) {
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
        
        vehicle *v_data = v_read_reg_data(v_file);
        
        // Compare vehicle register with all register of the line file
        boolean temp = _compare_all_registers_index(l_file, index, v_data);

        if (temp) has_reg = True;

        v_free_reg_data(v_data);
        free(v_reg_header);
    }
    
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
    
    // selecting print funcionality 5 or 11
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

    /* Error handling */
    if (!bin || (!index && index_file) || _check_consistency_in_files(2, bin, index) == False ||
        check_terminal_parameters(field, value) == False) {
        files_close(2, bin, index);
        return False;
    }

    boolean has_reg = False;
    long end_of_file = _get_end_of_file(bin);
    
    // selecting print funcionality 6 or 12
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

    _insert_dataregs_from_terminal(bin, index, amnt_regs, l_insert_datareg, next_reg, L_AMNT_REG_CONST, L_CONST_REG_SIZE);

    files_close(2, bin, index);    
    return True;
}

/*
    Ninth functionality: creates Index File (B-Tree) from vehicle Data File
*/
boolean vehicle_create_index(string bin_name, string index_name) {
    FILE *reg_bin = file_open(bin_name, "rb");
    FILE *index = file_open(index_name, "w+b");

    // Error handling
    if (!reg_bin || !index || _check_consistency_in_files(1, reg_bin) == False ) {
        files_close(2, reg_bin, index);
        return False;
    }

    long end_of_file = _get_end_of_file(reg_bin);

    // Case Data File has any registers    
    if (end_of_file < V_HEADER_SIZE)  {
        fclose(reg_bin);
        fclose(index);
        return False; 
    }   

    // Calls function that loops through all the Data File
    v_create_index_file(reg_bin, index, end_of_file);

    fclose(reg_bin);
    fclose(index);
 
    return True;
}

/*
    Tenth functionality: creates Index File (B-Tree) from line Data File
*/
boolean line_create_index(string bin_name, string index_name) {
    FILE *reg_bin = file_open(bin_name, "rb");
    FILE *index = file_open(index_name, "w+b");

    // Error handling
    if (!reg_bin || !index || _check_consistency_in_files(1, reg_bin) == False ) {
        files_close(2, reg_bin, index);
        return False;
    }

    long end_of_file = _get_end_of_file(reg_bin);
    
    // Case Data File has any registers
    if (end_of_file < L_HEADER_SIZE)  {
        fclose(reg_bin);
        fclose(index);
        return False; 
    }   

    // Calls function that loops through all the Data File
    l_create_index_file(reg_bin, index, end_of_file);

    fclose(reg_bin);
    fclose(index);

    return True;
}

boolean select_from(string vehicle_file, string line_file, string v_field, string l_field, string index_file) {
    FILE *v_file = file_open(vehicle_file, "rb");
    if (!v_file) return False;
    FILE *l_file = file_open(line_file, "rb");
    FILE *index = index_file ? file_open(index_file, "rb") : NULL;

        /* Error handling */
    if (!v_file || !l_file || _check_consistency_in_files(3, v_file, l_file, index) == False ||
        check_terminal_parameters(v_field, l_field) == False || check_field_parameters(v_field, l_field) == False) {
        files_close(3, v_file, l_file, index);
        return False;
    }

    long v_end_of_file = _get_end_of_file(v_file);
    long l_end_of_file = _get_end_of_file(l_file);

    boolean has_reg;
    if (index) 
        has_reg = _select_from_index(v_file, l_file, index, v_end_of_file);
    else
        has_reg = _select_from(v_file, l_file, v_end_of_file, l_end_of_file);

    if (has_reg == False) printf("Registro inexistente.\n");
    
    fclose(v_file);
    fclose(l_file);
    if (index) fclose(index);
    return has_reg;
}
