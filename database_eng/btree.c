#include "btree.h"
#include <stdlib.h>
#include <string.h>

// --- Private Helpers ---

// Read a node from disk at a specific offset
static void node_read(BTree *tree, long offset, BTreeNode *node) {
    fseek(tree->fp, offset, SEEK_SET);
    fread(node, sizeof(BTreeNode), 1, tree->fp);
}

// Write a node to disk at its 'self_offset'
static void node_write(BTree *tree, BTreeNode *node) {
    fseek(tree->fp, node->self_offset, SEEK_SET);
    fwrite(node, sizeof(BTreeNode), 1, tree->fp);
}

// Get a new, empty node (by appending to file)
static long get_new_node(BTree *tree, int is_leaf) {
    long offset = tree->next_free_offset;
    tree->next_free_offset += sizeof(BTreeNode);
    
    BTreeNode node;
    node.n = 0;
    node.leaf = is_leaf;
    node.self_offset = offset;
    
    node_write(tree, &node);
    return offset;
}

// Split a full child node 'y' of parent node 'x'
// i is the index of y in x.children[]
static void btree_split_child(BTree *tree, BTreeNode *x, int i) {
    // 1. Create a new node 'z'
    BTreeNode y, z;
    long y_offset = x->children[i];
    node_read(tree, y_offset, &y);
    
    long z_offset = get_new_node(tree, y.leaf);
    node_read(tree, z_offset, &z);
    
    z.n = T - 1;

    // 2. Move the last (T-1) keys from 'y' to 'z'
    for (int j = 0; j < T - 1; j++) {
        z.items[j] = y.items[j + T];
    }
    
    // 3. Move children if 'y' is not a leaf
    if (!y.leaf) {
        for (int j = 0; j < T; j++) {
            z.children[j] = y.children[j + T];
        }
    }
    y.n = T - 1; // 'y' now has T-1 keys

    // 4. Shift children in 'x' to make space for 'z'
    for (int j = x->n; j >= i + 1; j--) {
        x->children[j + 1] = x->children[j];
    }
    x->children[i + 1] = z_offset; // Link 'z'

    // 5. Shift keys in 'x' and move median key from 'y' up
    for (int j = x->n - 1; j >= i; j--) {
        x->items[j + 1] = x->items[j];
    }
    x->items[i] = y.items[T - 1]; // Median key
    x->n = x->n + 1;

    // 6. Write all modified nodes back to disk
    node_write(tree, &y);
    node_write(tree, &z);
    node_write(tree, x);
}

// Insert into a non-full node
static void btree_insert_nonfull(BTree *tree, long node_offset, KeyValue item) {
    BTreeNode node;
    node_read(tree, node_offset, &node);

    int i = node.n - 1;

    if (node.leaf) {
        // 1. Leaf node: find spot and insert
        while (i >= 0 && item.key < node.items[i].key) {
            node.items[i + 1] = node.items[i];
            i--;
        }
        node.items[i + 1] = item;
        node.n = node.n + 1;
        node_write(tree, &node);
    } else {
        // 2. Internal node: find child to recurse into
        while (i >= 0 && item.key < node.items[i].key) {
            i--;
        }
        i++; // Found the child
        
        // Load child to check if it's full
        BTreeNode child;
        node_read(tree, node.children[i], &child);
        
        if (child.n == MAX_KEYS) {
            // Child is full: split it
            btree_split_child(tree, &node, i);
            // After split, 'item' might go into the new child
            if (item.key > node.items[i].key) {
                i++;
            }
        }
        // Recurse
        btree_insert_nonfull(tree, node.children[i], item);
    }
}

// Recursive search
static int btree_search_internal(BTree *tree, long node_offset, int key, KeyValue *result) {
    BTreeNode node;
    node_read(tree, node_offset, &node);
    
    int i = 0;
    while (i < node.n && key > node.items[i].key) {
        i++;
    }
    
    // Found key
    if (i < node.n && key == node.items[i].key) {
        *result = node.items[i];
        return 1;
    }
    
    // Not found, and this is a leaf
    if (node.leaf) {
        return 0;
    }
    
    // Not found, recurse into child
    return btree_search_internal(tree, node.children[i], key, result);
}

// --- Public Functions ---

BTree* btree_create(const char *filename) {
    BTree *tree = (BTree*)malloc(sizeof(BTree));
    tree->fp = fopen(filename, "r+b");
    
    if (tree->fp == NULL) {
        // File doesn't exist, create it
        tree->fp = fopen(filename, "w+b");
        if (tree->fp == NULL) {
            perror("Error creating database file");
            free(tree);
            return NULL;
        }
        // Create root node
        tree->next_free_offset = 0;
        long root_offset = get_new_node(tree, 1); // Leaf
        tree->root_offset = root_offset;
        
        // Write metadata (root offset) to start of file
        fseek(tree->fp, 0, SEEK_SET);
        fwrite(&tree->root_offset, sizeof(long), 1, tree->fp);
        // ... a real DB would have a full header ...
        // We'll just assume root is *always* at offset 0
        // Let's simplify: root is at 0, data starts after.
        // --- SIMPLIFICATION ---
        // Let's reserve first 8 bytes for root_offset
        // and 8 bytes for next_free_offset.
        fseek(tree->fp, 0, SEEK_SET);
        long header[2] = {tree->root_offset, tree->next_free_offset};
        fwrite(header, sizeof(long), 2, tree->fp);
    } else {
        // File exists, read header
        long header[2];
        fseek(tree->fp, 0, SEEK_SET);
        fread(header, sizeof(long), 2, tree->fp);
        tree->root_offset = header[0];
        tree->next_free_offset = header[1];
    }
    return tree;
}

void btree_close(BTree *tree) {
    // Write the final root/free offsets to header
    fseek(tree->fp, 0, SEEK_SET);
    long header[2] = {tree->root_offset, tree->next_free_offset};
    fwrite(header, sizeof(long), 2, tree->fp);
    
    fclose(tree->fp);
    free(tree);
}

void btree_insert(BTree *tree, KeyValue item) {
    BTreeNode root;
    node_read(tree, tree->root_offset, &root);
    
    if (root.n == MAX_KEYS) {
        // Root is full, must split it
        long new_root_offset = get_new_node(tree, 0); // Not a leaf
        BTreeNode new_root;
        node_read(tree, new_root_offset, &new_root);
        
        new_root.children[0] = tree->root_offset;
        btree_split_child(tree, &new_root, 0);
        
        // Decide which child to insert into
        int i = 0;
        if (item.key > new_root.items[0].key) i++;
        btree_insert_nonfull(tree, new_root.children[i], item);
        
        // Update tree's root
        tree->root_offset = new_root_offset;
    } else {
        btree_insert_nonfull(tree, tree->root_offset, item);
    }
}

int btree_search(BTree *tree, int key, KeyValue *result) {
    return btree_search_internal(tree, tree->root_offset, key, result);
}
