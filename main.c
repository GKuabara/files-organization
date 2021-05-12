// Participantes:
// Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
// Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream.h"

int main() {    
    string s = readline(stdin);
    string *tokens = str_get_tokens(s,  
        (struct _delim_t) {.amnt_delim=2, .delim=csv_delim});

    for(string *t = tokens; *t; ++t) {
        printf("%s\n", *t);
        free(*t);
    }  

    free(tokens);
    free(s);
    return 0;
}