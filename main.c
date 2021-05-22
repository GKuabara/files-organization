// Participantes:
// Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
// Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream.h"
#include "operations.h"
#include "funcao-fornecida.h"

int main() {    
    string s = readline(stdin);
    string *tokens = str_get_tokens(s,  
        (struct _delim_t) {.amnt_delim=1, .delim=(char *[]){" "}});

    for(string *t = tokens; *t; ++t) {
        printf("%s\n", *t);
    }

    // vehicle_create_table(tokens[1], tokens[2]);
    
    // binarioNaTela(tokens[2]);

    func_select(tokens[0], 2);

    for (string *aux = tokens; *aux; aux++) free(*aux);
    free(tokens);
    free(s);
    return 0;
}