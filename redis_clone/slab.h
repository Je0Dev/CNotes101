/* slab.h - Simple fixed-size slab allocator */
#ifndef SLAB_H
#define SLAB_H

#include "common.h"

// --- Configuration ---
#define SLAB_SIZE (1024 * 1024) // Allocate 1MB slabs
#define MAX_SLABS 100           // Max total slabs

// --- Structures ---

// A single block within a slab (part of the free list)
typedef struct free_block_t {
    struct free_block_t *next;
} free_block_t;

// The slab allocator state
typedef struct {
    size_t item_size;             // Size of each item we allocate
    size_t slab_item_count;       // How many items fit in one slab
    
    void* slabs[MAX_SLABS];       // Pointers to the start of each slab
    int num_slabs;
    
    free_block_t *free_list;      // Head of the linked list of free blocks
    pthread_mutex_t lock;         // Mutex to protect the free list
    
} slab_allocator_t;

// --- Public API ---
slab_allocator_t* slab_create(size_t item_size);
void slab_destroy(slab_allocator_t* allocator);
void* slab_alloc(slab_allocator_t* allocator);
void slab_free(slab_allocator_t* allocator, void* ptr);

#endif // SLAB_H
