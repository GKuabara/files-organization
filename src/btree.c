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

static bt_node *_bt_node_init(int rrn_tar, char leaf, int n_keys);
static void _bt_node_store(FILE *bin, bt_node *node);
static bt_node *_bt_node_load(FILE *bin, int rrn_tar);
static int _bt_node_insert_key(bt_node *node, key_pair *new_pair);
static bt_node *_bt_split_child(bt_node *parent, bt_node *child, int *next_rrn) ;
static bt_node *_bt_insert_key_recursive(FILE *bin, int start, int *next_rrn, key_pair *new_pair);


/* 
    Initizalizes the btree file header
*/
void bt_header_init(FILE *bin) {
    int status = INC_STAT;
    int root_rrn = -1;
    int next_node_rrn = 0;

    /* fwrite & Error handling */
    if (fwrite(&status, sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&root_rrn, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&next_node_rrn, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");

    for (int i = 0; i < 68; ++i) {
        char trash = '@';
        if (fwrite(&trash, sizeof(char), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
    }
}

/*
    Updates de btree header 
*/
void bt_header_update(FILE *bin, char stats, int root_rrn, int next_node_rnn) {
    fseek(bin, 0, SEEK_SET);

    /* fwrite & Error handling */
    if (fwrite(&stats, sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (stats == INC_STAT) return; 

    if (fwrite(&root_rrn, sizeof(int), 1, bin) != 1)    
        file_error("Falha no processamento do arquivo");

    if (fwrite(&next_node_rnn, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
}

/* 
    Searchs and returns a key pair
*/
key_pair *bt_search_key(FILE *bin, int root_rrn, int c_tar) {
    if (root_rrn == -1) return NULL;
    
    bt_node *cur_node = _bt_node_load(bin, root_rrn);
    
    int i = 0;
    for (int i = 0; i < AMNT_KEYS; ++i) {
        if (c_tar == cur_node->pairs[i]->c) return cur_node->pairs[i];

        if (c_tar < cur_node->pairs[i]->c) break;
    }

    return bt_search_key(bin, c_tar, cur_node->p[i]);
}

//////////// IN PROGRESS //////////

/* 
    Initializes a new btree node with an extra 
    overflow in the keys space. Callers are 
    responsible for `free()`ing it.
*/
// bt_node *_bt_node_init(int rrn_tar) {
//     bt_node *new_node = malloc(sizeof(*new_node));

//     new_node->is_leaf = IS_LEAF;
//     new_node->amnt_keys = 0;
//     new_node->rrn = rrn_tar;
//     memset(&new_node->p, -1, BT_DEGREE + 1);
    
//     for (int i = 0; i < AMNT_KEYS + 1; i++) {
//         new_node->pairs[i] = malloc(sizeof(*new_node->pairs[i]));
//         new_node->pairs[i]->c = new_node->pairs[i]->p_r = -1;
//     }

//     return new_node;
// }

static bt_node *_bt_node_init(int rrn_tar, char leaf, int n_keys) {
    bt_node *new_node = malloc(sizeof(bt_node));

    new_node->is_leaf = leaf;
    new_node->amnt_keys = n_keys;
    new_node->rrn = rrn_tar;
    memset(&new_node->p, -1, BT_DEGREE + 1);
    
    for (int i = 0; i < AMNT_KEYS + 1; i++) {
        new_node->pairs[i] = malloc(sizeof(*new_node->pairs[i]));
        new_node->pairs[i]->c = new_node->pairs[i]->p_r = -1;
    }

    return new_node;
}

/* 
    Stores a btree node from memory to disk 
*/
static void _bt_node_store(FILE *bin, bt_node *node) {
    fseek(bin, node->rrn, SEEK_SET);
    /* Updates the header of each node */
    if (fwrite(&(node->is_leaf), sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&(node->amnt_keys), sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&(node->rrn), sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    
    /* Updates the key pairs of the node */ 
    if (fwrite(&(node->p[0]), sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    for (int i = 0; i < AMNT_KEYS; ++i) {     
        if (fwrite(&(node->pairs[i]->c), sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fwrite(&(node->pairs[i]->p_r), sizeof(long), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fwrite(&(node->p[i + 1]), sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
    }    
}

/* 
    Loads a btree node from disk to memory. 
    Callers are responsible for `free()`ing it.
*/
static bt_node *_bt_node_load(FILE *bin, int rrn_tar) {
    printf("rrn_tar: %d\n", rrn_tar);
    fseek(bin, rrn_tar, SEEK_SET);

    bt_node *node = _bt_node_init(rrn_tar, IS_LEAF, 0);

    /* Reads node's header */
    if (fread(&node->is_leaf, sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fread(&node->amnt_keys, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fread(&node->rrn, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");

    /* Reads all the node's key pairs */
    for (int i = 0; i < AMNT_KEYS; i++) {
        if (fread(&node->p[i], sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fread(&node->pairs[i]->c, sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fread(&node->pairs[i]->p_r, sizeof(long), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
    }

    if (fread(&node->p[AMNT_KEYS], sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");

    return node;
}

/*
    Inserts a key in a btree node with an extra 
    overflow space
*/
static int _bt_node_insert_key(bt_node *node, key_pair *new_pair) {
    if (node->amnt_keys > AMNT_KEYS) return -1;
    
    int new_key_pos;
    for (new_key_pos = 0; new_key_pos < node->amnt_keys; new_key_pos++) {
        if (new_pair->c < node->pairs[new_key_pos]->c) break;
    }


    free(node->pairs[node->amnt_keys]); // Frees space to pull everything before
    for (int j = node->amnt_keys, k = node->amnt_keys + 1; j > new_key_pos; --j, --k)  {
        node->pairs[j] = node->pairs[j - 1];
        node->p[k] = node->p[k - 1];
    }

    node->pairs[new_key_pos] = new_pair;
    node->p[new_key_pos + 1] = -1;
    node->amnt_keys++;

    return new_key_pos;
}

static bt_node *_bt_split_child(bt_node *parent, bt_node *child, int *next_rrn) { 
    bt_node *splited = _bt_node_init(*next_rrn, child->is_leaf, 2);
    printf("splited: %d", splited->rrn);
    *next_rrn += NODE_SIZE;

    child->amnt_keys = 2;

    for (int i = 0; i < 2; ++i) { // Copies the second half of pairs (3 and 4) to splited 
        free(splited->pairs[i]);
        splited->pairs[i] = child->pairs[i + 3];
        
        child->pairs[i + 3] = malloc(sizeof(key_pair));
        child->pairs[i + 3]->c = child->pairs[i + 3]->p_r = -1; 
    }

    for (int i = 0; i < 3; ++i) { // Copies the second half of p (3, 4 and 5) to splited
        splited->p[i] = child->p[i + 3]; 
        child->p[i + 3] = -1;
    } 

    // Promotes the middle key (child->pairs[2]) to the parent
    int promotion_pos = _bt_node_insert_key(parent, child->pairs[2]);
    
    child->pairs[2] = malloc(sizeof(key_pair));
    child->pairs[2]->c = child->pairs[2]->c = -1; 

    parent->p[promotion_pos + 1] = splited->rrn;

    return splited;
}

/*
    Recursive call to insert key at the b-tree, inside the function
    there are 3 cases to insert when spliting:
    1 - b-tree has any node,
    2 - b-tree has a full root,
    3 - node of insertion is full
*/
static bt_node *_bt_insert_key_recursive(FILE *bin, int start, int *next_rrn, key_pair *new_pair) {
    bt_node *cur_node = _bt_node_load(bin, start); 

    if (cur_node->is_leaf == IS_LEAF)  {
        _bt_node_insert_key(cur_node, new_pair);
        return cur_node;
    }

    int child_pos = 0;
    for (child_pos = 0; child_pos < AMNT_KEYS; ++child_pos) {
        if (new_pair->c < cur_node->pairs[child_pos]->c) break;
    }

    start = cur_node->p[child_pos];
    printf("start: %d\n", start);
    bt_node *child = _bt_insert_key_recursive(bin, start, next_rrn, new_pair);

    /* Recursive ascent */
    if (child == NULL) return NULL;

    /* In case the child node has a free key space (without overflow), 
    and "ends" the recursive ascention */
    if (child->amnt_keys <= AMNT_KEYS) {  
        _bt_node_store(bin, child);
        return NULL;
    }

    // Treats any other overflow in child
    bt_node *splited = _bt_split_child(cur_node, child, next_rrn);
    _bt_node_store(bin, splited);
    _bt_node_store(bin, child);

    return cur_node;
}

void bt_insert_key(FILE *bin, int *root_rrn, int *next_rrn, key_pair *new_pair) {

    // In case there is no root node
    if (*root_rrn  == -1) {
        printf("Inserindo a raiz\n");
        bt_node *node = _bt_node_init(NODE_SIZE, IS_LEAF, 0);
        _bt_node_insert_key(node, new_pair);
        _bt_node_store(bin, node);

        *next_rrn += (2 * NODE_SIZE);
        *root_rrn = NODE_SIZE;

        return;
    }

    printf("Inserindo nó\n");
    bt_node *node = _bt_insert_key_recursive(bin, *root_rrn, next_rrn, new_pair);

    if (node != NULL) {
        // If there is a overflow in the root
        if (node->amnt_keys > AMNT_KEYS) { 
            printf("VSF caralho funciona pls\n");
            bt_node *new_root = _bt_node_init(*next_rrn, NOT_LEAF, 0);
            *root_rrn = *next_rrn;
            *next_rrn += NODE_SIZE;

            new_root->is_leaf = NOT_LEAF;
            new_root->p[0] = node->rrn;

            bt_node *splited = _bt_split_child(new_root, node, next_rrn);
            
            _bt_node_store(bin, new_root);
            _bt_node_store(bin, splited);
        }

        
        _bt_node_store(bin, node);
    }

}