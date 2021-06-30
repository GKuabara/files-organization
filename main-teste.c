#include <stdio.h>
#include <stdlib.h>

#include "btree.h"

int main() {
    FILE *index = fopen("index", "w+b");
    int root_rrn = -1;
    int next_reg = 0;


    key_pair *new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1000;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 2;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 5;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 7;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1111;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 12;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 23;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 189;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1823;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 21;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 89;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 67;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 16;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 100;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 2111;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);
   
    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1100;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1102;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1103;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1104;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 1105;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 190;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 22;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 24;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 25;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    new_pair = malloc(sizeof(*new_pair));
    new_pair->c = 26;
    bt_insert_key(index, &root_rrn, &next_reg, new_pair);

    bt_header_update(index, CON_STAT, root_rrn, next_reg);
    fclose(index);
}