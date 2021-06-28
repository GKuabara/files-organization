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

/* Initizalizes the btree file header */
void bt_header_init(_files_t *files) {
    int status = INC_STAT;
    int root_rrn = -1;
    int next_node_rrn = 0;
    char trash = '@';

    /* fwrite & Error handling */
    if (fwrite(&status, sizeof(char), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&root_rrn, sizeof(int), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");
    if (fwrite(&next_node_rrn, sizeof(int), 1, files->bin) != 1)
        file_error("Falha no processamento do arquivo");

    for (int i = 0; i < 68; ++i) {
        if (fwrite(&trash, sizeof(char), 1, files->bin) != 1)
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

/* Searchs and returns a key pair */
key_pair *b_search_key(FILE *bin, int root_rrn, int c_tar) {
    if (root_rrn == -1) return NULL;
    
    bt_node *cur_node = _b_node_load(bin, root_rrn);
    
    int i = 0;
    for (int i = 0; i < AMNT_KEYS; ++i) {
        if (c_tar == cur_node->pairs[i]->c) return cur_node->pairs[i];

        if (c_tar < cur_node->pairs[i]->c) break;
    }

    return _b_node_search(bin, c_tar, cur_node->p[i]);
}

/* 
    Initializes a new btree node.
    Callers are responsible for `free()`ing it.
*/
bt_node *_bt_node_init(int *next_rrn) {
    bt_node *new_node = malloc(sizeof(*new_node));

    new_node->is_leaf = IS_LEAF;
    new_node->amnt_keys = 0;
    new_node->rrn = *next_rrn;
    memset(&new_node->p, -1, BT_DEGREE);
    
    for (int i = 0; i < AMNT_KEYS; i++) {
        new_node->pairs[i] = malloc(sizeof(*new_node->pairs[i]));
        new_node->pairs[i]->c = new_node->pairs[i]->p_r = -1;
    }

    *next_rrn += 77;
    return new_node;
}

/* 
    Stores a btree node from memory to disk 
*/
void _bt_node_store(FILE *bin, bt_node *node) {
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
bt_node *_bt_node_load(FILE *bin, int rrn_tar) {
    fseek(bin, rrn_tar, SEEK_SET);

    bt_node *node = malloc(sizeof(*node));

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
    Inserts a new pair into a node without overflow
*/
int _bt_leaf_insert_key(bt_node *node, key_pair *new_pair) {
    int i;
    for (i = 0; i < AMNT_KEYS - 1; i++) {
        if (node->pairs[i]->c > new_pair->c) break;
    }

    for (int j = AMNT_KEYS - 1, k = BT_DEGREE - 1; j > i; --j, --k)  {
        node->pairs[j] = node->pairs[j - 1];
        node->p[k] = node->p[k - 1];
    }

    node->pairs[i] = new_pair;
    node->p[i + 1] = -1;

    return i;
}

bt_node *_bt_insert_key_recursive_2(FILE *bin, int *root_rrn, int *next_rrn, key_pair *new_pair) {
    bt_node *cur_node = _b_node_load(bin, root_rrn); 

    int i = 0;
    for (i = 0; i < AMNT_KEYS; ++i) {
        if (new_pair->c < cur_node->pairs[i]->c) break;
    }

    bt_node *child = NULL;
    if (cur_node->is_leaf == NOT_LEAF) 
        child =_bt_insert_key_recursive(bin, root_rrn, next_rrn, cur_node->p[i]);
    
    else if (cur_node->amnt_keys < AMNT_KEYS){
        _bt_leaf_insert_key(cur_node, new_pair);
        new_pair = NULL;
        return cur_node;
    }

    if (new_pair == NULL) return cur_node;

    bt_node *new_child = _bt_split_child_3(..., cur_node, i, next_rrn, new_pair);
}

/*
    Recursive call to insert key at the b-tree, inside the function
    there are 3 cases to insert when spliting:
    1 - b-tree has any node,
    2 - b-tree has a full root,
    3 - node of insertion is full
*/
bt_node *_bt_insert_key_recursive(FILE *bin, int *root_rrn, int *next_rrn, key_pair *new_pair) {
    bt_node *cur_node = _b_node_load(bin, root_rrn); 

    if (cur_node->is_leaf == IS_LEAF)  {
        return cur_node;
    }

    int i = 0;
    for (i = 0; i < AMNT_KEYS; ++i) {
        if (new_pair->c < cur_node->pairs[i]->c) break;
    }

    /* Recursive decends */
    bt_node *child = _bt_insert_key_recursive(bin, root_rrn, next_rrn, cur_node->p[i]);

    // caso não tiver new_pair, quer dizer que a inserção incial foi feita
    // e todas as possíveis promoções também
    if (new_pair == NULL) return cur_node;

    // se o nó filho tiver espaço a gente insere normlamente
    // e retorna esse filho nas chamadas recursivas
    if (child->amnt_keys < AMNT_KEYS) {
        
        // caso o nó que inserimos nao for folha, entao esta chave inserida através
        // de uma promoção, logo devemos atualizar o rrn do nó junto da inserção
        if (child->is_leaf == NOT_LEAF) {
            child->pairs[i] = new_pair;
        }
        else {
            int pos = _bt_leaf_insert_key(child, new_pair);
        }

        new_pair = NULL;
        return cur_node;        
    }
    
    // a partir daqui somente casos de overflow

    // se for nó raiz, tratamento deve ser diferente
    else if (cur_node->rrn == NODE_SIZE) {
        
        // podemos fazer inserção sem oveflow no nó raiz
        if (cur_node->amnt_keys < AMNT_KEYS) {
            int pos = _bt_leaf_insert_key(cur_node, new_pair);
            // cur_node->p[pos] = *next_rrn;
            // *next_rrn += NODE_SIZE;

            new_pair = NULL;
            return cur_node;
        }

        // temos que criar dois novos nós:
        // um para ser a nova raiz e outro para ser o irmão da raiz atual
        else {
            bt_node *new_child = _bt_split_child(bin, cur_node, next_rrn, new_pair);
            bt_node *new_root = _bt_node_init(*next_rrn);
            
            new_root->pairs[0] = cur_node->pairs[2];
            cur_node->pairs[2] = NULL;
            new_root->p[0] = cur_node->rrn;
            new_root->p[1] = new_child->rrn;
            *root_rrn = new_root->rrn;

            new_pair = NULL;
            return new_root;
        }
    }

    // overflow de qualquer outro nó sem ser raiz
    else {
        bt_node *new_child = _bt_split_child_3(cur_node, child, i, next_rrn, new_pair);
        
        // fazemos o nó apontar para o novo filho criado no split
        cur_node->p[i + 1] = new_child->rrn;

        _bt_node_update(bin, cur_node);
        _bt_node_update(bin, child);
        _bt_node_update(bin, new_child);
    }
        
    return cur_node; 
}

bt_node *_bt_split_child_3(bt_node *parent, bt_node *child, int child_pos, int *next_rrn, key_pair *new_pair) {

    // inserindo no nó com overflow
    child = _bt_overflow_insert_key(child, new_pair);
    
    bt_node *new_child = _bt_node_init(*next_rrn);

    // copiando para novo nó as duas ultimas chaves e RNNs
    // ao mesmo tempo que esvazia o nó original
    for (int i = 0, j = 3; j < AMNT_KEYS; i++, j++) {
        new_child->p[i] = child->p[j];
        new_child->pairs[i] = child->pairs[j]; 
        child->p[j] = -1;
        child->pairs[j] = NULL;
    }
    new_child->p[2] = child->p[5];

    // seta a chave a ser promovida como new_pair e remove a mesma do nó
    new_pair = child->pairs[2];
    child->pairs[2] = NULL;

    return new_child;
}

bt_node *_bt_overflow_insert_key(bt_node *node, key_pair *new_pair) {
    int i;

    // acha posição para inserção
    for (i = 0; i < AMNT_KEYS; i++) {
        if (node->pairs[i]->c > new_pair->c) break;
    }

    // shift as chaves e RRNs
    for (int j = AMNT_KEYS, k = BT_DEGREE; j > i; --j, --k)  {
        node->pairs[j] = node->pairs[j - 1];
        node->p[k] = node->p[k - 1];
    }

    // posiciona novo nó
    node->pairs[i] = new_pair;
    node->p[i + 1] = -1;

    return i;
}

bt_node *_bt_split_child(FILE *bin, bt_node *parent, int child_pos, bt_node *child, int *next_rrn, key_pair *new) {

    key_pair **overflow_pairs = _bt_get_overflow_pairs(child, new);

    bt_node *new_child = _bt_node_init(next_rrn);

    int i;
    for(i = 0; i < 3; i++) child->pairs[i] = overflow_pairs[i];
    for(; i < 5; i++) new_child->pairs[i] = overflow_pairs[i];

    _bt_node_update(bin, new_child);
    *next_rrn += NODE_SIZE;

    return new_child;
}



key_pair *_bt_split_child2(FILE *bin, bt_node *parent, bt_node *child, int child_pos, int *next_rrn, key_pair *new) { // Assumes the parent is not full
    bt_node *splited = _bt_node_init(*next_rrn);
    
    splited->is_leaf = child->is_leaf;
    splited->amnt_keys = 2;
    child->amnt_keys = 2;

    for (int i = 0; i < 2; ++i) {
        splited->pairs[i] = child->pairs[i+2];
        child->pairs[i+2] = NULL;
    }
    for (int i = 0; i < 3; ++i) {
        splited->p[i] = child->pairs[i + 2]; 
        child->p[i+2] = -1;
    } // Copies the p pointers in case its not a leaf

    // for (int i = parent->amnt_keys - 1; i > child_pos; --i) {
    //     parent->pairs[i] = parent->pairs[i - 1];
    // } 

    // parent->pairs[child_pos] = 
   
    // for (int i = parent->amnt_keys; i > child_pos; --i) {
    //     parent->p[i + 1] = parent->p[i]; 
    // }
    // parent->p[child_pos + 1] = *next_rrn;
}

key_pair **_bt_get_overflow_pairs(bt_node *original_node, key_pair *new_pair) {
    key_pair **overflow_pairs = malloc(sizeof(*overflow_pairs) * 5);

    int i;
    for (i = 0; i < 4; ++i) {
        if (new_pair->c < original_node->pairs[i]->c) break; 
        overflow_pairs[i] = original_node->pairs[i];  
    }

    overflow_pairs[i] = new_pair;
    for (++i; i < 5; ++i) overflow_pairs[i] = original_node->pairs[i - 1];

    return overflow_pairs;
}

// /*
    
// */
// void _bt_insert_key(FILE *bin, int root_rrn, int next_node_rrn, key_pair *new_pair) {  

// }









