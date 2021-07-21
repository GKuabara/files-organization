/*
** B-Tree module

**  Gabriel Alves Kuabara - nUSP 11275043 - email: gabrielalveskuabara@usp.br
**  Milena Correa da Silva - nUSP 11795401 - email: milenacorreasilva@usp.br 
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btree.h"

static long _bt_search_key_recusive(FILE *bin, int start_rrn, int c_tar);
static bt_node_t *_bt_node_init(int rrn_tar, char leaf, int n_keys);
static void _bt_node_store(FILE *bin, bt_node_t *node);
static bt_node_t *_bt_node_load(FILE *bin, int rrn_tar);
static void _bt_node_free(bt_node_t *node);
static int _bt_node_insert_key(bt_node_t *node, bt_key_t *new_key);
static bt_node_t *_bt_split_child(bt_header_t *header, bt_node_t *parent, bt_node_t *child);
static bt_node_t *_bt_insert_key_recursive(FILE *bin, bt_header_t *header, int start_rrn, bt_key_t *new_key);

/* 
    Recursive call of `bt_search_key()`. 
*/
static long _bt_search_key_recusive(FILE *bin, int start_rrn, int c_tar) {
    if (start_rrn == -1) return -1; // If the key doesn't exists in the tree
    
    bt_node_t *cur_node = _bt_node_load(bin, start_rrn);
    
    int i;
    for (i = 0; i < cur_node->amnt_keys; ++i) {
        if (c_tar == cur_node->keys[i]->c) {
            long p_r = cur_node->keys[i]->p_r;
            _bt_node_free(cur_node);

            return p_r;
        }

        if (c_tar < cur_node->keys[i]->c) break;
    }

    int next = cur_node->p[i];
    _bt_node_free(cur_node);

    return _bt_search_key_recusive(bin, next, c_tar);
}

/* 
    Initializes a btree node. 
*/
static bt_node_t *_bt_node_init(int rrn_tar, char leaf, int n_keys) {
    bt_node_t *new_node = malloc(sizeof(*new_node));
    assert(new_node);

    new_node->is_leaf = leaf;
    new_node->amnt_keys = n_keys;
    new_node->rrn = rrn_tar;
    for (int i = 0; i < BT_DEGREE; ++i) new_node->keys[i] = NULL;
    for (int i = 0; i < BT_DEGREE + 1; ++i) new_node->p[i] = -1;

    return new_node;
}

/* 
    Stores/Writes a btree node from memory to disk. 
*/
static void _bt_node_store(FILE *bin, bt_node_t *node) {
    fseek(bin, (node->rrn + 1) * BT_NODE_SIZE, SEEK_SET);
    
    /* Updates the header of each node */
    file_write(&node->is_leaf, sizeof(char), 1, bin);
    file_write(&node->amnt_keys, sizeof(int), 1, bin);
    file_write(&node->rrn, sizeof(int), 1, bin);
    
    /* Updates the bt_key_t keys of the node */ 
    file_write(&node->p[0], sizeof(int), 1, bin);
    for (int i = 0; i < BT_MAX_KEYS; ++i) {    
        int c = node->keys[i] ? node->keys[i]->c : -1;
        long p_r = node ->keys[i] ? node->keys[i]->p_r : -1; 
        
        file_write(&c, sizeof(int), 1, bin);
        file_write(&p_r, sizeof(long), 1, bin);
        file_write(&node->p[i + 1], sizeof(int), 1, bin);
    }    
}

/* 
    Loads a btree node from disk to memory. 
    Callers are responsible for `free()`ing it. 
*/
static bt_node_t *_bt_node_load(FILE *bin, int rrn_tar) {
    fseek(bin, (rrn_tar + 1) * BT_NODE_SIZE, SEEK_SET);

    bt_node_t *node = _bt_node_init(rrn_tar, IS_LEAF, 0);

    /* Reads node's header */
    file_read(&node->is_leaf, sizeof(char), 1, bin);
    file_read(&node->amnt_keys, sizeof(int), 1, bin);
    file_read(&node->rrn, sizeof(int), 1, bin);
    /* Reads all the node's bt_key_t keys */
    file_read(&node->p[0], sizeof(int), 1, bin);
    for (int i = 0; i < node->amnt_keys; i++) {
        node->keys[i] = bt_node_key_init(-1, -1);

        file_read(&node->keys[i]->c, sizeof(int), 1, bin);
        file_read(&node->keys[i]->p_r, sizeof(long), 1, bin);
        file_read(&node->p[i + 1], sizeof(int), 1, bin);
    }

    return node;
}

/* 
    Frees a btree node. 
*/
static void _bt_node_free(bt_node_t *node) {
    for (int i = 0; i < BT_MAX_KEYS + 1; ++i) 
        free(node->keys[i]);
    
    free(node);
}

/* 
    Inserts a key in a btree node 
    with an extra overflow space. 
*/
static int _bt_node_insert_key(bt_node_t *node, bt_key_t *new_key) {
    if (node->amnt_keys > BT_MAX_KEYS) return -1; 
    
    // Finds the new position of the key to be inserted
    int new_key_pos;
    for (new_key_pos = 0; new_key_pos < node->amnt_keys; new_key_pos++) {
        if (new_key->c < node->keys[new_key_pos]->c) break;
    }

    // Moves everything  bigger oe index to the right in order to 
    // free space for the new key.
    for (int j = node->amnt_keys, k = node->amnt_keys + 1; j > new_key_pos; --j, --k)  {
        node->keys[j] = node->keys[j - 1];
        node->p[k] = node->p[k - 1];
    }

    // Adds the new key
    node->keys[new_key_pos] = new_key;
    node->p[new_key_pos + 1] = -1;
    node->amnt_keys++;

    return new_key_pos;
}

/* 
    Splits an already overflowed node (node->amnt_keys == BT_MAX_KEYS + 1). 
*/
static bt_node_t *_bt_split_child(bt_header_t *header, bt_node_t *parent, bt_node_t *child) { 
    child->amnt_keys = BT_SPLITED_KEYS;
    
    bt_node_t *splited = _bt_node_init(header->next_rrn, child->is_leaf, BT_SPLITED_KEYS);
    header->next_rrn++;

    // Splits the keys and the p's 
    for (int i = 0; i < BT_SPLITED_KEYS; ++i) { 
        splited->keys[i] = child->keys[i + BT_SPLITED_KEYS + 1];
        child->keys[i + BT_SPLITED_KEYS + 1] = NULL; 
    }

    for (int i = 0; i < BT_SPLITED_KEYS + 1; ++i) { 
        splited->p[i] = child->p[i + BT_SPLITED_KEYS + 1]; 
        child->p[i + BT_SPLITED_KEYS + 1] = -1;
    } 

    // Promotes the middle bt_key_t 
    int promotion_pos = _bt_node_insert_key(parent, child->keys[BT_SPLITED_KEYS]);
    child->keys[BT_SPLITED_KEYS] = NULL; 
    parent->p[promotion_pos] = child->rrn;
    parent->p[promotion_pos + 1] = splited->rrn;

    return splited;
}   

/*
    Recursive call to insert bt_key_t at the b-tree. Can return `NULL` if the insertion has ended before reaching the root of the tree, or the root node itself.
*/
static bt_node_t *_bt_insert_key_recursive(FILE *bin, bt_header_t *header, int start_rrn, bt_key_t *new_key) {
    bt_node_t *cur_node = _bt_node_load(bin, start_rrn); 

    // Always insert at a leaf node
    if (cur_node->is_leaf == IS_LEAF)  { 
        _bt_node_insert_key(cur_node, new_key);
        return cur_node;
    }

    int child_pos;
    for (child_pos = 0; child_pos < cur_node->amnt_keys; ++child_pos) {
        if (new_key->c < cur_node->keys[child_pos]->c) break;
    }

    // Starts recursive ascent 
    start_rrn = cur_node->p[child_pos];
    bt_node_t *child = _bt_insert_key_recursive(bin, header, start_rrn, new_key);

    // In case the insertion has been completed
    if (child == NULL) {
        _bt_node_free(cur_node);
        return NULL;
    }

    // In case the child node has at max 4 keys, the insertion is completed and
    // the overflow has been sucefully propagated 
    if (child->amnt_keys <= BT_MAX_KEYS) {  
        _bt_node_store(bin, child);
        
        _bt_node_free(cur_node);
        _bt_node_free(child);
        
        return NULL;
    }

    // In case there is still overflow to be treated and propagated
    bt_node_t *splited = _bt_split_child(header, cur_node, child);
    _bt_node_store(bin, splited); 
    _bt_node_store(bin, child);

    _bt_node_free(child);
    _bt_node_free(splited);

    return cur_node;
}

/* 
    Initizalizes a btree file header both in memory
    and in disk. Callers are responsible for `free()`ing 
    it.
*/
bt_header_t *bt_header_init(FILE *bin) {
    fseek(bin, 0, SEEK_SET);

    bt_header_t *header = malloc(sizeof(*header));
    assert(header);

    header->status = INC_STAT;
    header->root_rrn = -1;
    header->next_rrn = 0;
    char trash = '@';

    file_write(&header->status, sizeof(char), 1, bin);
    file_write(&header->root_rrn, sizeof(int), 1, bin);
    file_write(&header->next_rrn, sizeof(int), 1, bin);

    for (int i = 0; i < 68; ++i) file_write(&trash, sizeof(char), 1, bin);

    return header;
}

/*
    Stores/Writes a btree header from disk to memory.
*/
void bt_header_store(FILE *bin, bt_header_t *header) {
    fseek(bin, 0, SEEK_SET);

   file_write(&header->status, sizeof(char), 1, bin);
    if (header->status == INC_STAT) return; 

   file_write(&header->root_rrn, sizeof(int), 1, bin);
   file_write(&header->next_rrn, sizeof(int), 1, bin);
}

/* 
    Loads a btree file header from disk to memory. Callers 
    are responsible for `free()`ing it.
*/
bt_header_t *bt_header_load(FILE *bin) {
    fseek(bin, 0, SEEK_SET);

    bt_header_t *header = malloc(sizeof(*header));
    assert(header);

    file_read(&header->status, sizeof(char), 1, bin);
    file_read(&header->root_rrn, sizeof(int), 1, bin);
    file_read(&header->next_rrn, sizeof(int), 1, bin);

    return header;
}

/* 
    Initializes a new key instance given the code (c) and the 
    offset (p_r). Callers are responsible for `free()`ing it.
*/
bt_key_t *bt_node_key_init(int c, long p_r) {
    bt_key_t *new_key = malloc(sizeof(*new_key));
    assert(new_key);

    new_key->c = c;
    new_key->p_r = p_r;

    return new_key;
}

/*
    Searchs and returns a bt_key_t pair
*/
long bt_search_key(FILE *bin, bt_header_t *header, int c_tar) {
    if (!bin || !header || c_tar == -1) return -1;
    
    return _bt_search_key_recusive(bin, header->root_rrn, c_tar);
}

/*
    Inserts a new key in a btree.
*/
void bt_insert_key(FILE *bin, bt_header_t *header, bt_key_t *new_key) {
    // In case the tree has no root
    if (header->root_rrn  == -1) {
        bt_node_t *node = _bt_node_init(0, IS_LEAF, 0);
        _bt_node_insert_key(node, new_key);

        _bt_node_store(bin, node);
        _bt_node_free(node);

        header->root_rrn = header->next_rrn++;

        return;
    }

    bt_node_t *node = _bt_insert_key_recursive(bin, header, header->root_rrn, new_key);

    // If the root is return there are 2 insertion cases: regular insertion at the root or overflow at the root.
    if (node != NULL) {
        // If there is a overflow in the root
        if (node->amnt_keys > BT_MAX_KEYS) { 
            bt_node_t *new_root = _bt_node_init(header->next_rrn + 1, NOT_LEAF, 0);
            bt_node_t *splited = _bt_split_child(header, new_root, node);

            header->root_rrn = header->next_rrn++;

            _bt_node_store(bin, new_root);
            _bt_node_store(bin, splited);
        
            _bt_node_free(new_root);
            _bt_node_free(splited);
        }
        
        _bt_node_store(bin, node);
        _bt_node_free(node);
    }

}