/*
**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream.h"
#include "operations.h"
#include "funcao-fornecida.h"

/*
    Function to select what SQL implementation we want to reproduce
*/
void select_funcionality(string *tokens) {
    boolean is_correct;

    if(atoi(tokens[0]) == 1) {
        is_correct = vehicle_create_table(tokens[1], tokens[2]);
        if(is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(atoi(tokens[0]) == 2) {
        is_correct = line_create_table(tokens[1], tokens[2]);
        if(is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(atoi(tokens[0]) == 3) {
        vehicle_select(tokens[1]);
    }
    else if(atoi(tokens[0]) == 4) {
        line_select(tokens[1]);
    }    
    else if(atoi(tokens[0]) == 5) {
        vehicle_select_where(tokens[1], NULL, tokens[2], tokens[3]);
    }
    else if(atoi(tokens[0]) == 6) {
        line_select_where(tokens[1], NULL, tokens[2], tokens[3]);
    }
    else if(atoi(tokens[0]) == 7) {
        is_correct = vehicle_insert_into(tokens[1], NULL, atoi(tokens[2]));
        if (is_correct == True) binarioNaTela(tokens[1]);
    }
    else if(atoi(tokens[0]) == 8) {
        is_correct = line_insert_into(tokens[1], NULL, atoi(tokens[2]));
        if (is_correct == True) binarioNaTela(tokens[1]);
    }
    else if(atoi(tokens[0]) == 9) {
        is_correct = vehicle_create_index(tokens[1], tokens[2]);
        if (is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(atoi(tokens[0]) == 10) {
        is_correct = line_create_index(tokens[1], tokens[2]);
        if (is_correct == True) binarioNaTela(tokens[2]);
    } 
    else if(atoi(tokens[0]) == 11) {
        is_correct = vehicle_select_where(tokens[1], tokens[2], tokens[3], tokens[4]);
    }
    else if(atoi(tokens[0]) == 12) {
        is_correct = line_select_where(tokens[1], tokens[2], tokens[3], tokens[4]);
    }
    else if(atoi(tokens[0]) == 13) {
        is_correct = vehicle_insert_into(tokens[1], tokens[2], atoi(tokens[3]));
        if (is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(atoi(tokens[0]) == 14) {
        is_correct = line_insert_into(tokens[1], tokens[2], atoi(tokens[3]));
        if (is_correct == True) binarioNaTela(tokens[2]);
    }

    else {
        printf("Please select a valid funcionality\n");
        return;
    }



}

int main() {    
    string s = readline(stdin);
    string *tokens = str_get_tokens(s, .amnt_delim=1, .delim=(char *[]){" "});

    select_funcionality(tokens);
    str_free_tokens(tokens);

    free(s);
    return 0;
}