#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"
#include "utils.h"

#define len(string) (string == NULL ? 0 : strlen(string))

static int _is_delim(char *stream, const struct _delim d) {
    for (int i = 0; i < d.amnt_delimitators; ++i) {
        int delim_len = strlen(d.delimitators[i]);
        if (strncmp(stream, d.delimitators[i], delim_len) == 0) return delim_len; 
    }

    return 0;
}

void *alloc_newpage(void *__ptr, size_t __size, int amnt_pages) {
    return realloc(__ptr, __size * amnt_pages * BUFFER_SIZE);
}

char *_build_token(char **t, int t_size, char c) {
    *t = realloc(*t, sizeof(char) * (t_size + 1));
    (*t)[t_size] = c;

    return *t;
}

char **linear_stream_get_tokens(char *stream, const struct _delim d) {
    char **tokens = calloc(BUFFER_SIZE, sizeof(char *));
    char **t = tokens;

    int token_size = 0;
    for (; *stream != '\0';) {        
        int off;
        if ((off = _is_delim(stream, d))) {
            *t = _build_token(&(*t), token_size, '\0');
            t++;
            token_size = 0;
        } else {
            off = 1;
            *t = _build_token(&(*t), token_size++, *stream);
        }

        stream += off;
    }
    
    *t =  _build_token(&(*t), token_size, '\0');
    return tokens;
}
