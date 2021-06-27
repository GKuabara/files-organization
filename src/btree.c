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

void _b_node_init(FILE *bin, int rrn_tar) {
    fseek(bin, rrn_tar, SEEK_SET);

    /* Initializes the header of each node */
    char is_leaf = IS_LEAF;
    int amnt_keys = 0;

    if (fwrite(&is_leaf, sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&amnt_keys, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&rrn_tar, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");


    /* Initializes the key pairs of the node */ 
    int p = -1;

    if (fwrite(&p, sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    for (int i = 0; i < 4; ++i) {
        int c = 0;
        long p_r = 0;
        
        if (fwrite(&c, sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fwrite(&p_r, sizeof(long), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fwrite(&p, sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
    }
}

void update_node(FILE *bin, b_node *node) {
    assert(node);
    fseek(bin, node->rrn, SEEK_SET);
    
    /* Updates the header of each node */
    if (fwrite(&(node->is_leaf), sizeof(char), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&(node->amnt_keys), sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&(node->rrn), sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    
    /* Initializes the key pairs of the node */ 
    if (fwrite(&(node->p[0]), sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");
    for (int i = 0; i < 4; ++i) {     
        if (fwrite(&(node->pair[i].c), sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fwrite(&(node->pair[i].p_r), sizeof(long), 1, bin) != 1)
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
        if (fread(&node->pair[i].c, sizeof(int), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
        if (fread(&node->pair[i].p_r, sizeof(long), 1, bin) != 1)
            file_error("Falha no processamento do arquivo");
    }

    if (fread(&node->p[4], sizeof(int), 1, bin) != 1)
        file_error("Falha no processamento do arquivo");

    return node;
}

b_node *_b_node_leaf_insert_key(b_node *node, key_pair new_pair) {
    int i;
    for (i = 0; i < AMNT_KEYS - 1; i++) {
        if (node->pair[i].c > new_pair.c) {
            break;
        }
    }

    for (int j = AMNT_KEYS - 1; j > i; --j ) {
            node->pair[j].c = node->pair[j - 1].c;
            node->pair[j].p_r = node->pair[j - 1].p_r;  
    }

    node->pair[i].c = new_pair.c;
    node->pair[i].p_r = new_pair.p_r;

    return node;
}

b_node *_b_node_search(FILE *bin, int c_target, int rrn_root) {
    if (rrn_root == -1)
        return NULL;
    
    b_node *cur_node = _b_node_load(bin, rrn_root);
    int i = 0;

    for (int i = 0; i < AMNT_KEYS; ++i) {
        if (c_target == cur_node->pair[i].c) return cur_node;

        if (c_target < cur_node->pair[i].c) break;
    }

    return _b_node_search(bin, c_target, cur_node->p[i]);
}