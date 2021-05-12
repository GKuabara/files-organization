#include "vehicle.h"
#include "utils.h"


#include "parser.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct _vehicle_files {
    FILE *bin;
    FILE *csv;
} Vehicle_Files;

static void _vehicle_write_bin_reg(Vehicle_Files *files, string *values);
static bool _vehicle_write_binary();

static Vehicle_Files *_vehicle_get_files(const string csv_name, const string bin_name) {
    Vehicle_Files *files = malloc(sizeof(*files));
    assert(files && "error alocating struct");

    files->csv = open_file(csv_name, 'r');
    files->bin = open_file(bin_name, 'w+b');
    
    return files;
}

static void _vehicle_write_bin_reg(Vehicle_Files *files, string *values) {
       
}


    // Reader r = parser_reader_init(CSV_PATTERN);

    // string line = readline(files->csv);
    // parser_read(r, line);
    // r->values[0];
    // r->amnt_values;

    // free(line);
    // parser_clean_reader(r);


    // parser_reader_delete(r);

static void _vehicle_write_bin_header(Vehicle_Files *files) {

    Reader header = parser_reader_init(CSV_PATTERN);
    string line = readline(files->csv);
    parser_read(header, line);

    fwrite(header->values[i])

}

void vehicle_create_table(string csv_name, string bin_name) {

    Vehicle_Files *files = _vehicle_open_files(csv_name, bin_name);

    _vehicle_write_bin_header(files);
    
    while() {
        
        _vehicle_write_binary();
    }
    
}