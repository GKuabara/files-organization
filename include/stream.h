#ifndef _STREAM_H
#define _STREAM_H

#include <stdio.h>

#define string char *
#define PAGE_SIZE 4096

struct _term_t {
    int amnt_term;
    int *term;
};

struct _delim_t {
    int amnt_delim;
    string *delim;
};

#define default_term \
    (int[]){'\n', EOF}

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

string *str_get_tokens(string str, const struct _delim_t d);
string input_stream_read(FILE *stream, const struct _term_t t);

FILE* open_file(const char *file_path, const char *flag);

#define csv_delim \
    (char *[]) {", ", ","}


#endif