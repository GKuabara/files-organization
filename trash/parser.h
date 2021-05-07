#ifndef _PARSER_H
#define _PARSER_H

#include "utils.h"
#include <regex.h>


#define CSV_PATTERN "[,]"

typedef struct reader *Reader;
struct reader {    
    regex_t pattern;
    string *values;
    int amnt_values;
};

typedef struct header *Header;
struct header {
    int amnt_fields;
    string *fields;
};

/*
    Initializes a `Reader` instance given a parsing pattern.
    Callers are responsible for `free()`ing it using `parser_reader_delete()`.
*/
Reader parser_reader_init(string pattern);

/*
    Cleans the `values` in a `Reader`.
*/
void parser_clean_reader(Reader reader);

/* 
    Parses a `line` given a `reader` instance. 
*/
void parser_read(Reader reader, string line);

/*
    Deletes a `Reader` instance.
*/
void parser_reader_delete(Reader *reader);

/*
    Reads the header of a `stream`  given its parsing `pattern`.
    Callers are responsible for `free()`ing it using `parser_header_delete()`. 
*/
Header parser_header_init(FILE *stream, string pattern);

/*
    Deletes an instance of a track list.
*/
void parser_header_delete(Header *header);

#endif