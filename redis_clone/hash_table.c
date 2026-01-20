/* hash_table.c - Implementation of the hash table */
#include "hash_table.h"

// --- Hash Function (djb2) ---
static size_t hash_key(const char* key) {
    size_t hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

hash_table_t* ht_create() {
    hash_table_t* ht = (hash_table_t*)malloc(sizeof(hash_table_t));
    if (!ht) ERROR_EXIT("malloc hash_table_t");
    
    ht->capacity = HT_INITIAL_CAPACITY;
    ht->count = 0;
    // Niche C: Use calloc to zero-initialize the bucket pointers
    ht->buckets = (ht_entry_t**)calloc(ht->capacity, sizeof(ht_entry_t*));
    if (!ht->buckets) ERROR_EXIT("calloc buckets");
    
    pthread_mutex_init(&ht->lock, NULL);
    return ht;
}

static void ht_free_entry(ht_entry_t* entry) {
    free(entry->key);
    free(entry->value);
    free(entry);
}

void ht_destroy(hash_table_t* ht) {
    pthread_mutex_lock(&ht->lock);
    for (size_t i = 0; i < ht->capacity; i++) {
        ht_entry_t* entry = ht->buckets[i];
        while (entry) {
            ht_entry_t* next = entry->next;
            ht_free_entry(entry);
            entry = next;
        }
    }
    free(ht->buckets);
    pthread_mutex_unlock(&ht->lock);
    pthread_mutex_destroy(&ht->lock);
    free(ht);
}

// TODO: Add ht_resize() for production use

void ht_set(hash_table_t* ht, const char* key, const char* value) {
    pthread_mutex_lock(&ht->lock);
    
    size_t index = hash_key(key) % ht->capacity;
    ht_entry_t* entry = ht->buckets[index];
    
    // Check if key already exists (update)
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            free(entry->value); // Free old value
            entry->value = strdup(value);
            pthread_mutex_unlock(&ht->lock);
            return;
        }
        entry = entry->next;
    }
    
    // Key not found, create new entry
    ht_entry_t* new_entry = (ht_entry_t*)malloc(sizeof(ht_entry_t));
    new_entry->key = strdup(key);
    new_entry->value = strdup(value);
    
    // Prepend to the bucket's linked list
    new_entry->next = ht->buckets[index];
    ht->buckets[index] = new_entry;
    ht->count++;
    
    pthread_mutex_unlock(&ht->lock);
    
    // TODO: Check load factor and call ht_resize if needed
}

char* ht_get(hash_table_t* ht, const char* key) {
    pthread_mutex_lock(&ht->lock);
    
    size_t index = hash_key(key) % ht->capacity;
    ht_entry_t* entry = ht->buckets[index];
    
    char* result = NULL;
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            result = strdup(entry->value); // Return a copy
            break;
        }
        entry = entry->next;
    }
    
    pthread_mutex_unlock(&ht->lock);
    return result;
}

void ht_delete(hash_table_t* ht, const char* key) {
    pthread_mutex_lock(&ht->lock);
    
    size_t index = hash_key(key) % ht->capacity;
    ht_entry_t** indirect = &ht->buckets[index]; // Pointer to the pointer
    
    while (*indirect) {
        if (strcmp((*indirect)->key, key) == 0) {
            ht_entry_t* entry_to_delete = *indirect;
            *indirect = entry_to_delete->next; // Bypass the node
            ht_free_entry(entry_to_delete);
            ht->count--;
            break;
        }
        indirect = &(*indirect)->next;
    }
    
    pthread_mutex_unlock(&ht->lock);
}
