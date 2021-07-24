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


/*
    First functionality: creates a vehicle table.
*/
boolean vehicle_create_table(string _csv, string _bin) {
    files_t files = {.bin = file_open(_bin, "w+b"),
                      .csv = file_open(_csv, "r")};

    /* Error handling */
    if (!files.bin || !files.csv) {
        files_close(2, files.bin, files.csv);
        return False; 
    }

    aux_header_init(&files, V_HEADER_SIZE, v_header_init);
    aux_insert_regs_from_csv(&files, v_store_reg, V_HEADER_SIZE, V_AMNT_REG_CONST, V_CONST_REG_SIZE);
    
    files_close(2, files.bin, files.csv);
    return True;
}

/*
    Second functionality: creates a line table
*/
boolean line_create_table(string _csv, string _bin) {
    files_t files = {.bin = file_open(_bin, "w+b"),
                      .csv = file_open(_csv, "r")};

    /* Error handling */
    if (!files.bin || !files.csv) {
        files_close(2, files.bin, files.csv);
        return False; 
    }
    
    aux_header_init(&files, L_HEADER_SIZE, l_header_init);
    aux_insert_regs_from_csv(&files, l_store_reg, L_HEADER_SIZE, L_AMNT_REG_CONST, L_CONST_REG_SIZE);

    
    files_close(2, files.bin, files.csv);
    return True;
}

/*
    Third functionality: prints all dataregs from a vehicle file
*/
void vehicle_select(string _bin) {
    FILE *bin = file_open(_bin, "rb");

    /* Error handling */
    if (!bin || aux_check_consistency_in_files(1, bin) == False) {
        files_close(1, bin);
        return;
    }
    
    long eof = aux_get_end_of_file(bin);
    boolean has_reg = v_select_regs(bin, eof);

    /* In case no reg was found */
    if (has_reg == False) printf("Registro inexistente.\n");

    fclose(bin);
}

/*
    Forth functionality: prints all dataregs from a line file
*/
void line_select(string _bin) {
    FILE *bin = file_open(_bin, "rb");
    
    /* Error handling */
    if (!bin || aux_check_consistency_in_files(1, bin) == False) {
        files_close(1, bin);
        return;
    }

    long eof = aux_get_end_of_file(bin);
    boolean has_reg = l_select_regs(bin, eof);

    // In case no reg was found
    if (has_reg == False) printf("Registro inexistente.\n");
    
    fclose(bin);
}

/*
    Fifth | Eleventh funcionality: "SELECT WHERE" for vehicle files. Has been adapted suport a index file based selection.  
*/
void vehicle_select_where(string _reg, string _index, string field, string value) {
    FILE *bin = file_open(_reg, "rb");
    FILE *index = _index ? file_open(_index, "rb") : NULL;

    /* Error handling */
    if (!bin || (!index && _index) 
    || aux_check_consistency_in_files(2, bin, index) == False 
    || aux_check_terminal_parameters(field, value) == False) {
        return;
    }

    long eof = aux_get_end_of_file(bin);
    boolean has_reg; 
    
    /* Selects the functionality */
    if (index) 
        has_reg = aux_select_index(bin, index, convertePrefixo(value), v_print_reg_from_offset);
    else 
        has_reg = v_select_regs_where(bin, field, value, eof);
    
    /* In case no reg was found */
    if (has_reg == False) printf("Registro inexistente.\n");

    files_close(2, bin, index);
}

/*
    Sixth | Twelfth funcionality: "SELECT WHERE" for line files.
    Has been adapted suport a index file based selection.
*/
void line_select_where(string _reg, string _index, string field, string value) {
    FILE *bin = file_open(_reg, "rb");
    FILE *index = _index ? file_open(_index, "rb") : NULL;

    /* Error handling */
    if (!bin || (!index && _index) 
        || aux_check_consistency_in_files(2, bin, index) == False 
        || aux_check_terminal_parameters(field, value) == False) {
        files_close(2, bin, index);
        return;

    }

    boolean has_reg = False;
    long eof = aux_get_end_of_file(bin);
    
    /* Selects the functionality */
    if (index) 
        has_reg = aux_select_index(bin, index, atoi(value), l_print_reg_from_offset);
    else 
        has_reg = l_select_regs_where(bin, field, value, eof);

    /* In case no reg was found */
    if (has_reg == False) printf("Registro inexistente.\n");

    files_close(2, bin, index);;
}

/*
    Seventh | Tirteenth functionality: inserts new reg into vehicle binary 
    file. Has been adapted suport a index file based selection.
*/
boolean vehicle_insert_into(string _reg, string _index, int amnt_regs) {
    FILE *bin = file_open(_reg, "r+b");
    FILE *index = _index ? file_open(_index, "r+b") : NULL;

    /* Error handling */
    if (!bin || !(index && _index) || aux_check_consistency_in_files(2, bin, index) == False) {
        files_close(2, bin, index);
        return False;
    }

    /* Tries to get the next reg offset */
    long next_reg;
    if (fread(&next_reg, sizeof(long), 1, bin) != 1) {
        printf("Falha no processamento do arquivo.\n");
        return False;
    }

    aux_insert_dataregs_from_terminal(bin, index, amnt_regs, v_store_reg, next_reg, V_AMNT_REG_CONST, V_CONST_REG_SIZE);
    
    files_close(2, bin, index);
    return True;
}

/*
    Eighth | Fourteenth functionality: inserts new reg into line binary file.
    Has been adapted suport a index file based selection.
*/
boolean line_insert_into(string _bin, string _index, int amnt_regs) {
    FILE *bin = file_open(_bin, "r+b");
    FILE *index = _index ? file_open(_index, "r+b") : NULL;

    /* Error handling */
    if (!bin || !(index && _index) || aux_check_consistency_in_files(2, bin, index) == False) {
        files_close(2, bin, index);
        return False;
    }

    /* Tries to get the next reg offset */
    long next_reg;
    if (fread(&next_reg, sizeof(long), 1, bin) != 1) {
        printf("Falha no processamento do arquivo.\n");
        return False;
    }

    aux_insert_dataregs_from_terminal(bin, index, amnt_regs, l_store_reg, next_reg, L_AMNT_REG_CONST, L_CONST_REG_SIZE);

    files_close(2, bin, index);    
    return True;
}

/*
    Ninth functionality: creates an index file (B-Tree) from 
    vehicle bin
*/
boolean vehicle_create_index(string _bin, string _index) {
    FILE *bin = file_open(_bin, "rb");
    FILE *index = file_open(_index, "w+b");

    /* Error handling */
    if (!bin || !index || aux_check_consistency_in_files(1, bin) == False ) {
        files_close(2, bin, index);
        return False;
    }

    long eof = aux_get_end_of_file(bin);

    /* More error handling in case the are no regs */ 
    if (eof < V_HEADER_SIZE)  {
        files_close(2, bin, index);
        return False; 
    }   

    /* Calls function that loops through bin */
    v_index_file_init(bin, index, eof);

    files_close(2, bin, index);
    return True;
}

/*
    Tenth functionality: creates Index File (B-Tree) from line Data File
*/
boolean line_create_index(string _bin, string _index) {
    FILE *bin = file_open(_bin, "rb");
    FILE *index = file_open(_index, "w+b");

    /* Error handling */
    if (!bin || !index || aux_check_consistency_in_files(1, bin) == False ) {
        files_close(2, bin, index);
        return False;
    }

    long eof = aux_get_end_of_file(bin);
    
    /* More error handling in case the are no regs */ 
    if (eof < L_HEADER_SIZE)  {
        files_close(2, bin, index);
        return False; 
    }   

    /* Calls function that loops through bin */
    l_index_file_init(bin, index, eof);

    files_close(2, bin, index);
    return True;
}

/*
    Fifteenth | Sixteenth Functionality: selects and prints registers that matches line field. Has been adapted suport a index file based selection.
*/
void select_from(string _vehicle, string _line, string v_field, string l_field, string _index) {
    FILE *v_file = file_open(_vehicle, "rb");
    if (!v_file) return;
    FILE *l_file = file_open(_line, "rb");
    FILE *index = _index ? file_open(_index, "rb") : NULL;

    /* Error handling */
    if (!v_file || !l_file ||
        aux_check_consistency_in_files(3, v_file, l_file, index) == False ||
        aux_check_terminal_parameters(v_field, l_field) == False || aux_check_field_parameters(v_field, l_field) == False) {
        files_close(3, v_file, l_file, index);
        return;
    }

    long v_eof = aux_get_end_of_file(v_file);
    long l_eof = aux_get_end_of_file(l_file);

    /* Selects the functionality */
    boolean has_reg;
    if (index) 
        has_reg = aux_select_from_index(v_file, l_file, index, v_eof);
    else
        has_reg = aux_select_from(v_file, l_file, v_eof, l_eof);

    /* In case no reg was found */
    if (has_reg == False) printf("Registro inexistente.\n");
    
    files_close(3, v_file, l_file, index);
}

/*
    Seventeenth Functionality: from a binary vehicle file, creates a sorted one
*/
boolean vehicle_create_sorted_file(string _original, string _sorted, string field) {
    FILE *original = file_open(_original, "rb");
    FILE *sorted = file_open(_sorted, "w+b");

    /* Error handling */
    if (!original || !sorted || aux_check_consistency_in_files(1, original) == False ) {
        files_close(2, original, sorted);
        return False;
    }

    long eof = aux_get_end_of_file(original);
    int amnt_regs = g_header_read_amnt_regs(original);

    /* More error handling in case the are no regs */
    if (amnt_regs == 0  || eof < V_HEADER_SIZE) {
        files_close(2, original, sorted);
        return False;
    }

    /* Copies the header and sorts (in RAM) all regs from the original file*/
    v_copy_header(original, sorted);
    vehicle **regs = v_sort_regs_by_field(original, field, eof, amnt_regs);
    
    /* Writes to new `sorted` file and updates its consistency */
    v_store_regs(sorted, regs, amnt_regs);
    v_free_regs_data(regs, amnt_regs);
    g_header_update(sorted, CON_STAT, amnt_regs, 0);

    files_close(2, original, sorted);
    return True;
}

/*
    Eighteenth Functionality: from a binary line file, creates a sorted one
*/
boolean line_create_sorted_file(string _original, string _sorted, string field) {
    FILE *original = file_open(_original, "rb");
    FILE *sorted = file_open(_sorted, "w+b");

    /* Error handling */
    if (!original || !sorted || aux_check_consistency_in_files(1, original) == False ) {
        files_close(2, original, sorted);
        return False;
    }

    long eof = aux_get_end_of_file(original);
    int amnt_regs = g_header_read_amnt_regs(original);

    /* More error handling in case the are no regs */
    if (amnt_regs == 0  || eof < L_HEADER_SIZE) {
        files_close(2, original, sorted);
        return False;
    }

    /* Copies the header and sorts (IN RAM) all regs from the original file*/
    l_copy_header(original, sorted);    
    line **regs = l_sort_regs_by_field(original, field, eof, amnt_regs);
    
    /* Writes to new `sorted` file and updates its consistency */
    l_store_regs(sorted, regs, amnt_regs);
    l_free_regs_data(regs, amnt_regs);
    g_header_update(sorted, CON_STAT, amnt_regs, 0);

    files_close(2, original, sorted);
    return True;
}

/* 
    Nineteenth Functionality: Matches and prints the data from bin files given a target field.
*/
void match_files(string _vehicle, string _line, string v_field, string l_field) {
    vehicle_create_sorted_file(_vehicle, "vehicle_sorted", v_field);
    line_create_sorted_file(_line, "line_sorted", l_field);

    FILE *v_file = file_open("vehicle_sorted", "rb");
    FILE *l_file = file_open("line_sorted", "rb");

    /* Error handling */
    if (!v_file || !l_file || aux_check_consistency_in_files(2, v_file, l_file) == False ) {
        files_close(2, v_file, l_file);
        return;
    }

    /* If there are matching files */
    if (!aux_match_files(v_file, l_file, v_field, l_field))
        printf("Registro inexistente.\n");

    files_close(2, v_file, l_file);    
}