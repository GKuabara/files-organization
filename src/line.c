#include "line.h"
#include "stream.h"

#include <stdio.h>
#include <stdlib.h>

static line *_l_read_reg_data(FILE *fp) {

    line *data = malloc(sizeof(*data));

    fread(&data->code, sizeof(int), 1, fp);
    fread(&data->card, sizeof(char), 1, fp);
    
    fread(&data->name_size, sizeof(int), 1, fp);
    data->line_name = g_read_var_field(fp, data->name_size);

    fread(&data->color_size, sizeof(int), 1, fp);
    data->color = g_read_var_field(fp, data->color_size);

    return data;
}

static void print_card(char card) {
    if (card == S) printf("Aceita cartao: PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR\n");
    else if (card == N) printf("Aceita cartao: PAGAMENTO EM CARTAO E DINHEIRO\n");
    else if (card == F) printf("Aceita cartao: PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n");
}

static void l_print_reg_data(line *data) {
    printf("Codigo da linha: %d\n", data->code);
    printf("Nome da linha: %s\n", data->line_name);
    printf("Cor que descreve a linha: %s\n", data->color);
    print_card(data->card);
    printf("\n");
}

void line_select(FILE *fp, int last_byte) {
    while(ftell(fp) < last_byte) {

        data_header *header = _g_read_reg_header(fp);
        //printf("removed: %c regsize: %d\n", header->removed, header->reg_size);

        if(header->removed == RMV) {
            int next_reg = ftell(fp) + header->reg_size;
            fseek(fp, next_reg, SEEK_SET);
            free(header);
            continue;
        }

        line *data = _l_read_reg_data(fp);

        l_print_reg_data(data);

        free(header);
        free(data->line_name);
        free(data->color);
        free(data);
    }
}