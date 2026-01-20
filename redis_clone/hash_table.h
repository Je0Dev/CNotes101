/* hash_table.h - Mutex-protected hash table */
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "common.h"

// --- Configuration ---
#define HT_INITIAL_CAPACITY 16

// --- Structures ---

// Entry in the hash table
typedef struct ht_entry_t {
    char* key;
    char* value;
    struct ht_entry_t* next; // For collision chaining
} ht_entry_t;

// The hash table itself
typedef struct {
    ht_entry_t** buckets;
    size_t capacity;
    size_t count;
    pthread_mutex_t lock; // Mutex to protect the table
} hash_table_t;

// --- Public API ---
hash_table_t* ht_create();
void ht_destroy(hash_table_t* ht);
void ht_set(hash_table_t* ht, const char* key, const char* value);
char* ht_get(hash_table_t* ht, const char* key); // Returns malloc'd value or NULL
void ht_delete(hash_table_t* ht, const char* key);

#endif // HASH_TABLE_H
