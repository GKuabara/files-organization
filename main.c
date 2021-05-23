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

    if(tokens[0][0] == '1') {
        vehicle_create_table(tokens[1], tokens[2]);
        binarioNaTela(tokens[2]);
        return;
    }
    else if(tokens[0][0] == '2') {
        line_create_table(tokens[1], tokens[2]);
        binarioNaTela(tokens[2]);
        return;
    }
    else if(tokens[0][0] == '3') func_select(tokens[1], 1);
    else if(tokens[0][0] == '4') func_select(tokens[1], 2);
    else if(tokens[0][0] == '5') vehicle_select_where(tokens[1], tokens[2], tokens[3]);
    else if(tokens[0][0] == '6') line_select_where(tokens[1], tokens[2], tokens[3]);
    else if(tokens[0][0] == '7') {
        vehicle_insert_into(tokens[1], atoi(tokens[2]));
        binarioNaTela(tokens[1]);
        
        return;
    }
    else if(tokens[0][0] == '8') {
        vehicle_insert_into(tokens[1], atoi(tokens[2]));
        binarioNaTela(tokens[1]);

        return;
    }
    else {
        printf("Please select a valid funcionality\n");
        return;
    }

}

int main() {    
    string s = readline(stdin);
    string *tokens = str_get_tokens(s,  
        (struct _delim_t) {.amnt_delim=1, .delim=(char *[]){" "}});
        
    select_funcionality(tokens);

    for (string *aux = tokens; *aux; aux++) free(*aux);
    free(tokens);
    free(s);
    return 0;
}