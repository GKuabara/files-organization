#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stream.h"

static int _input_stream_get_char(FILE *stream, const struct _term_t t);
static void *_alloc_new_page(void *__ptr, size_t __size, int amnt_pages);
static int _delim(char *stream, const struct _delim_t d);
static string _build_token(char **t, int t_size, char c);

static int _delim(string str, const struct _delim_t d) {
    for (int i = 0; i < d.amnt_delim; ++i) {
        int delim_len = strlen(d.delim[i]);
        if (strncmp(str, d.delim[i], delim_len) == 0) return delim_len;
    }

    return 0;
}

static string _build_token(string *t, int t_size, char c) {
    *t = realloc(*t, sizeof(char) * (t_size + 1));
    (*t)[t_size] = c;

    return *t;
}

static void *_alloc_new_page(void *__ptr, size_t __size, int buffer_size) {
    int total_bytes = (buffer_size / PAGE_SIZE + 1) * PAGE_SIZE;
    return realloc(__ptr, total_bytes * __size);
}

static int _input_stream_get_char(FILE *stream, const struct _term_t t) {
    int c = fgetc(stream);
    
    if (c == '\r') c = fgetc(stream);

    for (int i = 0; i < t.amnt_term; ++i) {
        if (c == t.term[i]) return 0; 
    }
  
    return c;
}

string input_stream_read(FILE *stream, const struct _term_t t) {
    if (feof(stream)) return NULL;
    if (t.amnt_term == 0) return NULL; // Todo: printf("Erro ao ler string: nenhum caracter de terminação\n");
   
    char *buffer = NULL;
    int b_size = 0;
    int c;

    do {
        if (b_size % PAGE_SIZE == 0) buffer = _alloc_new_page(buffer, sizeof(*buffer), b_size);
 
        c = _input_stream_get_char(stream, t);
        buffer[b_size] = c;
        b_size++;
    } while (c);

    if (b_size < 2) {
        free(buffer);
        return NULL; 
    }

    if (buffer[b_size - 2] ==  ' ') buffer[--b_size - 1] = '\0';

    return realloc(buffer, sizeof(*buffer) * b_size);
}

string *str_get_tokens(string str, const struct _delim_t d) {
    if (str == NULL) return NULL;
    // if (!d.amnt_delim)

    string *tokens = calloc(PAGE_SIZE, sizeof(*tokens));
    int i = 0;
    int token_size = 0;
    while (*str != '\0') {        
        int off;
        if ((off = _delim(str, d))) {
            tokens[i] = _build_token(&tokens[i], token_size, '\0');
            token_size = 0;
            i++;
        } else {
            off = 1;
            tokens[i] = _build_token(&tokens[i], token_size++, *str);
        }

        str += off;
    }

    tokens[i] = _build_token(&tokens[i], token_size, '\0');
    return tokens;
}

int readnum_(FILE *stream, const struct _term_t t) {
    string tmp_num = input_stream_read(stream, t);
    int num = atoi(tmp_num);

    free(tmp_num);
    return num;
}

string readline_(FILE *stream, const struct _term_t t) {
    return input_stream_read(stream, t);
}

FILE* open_file(const char *file_path, const char *flag) {
    FILE *fp = fopen(file_path, flag);

    if (fp == NULL) { 
        perror("Error");
        exit(EXIT_FAILURE);
    }

    return fp;
}   

string str_add_terminator(string str, int len) {

    string new = malloc(sizeof(char) * (len + 1));
    new[len] = '\0';
    return new;
}