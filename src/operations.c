#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"

/*
    Initializes a binary header. Can be used for both line and vehicle files
    given their respective `header_funct` via `finfo`. 
*/
static void _header_init(struct _g_files *files, struct _finfo *finfo) {
    g_header_init(files->bin, finfo->header_size);
    finfo->header_funct(files);
}

/* 
    Inserts a new datareg. Can be used for both line and vehicle files given
    their respective `insert_funct` via `finfo` 
*/
static struct _reg_update *_insert_datareg(FILE *bin, string *tokens, struct _finfo *finfo) {
    fseek(bin, finfo->next_reg_offset, SEEK_SET);
   
    struct _reg_update *update = g_insert_datareg(bin, tokens, finfo);
    finfo->insert_funct(bin, tokens);

    return update;
}

/*
    Reads an csv file and inserts all dataregs from it. Can be used for both
    line and vehicle files given their respective `insert_funct` via `finfo`.
*/
static void _insert_dataregs_from_csv(struct _g_files *files, struct _finfo *finfo) {
    string line;

    while ((line = readline(files->csv)) != NULL) {
        string *tokens = str_get_tokens(line,  
            (struct _delim_t) {.amnt_delim=2, .delim=csv_delim});
        
        struct _reg_update *update = _insert_datareg(files->bin, tokens, finfo);

        if (update->is_removed == RMV) finfo->amnt_rmv++;
        else finfo->amnt_reg++;

        g_header_update(files->bin, INC_STAT, update, finfo); // Updates the header

        /* Frees all aux structures */
        for (string *t = tokens; *t; ++t) free(*t); //TODO: Make a function for this
        free(tokens);       
        free(line);
        free(update);
    }

    g_header_update(files->bin, CON_STAT, NULL, finfo); // Update header status only
}

static void _insert_dataregs_from_terminal(FILE *bin, struct _finfo *finfo, int amnt_regs, string *(*read_terminal_tokens)(void)) {
    for (int i = 0; i < amnt_regs; ++i) {
        string *tokens = read_terminal_tokens();
        struct _reg_update *update = _insert_datareg(bin, tokens, finfo);
        finfo->amnt_reg++;
        g_header_update(bin, INC_STAT, update, finfo); // Updates the header

        /* Frees all aux structures */
        for (string *t = tokens; *t; ++t) free(*t); //TODO: Make a function for thi
        free(update);
    }

    g_header_update(bin, CON_STAT, NULL, finfo); // Update header status only
}

static void _select_where(FILE *bin, int ftype, string field, string value) {
    if (ftype == VEHICLE_FILE) 
        v_select_where(bin, field, value);
    else if (ftype == LINE_FILE)
        l_select_where(bin, field, value);

}


/*
    Creates a vehicle table
*/
void vehicle_create_table(string csv_name, string bin_name) {
    struct _g_files *files = g_open_files(csv_name, bin_name);
    struct _finfo vinfo = {.amnt_const = 4, 
                           .amnt_reg = 0,
                           .amnt_rmv = 0,
                           .next_reg_offset = V_HEADER_SIZE,
                           .const_size = V_CONST_REG_SIZE, 
                           .header_funct = v_header_init, 
                           .header_size = V_HEADER_SIZE, 
                           .insert_funct = v_insert_datareg};

    
    _header_init(files, &vinfo);
    _insert_dataregs_from_csv(files, &vinfo);

    fclose(files->bin); //TODO: Make a function for this
    fclose(files->csv);
    free(files);  
}

/*
    Creates a line table
*/
void line_create_table(string csv_name, string bin_name) {
    struct _g_files *files = g_open_files(csv_name, bin_name);
    struct _finfo linfo = {.amnt_const = 2, 
                           .amnt_reg = 0,
                           .amnt_rmv = 0,
                           .next_reg_offset = L_HEADER_SIZE,
                           .const_size = L_CONST_REG_SIZE, 
                           .header_funct = l_header_init, 
                           .header_size = L_HEADER_SIZE, 
                           .insert_funct = l_insert_datareg};

    
    _header_init(files, &linfo);
    _insert_dataregs_from_csv(files, &linfo);

    fclose(files->bin); //TODO: Make a function for this
    fclose(files->csv);
    free(files);  
}

void vehicle_insert_into(string bin_name, int amnt_regs) {
    FILE *bin = open_file(bin_name, "r+b");
    struct _finfo vinfo = {.amnt_const = 4, 
                           .amnt_reg = 0,
                           .amnt_rmv = 0,
                           .next_reg_offset = 0,
                           .const_size = V_CONST_REG_SIZE, 
                           .header_funct = v_header_init, 
                           .header_size = V_HEADER_SIZE, 
                           .insert_funct = v_insert_datareg};
    
    g_read_header(bin, &vinfo);
    _insert_dataregs_from_terminal(bin, &vinfo, amnt_regs, v_read_tokens_from_terminal);
    
    fclose(bin); //TODO: Make a function for this
}

void line_insert_into(string bin_name, int amnt_regs) {
    FILE *bin = open_file(bin_name, "r+b");

    struct _finfo linfo = {.amnt_const = 2, 
                           .amnt_reg = 0,
                           .amnt_rmv = 0,
                           .next_reg_offset = L_HEADER_SIZE,
                           .const_size = L_CONST_REG_SIZE, 
                           .header_funct = l_header_init, 
                           .header_size = L_HEADER_SIZE, 
                           .insert_funct = l_insert_datareg};
    
    g_read_header(bin, &linfo);
    _insert_dataregs_from_terminal(bin, &linfo, amnt_regs, l_read_tokens_from_terminal);
    
    fclose(bin); //TODO: Make a function for this

}

void func_select(string bin_name, int select) {
    FILE *fp = open_file(bin_name, "rb");

    // getting the last file's byte
    fseek(fp, 0, SEEK_END);
    int last_byte = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (select == VEHICLE_FILE){
        fseek(fp, V_HEADER_SIZE, SEEK_SET);
        v_select(fp, last_byte);
    }
    else if (select == LINE_FILE) {
        fseek(fp, L_HEADER_SIZE, SEEK_SET);
        line_select(fp, last_byte);
    }

    fclose(fp);
}

void vehicle_select_where(string bin_name, string field, string value) {
    FILE *bin = open_file(bin_name, "rb");

    _select_where(bin, VEHICLE_FILE, field, value);
    fclose(bin);
}

void line_select_where(string bin_name, string field, string value) {
    FILE *bin = open_file(bin_name, "rb");

    _select_where(bin, LINE_FILE, field, value);
    fclose(bin);
}