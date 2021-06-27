#include "stream.h"
#include "global.h"
#include "vehicle.h"
#include "line.h"

#define IS_LEAF '1'
#define NOT_LEAF '0'
#define NODE_SIZE 77
#define NODE_DEGREE 5

#define AMNT_KEYS 4

typedef struct {
    int c;
    long p_r;
} key_pair;

typedef struct {
    char is_leaf;
    int amnt_keys;
    int rrn;
    int p[5];
    key_pair pair[4];
} b_node;

void b_header_init(_files_t *files);