#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"



/*
    Initializes a binary header. Can be used for both line and vehicle files
    given their respective `header_funct` via `finfo`. 
*/
static void _header_init(_files_t *files, long h_size, void (*header_funct)(_files_t *)) {
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
    Reads a csv file and inserts all dataregs from it. Can be used for both
    line and vehicle files given their respective `insert_funct` via `finfo`.
*/
static void _insert_dataregs_from_csv(_files_t *files, void (*insert_funct)(FILE *, string *), long h_size, int amnt_const, int const_size) {
    g_header_update(files->bin, INC_STAT, 0, 0);

    int amnt_reg = 0;
    int amnt_rmv = 0;
    long next_reg = h_size;
    
    string line;
    while ((line = readline(files->csv)) != NULL) {
        string *tokens = str_get_tokens(line, .amnt_delim=2, .delim=csv_delim);

        _reg_update_t *update = _insert_datareg(files->bin, tokens,insert_funct, next_reg, amnt_const, const_size);
        update->is_removed == RMV ? amnt_rmv++ : amnt_reg++;
        next_reg += update->reg_size + G_CONST_REG_SIZE;

        /* Frees all aux structures */
        str_free_tokens(tokens);       
        free(update);
        free(line);
    }

    g_header_update(files->bin, CON_STAT, amnt_reg, amnt_rmv); // Update header status only
}

/*
    Reads new regs from terminal to insert in the end of the binary files
*/
static void _insert_dataregs_from_terminal(FILE *bin, int amnt_regs, void (*insert_funct)(FILE *, string *), long offset, int amnt_const, int const_size) {
    g_header_update(bin, INC_STAT, 0, 0);
    
    int amnt_reg = 0;
    int amnt_rmv = 0;
    long next_reg = offset;


    for (int i = 0; i < amnt_regs; ++i) {
        // Gets line from terminal
        string line = readline(stdin);
        string *tokens = str_get_tokens(line, .amnt_delim=1, .delim=(char *[]){" "});

        _reg_update_t *update = _insert_datareg(bin, tokens, insert_funct, next_reg, amnt_const, const_size);
        update->is_removed == RMV ? amnt_rmv++ : amnt_reg++;
        next_reg += update->reg_size + G_CONST_REG_SIZE;

        /* Frees all aux structures */
        str_free_tokens(tokens);
        free(update);
        free(line);
    }

    g_header_update(bin, CON_STAT, amnt_reg, amnt_rmv); // Update header status only
}

static long _get_end_of_file(FILE *bin) {
    fseek(bin, 0, SEEK_END);
    long end_of_file = ftell(bin);

    return end_of_file;
} 

/*
    First functionality: creates a vehicle table
*/
boolean vehicle_create_table(string csv_name, string bin_name) {
    _files_t files = {.bin = open_file(bin_name, "w+b"),
                      .csv = open_file(csv_name, "r")};

    if (!files.bin || !files.csv) return False;
    
    _header_init(&files, V_HEADER_SIZE, v_header_init);
    _insert_dataregs_from_csv(&files, v_insert_datareg, V_HEADER_SIZE, V_AMNT_REG_CONST, V_CONST_REG_SIZE);

    fclose(files.bin);
    fclose(files.csv);

    return True;
}

/*
    Second functionality: creates a line table
*/
boolean line_create_table(string csv_name, string bin_name) {
     _files_t files = {.bin = open_file(bin_name, "w+b"),
                      .csv = open_file(csv_name, "r")};

    if (!files.bin || !files.csv) return False;
    
    _header_init(&files, L_HEADER_SIZE, l_header_init);
    _insert_dataregs_from_csv(&files, l_insert_datareg, L_HEADER_SIZE, L_AMNT_REG_CONST, L_CONST_REG_SIZE);

    fclose(files.bin); 
    fclose(files.csv);
    
    return True;
}

/*
    Third functionality: prints all dataregs from a vehicle file
*/
boolean vehicle_select(string bin_name) {
    FILE *bin = open_file(bin_name, "rb");
    if (!bin) return False;

    if (check_bin_consistency(bin) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }

    long end_of_file = _get_end_of_file(bin);
    boolean has_reg = v_select(bin, end_of_file);

    if (has_reg == False) printf("Registro inexistente.\n");
    
    return has_reg;
}

/*
    Forth functionality: prints all dataregs from a line file
*/
boolean line_select(string bin_name) {
    FILE *bin = open_file(bin_name, "rb");
    if (!bin) return False;

    if (check_bin_consistency(bin) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }

    long end_of_file = _get_end_of_file(bin);
    boolean has_reg = l_select(bin, end_of_file);

    if (has_reg == False) printf("Registro inexistente.\n");
    
    return has_reg;
}

/*
    Fifth funcionality "SELECT WHERE"
*/
boolean vehicle_select_where(string bin_name, string field, string value) {
    FILE *bin = open_file(bin_name, "rb");
    if (!bin) return False;

    if (check_bin_consistency(bin) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }
   
    if (check_terminal_parameters(field, value) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }

    long end_of_file = _get_end_of_file(bin);
    boolean has_reg = v_select_where(bin, field, value, end_of_file);
    
    if (has_reg == False) printf("Registro inexistente.\n");

    return has_reg;
}

/*
    Sixth funcionality "SELECT WHERE"
*/
boolean line_select_where(string bin_name, string field, string value) {
    FILE *bin = open_file(bin_name, "rb");
    if (!bin) return False;
   
    if (check_terminal_parameters(field, value) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }

    if (check_bin_consistency(bin) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }

    long end_of_file = _get_end_of_file(bin);
    boolean has_reg = l_select_where(bin, field, value, end_of_file);
    
    if (has_reg == False) printf("Registro inexistente.\n");

    return has_reg;
}


/*
    Seventh functionality: inserts new reg into vehicle binary file
*/
boolean vehicle_insert_into(string bin_name, int amnt_regs) {
    FILE *bin = open_file(bin_name, "r+b");

    if (check_bin_consistency(bin) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }

    long next_reg;
    if (fread(&next_reg, sizeof(long), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");

    _insert_dataregs_from_terminal(bin, amnt_regs, v_insert_datareg, next_reg, V_AMNT_REG_CONST, V_CONST_REG_SIZE);
    
    fclose(bin);
    return True;
}

/*
    Eighty functionality: inserts new reg into line binary file
*/
boolean line_insert_into(string bin_name, int amnt_regs) {
    FILE *bin = open_file(bin_name, "r+b");

    if (check_bin_consistency(bin) == False) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return False;
    }

    long next_reg;
    if (fread(&next_reg, sizeof(long), 1, bin) != 1)
        printf("Falha no processamento do arquivo.\n");

    _insert_dataregs_from_terminal(bin, amnt_regs, l_insert_datareg, next_reg, L_AMNT_REG_CONST, L_CONST_REG_SIZE);
    
    fclose(bin);
    return True;
}




