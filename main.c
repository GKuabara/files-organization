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

    if(tokens[0][0] == '1') {
        is_correct = vehicle_create_table(tokens[1], tokens[2]);
        if(is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(tokens[0][0] == '2') {
        is_correct = line_create_table(tokens[1], tokens[2]);
        if(is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(tokens[0][0] == '3') {
        vehicle_select(tokens[1]);
    }
    else if(tokens[0][0] == '4') {
        line_select(tokens[1]);
    }    
    else if(tokens[0][0] == '5') {
        vehicle_select_where(tokens[1], tokens[2], tokens[3]);
    }
    else if(tokens[0][0] == '6') {
        line_select_where(tokens[1], tokens[2], tokens[3]);
    }
    else if(tokens[0][0] == '7') {
        is_correct = vehicle_insert_into(tokens[1], atoi(tokens[2]));
        if (is_correct == True) binarioNaTela(tokens[1]);
    }
    else if(tokens[0][0] == '8') {
        is_correct = line_insert_into(tokens[1], atoi(tokens[2]));
        if (is_correct == True) binarioNaTela(tokens[1]);
    }
    else {
        printf("Please select a valid funcionality\n");
        return;
    }

}

int main() {    
    string s = readline(stdin);
    string *tokens = str_get_tokens(s, .amnt_delim=1, .delim=(char *[]){" "});

    printf("filename: %s\t %s\n", tokens[1], tokens[2]);

    vehicle_create_index(tokens[1], tokens[2]);

    // select_funcionality(tokens);
    str_free_tokens(tokens);
    free(s);
    return 0;
}