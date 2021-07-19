/*
**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream.h"
#include "operations.h"
#include "funcao_fornecida.h"

/*
    Function to select what SQL implementation we want to reproduce
*/
void select_funcionality(string *tokens) {
    boolean is_correct;
    int num = atoi(tokens[0]);

    if(num == 1) {
        is_correct = vehicle_create_table(tokens[1], tokens[2]);
        if(is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(num == 2) {
        is_correct = line_create_table(tokens[1], tokens[2]);
        if(is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(num == 3) {
        vehicle_select(tokens[1]);
    }
    else if(num == 4) {
        line_select(tokens[1]);
    }    
    else if(num == 5) {
        vehicle_select_where(tokens[1], NULL, tokens[2], tokens[3]);
    }
    else if(num == 6) {
        line_select_where(tokens[1], NULL, tokens[2], tokens[3]);
    }
    else if(num == 7) {
        is_correct = vehicle_insert_into(tokens[1], NULL, atoi(tokens[2]));
        if (is_correct == True) binarioNaTela(tokens[1]);
    }
    else if(num == 8) {
        is_correct = line_insert_into(tokens[1], NULL, atoi(tokens[2]));
        if (is_correct == True) binarioNaTela(tokens[1]);
    }
    else if(num == 9) {
        is_correct = vehicle_create_index(tokens[1], tokens[2]);
        if (is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(num == 10) {
        is_correct = line_create_index(tokens[1], tokens[2]);
        if (is_correct == True) binarioNaTela(tokens[2]);
    } 
    else if(num == 11) {
        is_correct = vehicle_select_where(tokens[1], tokens[2], tokens[3], tokens[4]);
    }
    else if(num == 12) {
        is_correct = line_select_where(tokens[1], tokens[2], tokens[3], tokens[4]);
    }
    else if(num == 13) {
        is_correct = vehicle_insert_into(tokens[1], tokens[2], atoi(tokens[3]));
        if (is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(num == 14) {
        is_correct = line_insert_into(tokens[1], tokens[2], atoi(tokens[3]));
        if (is_correct == True) binarioNaTela(tokens[2]);
    }
<<<<<<< HEAD
    else if(num == 17) {
        is_correct = vehicle_create_sorted_file(tokens[1], tokens[2], tokens[3]);
        if (is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(num == 18) {
        is_correct = line_create_sorted_file(tokens[1], tokens[2], tokens[3]);
        if (is_correct == True) binarioNaTela(tokens[2]);
    }
    else if(num == 19) {
        merge_files_by_field(tokens[1], tokens[2], tokens[3], tokens[4]);
    }
=======
    else if(num == 15) {
        select_from(tokens[1], tokens[2], tokens[3], tokens[4], NULL);
    }
    else if(num == 16) {
        select_from(tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
    }

>>>>>>> ku
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