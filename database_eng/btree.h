#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>

// B-Tree 'T' parameter (degree). T=2 means a 2-3 Tree
// Max keys = 2*T - 1 = 3
// Min keys = T - 1 = 1
#define T 2
#define MAX_KEYS (2 * T - 1)
#define MIN_KEYS (T - 1)

// --- Data Structures ---

typedef struct {
    int key;
    char value[100]; // Our value
} KeyValue;

typedef struct {
    int n; // Current number of keys
    int leaf; // 1 if leaf, 0 if internal
    long self_offset; // This node's offset in the file
    
    KeyValue items[MAX_KEYS];
    long children[MAX_KEYS + 1]; // File offsets for children
} BTreeNode;

typedef struct {
    FILE *fp; // File pointer to the database file
    long root_offset; // Offset of the root node
    long next_free_offset; // Where to write the next new node
} BTree;

// --- Public Functions ---
BTree* btree_create(const char *filename);
void btree_close(BTree *tree);
void btree_insert(BTree *tree, KeyValue item);
int btree_search(BTree *tree, int key, KeyValue *result);

#endif // BTREE_H
