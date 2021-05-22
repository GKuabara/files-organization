// Participantes:
// Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
// Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream.h"
#include "operations.h"
#include "funcao-fornecida.h"

void select_funcionality(string *tokens) {

    if(tokens[0] == 1) vehicle_create_table(tokens[1], tokens[2]);
    else if(tokens[0] == 2) line_create_table(tokens[1], tokens[2]);
    else if(tokens[0] == 3) func_select(tokens[1], 1);
    else if(tokens[0] == 4) func_select(tokens[1], 2);
    else if(tokens[0] == 5) select_where();
    else if(tokens[0] == 6) select_where();
    else if(tokens[0] == 7) vehicle_insert();
    else if(tokens[0] == 8) line_insert();
    else {
        printf("Please select a valid funcionality\n");
    }
}

int main() {    
    string s = readline(stdin);
    string *tokens = str_get_tokens(s,  
        (struct _delim_t) {.amnt_delim=1, .delim=(char *[]){" "}});

    for(string *t = tokens; *t; ++t) {
        printf("%s\n", *t);
    }

    select_funcionality(tokens);

    for (string *aux = tokens; *aux; aux++) free(*aux);
    free(tokens);
    free(s);
    return 0;
}