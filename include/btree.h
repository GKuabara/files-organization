#ifndef _BTREE_H
#define _BTREE_H

#include "stream.h"
#include "global.h"
#include "vehicle.h"
#include "line.h"

#define IS_LEAF '1'
#define NOT_LEAF '0'
#define NODE_SIZE 77
#define BT_DEGREE 5

#define AMNT_KEYS (BT_DEGREE - 1)

typedef struct _key_pair{
    int c;
    long p_r;
} key_pair;

typedef struct _bt_node{
    char is_leaf;
    int amnt_keys;
    int rrn;
    int p[BT_DEGREE + 1];
    key_pair *pairs[AMNT_KEYS + 1];
} bt_node;

typedef struct _bt_header_t {
    char status;
    int root_rrn;
    int next_rrn;
} bt_header_t;

void bt_header_init(FILE *bin);
void bt_header_update(FILE *bin, char stats, int root_rrn, int next_node_rnn);
void bt_insert_key(FILE *bin, int *root_rrn, int *next_rrn, key_pair *new_pair);
key_pair *bt_search_key(FILE *bin, int c_tar);
bt_header_t *bt_header_load(FILE *bin);

#endif