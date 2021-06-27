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

void b_header_init(_files_t *files) {
    /* Gets the header line of the csv */

    int status = INC_STAT;
    int root_node = -1;
    int next_node = 0;
    char trash[] = '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@';

    /* fwrite & Error handling */
    if (fwrite(&status, sizeof(char), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&root_node, sizeof(int), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&next_node, sizeof(int), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(trash, sizeof(char), 68, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
}

/*
    Updates the 'global' part of a header. Can be used for both vehicle and line files given `finfo`.
*/
void b_header_update(FILE *bin, char stats, int next_node) {
    int next_node = ftell(bin);
    fseek(bin, 0, SEEK_SET);

    /* fwrite & Error handling */
    if (fwrite(&stats, sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (stats == INC_STAT) return; 

    if (fwrite(&next_node, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
}

b_node *_b_node_init(int rrn_tar) {
    b_node *node = malloc(sizeof(*node));

    node->is_leaf = IS_LEAF;
    node->amnt_keys = 0;
    node->rrn = rrn_tar;
    
    int p[5] = {-1};
    node->p = &p;

    //key_pair pair[4] = {.c = 0, .p_r = 0};
    
    node->pair = malloc(sizeof(key_pair *) * 4);
    for (int i = 0; i < 4; i++) {
        node->pair[i] = malloc(sizeof(key_pair));
        node->pair[i]->c = node->pair[i]->p_r = 0;
    }

    return node;
}

void _b_node_update(FILE *bin, b_node *node) {
    assert(node);
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
    for (int i = 0; i < 4; ++i) {     
        if (fwrite(&(node->pair[i]->c), sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fwrite(&(node->pair[i]->p_r), sizeof(long), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fwrite(&(node->p[i + 1]), sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
    }    
}

b_node *_b_node_load(FILE *bin, int rrn_tar) {
    fseek(bin, rrn_tar, SEEK_SET);

    b_node *node = malloc(sizeof(*node));
    if (fread(&node->is_leaf, sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fread(&node->amnt_keys, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fread(&node->rrn, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");

    for (int i = 0; i < 4; i++) {
        if (fread(&node->p[i], sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fread(&node->pair[i]->c, sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fread(&node->pair[i]->p_r, sizeof(long), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
    }

    if (fread(&node->p[4], sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");

    return node;
}

b_node *_b_node_leaf_insert_key(b_node *node, key_pair *new_pair) {
    int i;
    for (i = 0; i < AMNT_KEYS - 1; i++) {
        if (node->pair[i]->c > new_pair->c) break;
    }

    for (int j = AMNT_KEYS - 1; j > i; --j) node->pair[j] = node->pair[j - 1];
    node->pair[i] = new_pair;

    return node;
}

b_node *_b_node_search(FILE *bin, int c_target, int rrn_root) {
    if (rrn_root == -1) return NULL;
    
    b_node *cur_node = _b_node_load(bin, rrn_root);
    
    int i = 0;
    for (int i = 0; i < AMNT_KEYS; ++i) {
        if (c_target == cur_node->pair[i]->c) return cur_node;

        if (c_target < cur_node->pair[i]->c) break;
    }

    return _b_node_search(bin, c_target, cur_node->p[i]);
}


b_node *_b_get_leaf_node(FILE *bin, key_pair *new_pair, int rrn_root, int *next_rnn) {
    b_node *cur_node = _b_node_load(bin, rrn_root); // Searches for the leaf

    // if (cur_node->rrn = rrn_raiz) {
        // return root_overflow();
    //}


    if (cur_node->is_leaf == IS_LEAF)  {
        return cur_node;
    }

    int i = 0;
    for (i = 0; i < AMNT_KEYS; ++i) {
        if (new_pair->c < cur_node->pair[i]->c) break;
    }
/
    
    b_node *child = _b_get_leaf_node(bin, new_pair, cur_node->p[i]);

    if (child->amnt_keys < AMNT_KEYS) { // Retorna o filho caso a folha encontrada já esteja prontinha (tenha espaço)
        return child;        
    }

    _b_slipt_wantchutchu(bin, child, new_pair, next_rnn);
    // Fazer e escrever o nó atual dentro da função
    // Arrumar os p
    // trocar os new_pair
    // next_rnn
    // passar o pay

    
    // if (cur_node->amnt_keys < AMNT_KEYS) { // Terminou overflow
    //     // cur_node->p[i + 1] = splited_child->rrn;
    //     // new_pair = child->pair[2];
        
    //     // cur_node->pair[i] = child->pair[2];
    //     // child->pair[2]->c = 0;
    //     // child->pair[2]->p_r = 0; 
    // }

    return cur_node; 

    
    
    


}


int _b_read_root_rrn(FILE *bin) {
    fseek(bin, sizeof(char), SEEK_SET);

    int root_rrn;
    if (fread(&root_rrn, sizeof(int), 1, bin) != 1);
        file_error("Falha no processamento do arquivo");

    return root_rrn;
}

int _b_insert_key(FILE *bin, int rrn_root, key_pair *pair) {  
    if (rrn_root == -1) { // In case the root is empty
        b_node *node = _b_node_init(bin, NODE_SIZE);
        node = _b_node_leaf_insert_key(node, pair);
        _b_node_update(bin, node);
        
        return NODE_SIZE;
    }

    b_node *node = _b_get_leaf_node(bin, pair->c, rrn_root);
    
    if (node->amnt_keys < AMNT_KEYS) {
        node = _b_node_leaf_insert_key(node, pair);
        _b_node_update(bin, node);

        return rrn_root;
    }



}

key_pair **_get_overflow_pairs(b_node *original_node, key_pair *new_pair) {
    key_pair **overflow_pairs = malloc(sizeof(*overflow_pairs) * 5);

    int i;
    for (i = 0; i < 4; ++i) {
        if (new_pair->c < original_node->pair[i]->c) break; 
        overflow_pairs[i] = original_node->pair[i];        
    }

    overflow_pairs[i] = new_pair;
    for (++i; i < 5; ++i) overflow_pairs[i] = original_node->pair[i - 1];

    return overflow_pairs;
}


b_node *_b_slipt_wantchutchu(FILE *bin, b_node *original_node, key_pair *new_pair, int *next_rrn) {
    key_pair **overflow_pairs = _get_overflow_pairs(original_node, new_pair);
    b_node *new_node = _b_node_init(*next_rrn);

    original_node->amnt_keys = 3;
    original_node->pair[0] = overflow_pairs[0];
    original_node->pair[1] = overflow_pairs[1]; 
    original_node->pair[2] = overflow_pairs[2]; 

    new_node->amnt_keys = 2;
    new_node->pair[0] = overflow_pairs[3];
    new_node->pair[1] = overflow_pairs[4];

    _b_node_update(bin, new_node);
    *next_rrn += NODE_SIZE;

    return new_node;
}

