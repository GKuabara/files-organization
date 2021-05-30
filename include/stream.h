#ifndef _STREAM_H
#define _STREAM_H

#include <stdio.h>

#define string char *
#define PAGE_SIZE 4096

// Contains string terminators
struct _term_t {
    int amnt_term;
    int *term;
};

// Contains string delemiters to form substrings
struct _delim_t {
    int amnt_delim;
    string *delim;
};

#define default_term \
    (int[]){'\n', EOF}

/*
    Gets int from readline
*/
#define readnum(file, ...) \
    readnum_(file, (struct _term_t) { \
        .amnt_term=2, \
        .term=default_term,\
        __VA_ARGS__\
    })
int readnum_(FILE *, const struct _term_t);

#define readline(file, ...) \
    readline_(file, (struct _term_t) { \
        .amnt_term=2, \
        .term=default_term,\
        __VA_ARGS__\
    })    
string readline_(FILE *, const struct _term_t);

string input_stream_read(FILE *stream, const struct _term_t t);

/*
    Lexer funcion, from 'str' gets tokens divided by delimiters 'd'
*/

#define str_get_tokens(file, ...) \
    str_get_tokens_(file, (struct _delim_t) { \
        __VA_ARGS__\
    }) 
    
string *str_get_tokens_(string str, const struct _delim_t d);
void str_free_tokens(string *tokens);




/*
    Open file with 'flag' treating errors
*/
FILE* open_file(const char *file_path, const char *flag);

/*
    Adds '\0' in the end of string
*/
string str_add_terminator(string str, int len);

#define csv_delim \
    (char *[]) {", ", ","}


void file_error(string error_mmessage);

#endif