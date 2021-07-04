#ifndef _BTREE_H
#define _BTREE_H

#include "stream.h"
#include "global.h"
#include "file_handler.h"


#define IS_LEAF '1'
#define NOT_LEAF '0'

#define BT_NODE_SIZE 77

#define BT_DEGREE 5
#define BT_MAX_KEYS (BT_DEGREE - 1)
#define BT_SPLITED_KEYS (BT_MAX_KEYS / 2) 

typedef struct _bt_key_t {
    int c;
    long p_r;
} bt_key_t;

typedef struct _bt_node_t {
    char is_leaf;
    int amnt_keys;
    int rrn;
    int p[BT_DEGREE + 1];
    bt_key_t *keys[BT_MAX_KEYS + 1];
} bt_node_t;

typedef struct _bt_header_t {
    char status;
    int root_rrn;
    int next_rrn;
} bt_header_t;


/* 
    Initizalizes a btree file header both in memory
    and in disk. Callers are responsible for `free()`ing 
    it.
*/
bt_header_t  *bt_header_init(FILE *bin);

/*
    Stores/Writes a btree header from disk to memory.
*/
void bt_header_store(FILE *bin, bt_header_t *header);

/* 
    Loads a btree file header from disk to memory. Callers 
    are responsible for `free()`ing it.
*/
bt_header_t *bt_header_load(FILE *bin);

/* 
    Initializes a new key instance given the code (c) and the 
    offset (p_r). Callers are responsible for `free()`ing it.
*/
bt_key_t *bt_node_key_init(int c, long p_r);

/*
    Searchs and returns a bt_key_t pair
*/
long bt_search_key(FILE *bin, bt_header_t *header, int c_tar);

/*
    Inserts a new key in a btree.
*/
void bt_insert_key(FILE *bin, bt_header_t *header, bt_key_t *new_key);

#endif