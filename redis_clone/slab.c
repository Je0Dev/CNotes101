#include "slab.h"
#include <sys/mman.h> // for mmap/munmap

/**
 * @brief Allocates a new slab from the OS.
 */
static bool slab_grow(slab_allocator_t* allocator) {
    if (allocator->num_slabs >= MAX_SLABS) {
        fprintf(stderr, "Slab Allocator: Max slabs reached.\n");
        return false;
    }
    
    // Allocate a new slab using mmap
    void* new_slab = mmap(NULL, SLAB_SIZE, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_slab == MAP_FAILED) {
        perror("mmap failed in slab_grow");
        return false;
    }
    
    allocator->slabs[allocator->num_slabs++] = new_slab;
    LOG("Grew slab allocator. New slab %d at %p", allocator->num_slabs, new_slab);

    // Carve up the new slab and add its blocks to the free list
    for (size_t i = 0; i < allocator->slab_item_count; i++) {
        free_block_t* block = (free_block_t*)((char*)new_slab + (i * allocator->item_size));
        block->next = allocator->free_list;
        allocator->free_list = block;
    }
    return true;
}

slab_allocator_t* slab_create(size_t item_size) {
    // Niche C: Ensure item_size is at least the size of a pointer
    // so we can overlay the free_block_t struct.
    item_size = MAX(item_size, sizeof(free_block_t));
    
    // Niche C: Align item_size for potentially better performance
    // (though correctness is guaranteed by the MAX above).
    item_size = (item_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    
    slab_allocator_t* allocator = (slab_allocator_t*)malloc(sizeof(slab_allocator_t));
    if (!allocator) ERROR_EXIT("malloc slab_allocator_t");
    
    allocator->item_size = item_size;
    allocator->slab_item_count = SLAB_SIZE / item_size;
    allocator->num_slabs = 0;
    allocator->free_list = NULL;
    pthread_mutex_init(&allocator->lock, NULL);
    
    // Pre-allocate one slab
    if (!slab_grow(allocator)) {
        free(allocator);
        return NULL;
    }
    
    LOG("Slab allocator created: item_size=%zu, items_per_slab=%zu",
        allocator->item_size, allocator->slab_item_count);
    return allocator;
}

void slab_destroy(slab_allocator_t* allocator) {
    pthread_mutex_lock(&allocator->lock);
    for (int i = 0; i < allocator->num_slabs; i++) {
        munmap(allocator->slabs[i], SLAB_SIZE);
    }
    pthread_mutex_unlock(&allocator->lock);
    pthread_mutex_destroy(&allocator->lock);
    free(allocator);
}

void* slab_alloc(slab_allocator_t* allocator) {
    pthread_mutex_lock(&allocator->lock);
    
    // If free list is empty, try to grow
    if (allocator->free_list == NULL) {
        if (!slab_grow(allocator)) {
            pthread_mutex_unlock(&allocator->lock);
            return NULL; // Out of memory
        }
    }
    
    // Pop from free list
    free_block_t* block = allocator->free_list;
    allocator->free_list = block->next;
    
    pthread_mutex_unlock(&allocator->lock);
    
    // Return the pointer to the user data area
    return (void*)block;
}

void slab_free(slab_allocator_t* allocator, void* ptr) {
    if (!ptr) return;
    
    // Niche C: Overlay the free_block_t struct onto the freed memory
    free_block_t* block = (free_block_t*)ptr;
    
    pthread_mutex_lock(&allocator->lock);
    
    // Push onto free list
    block->next = allocator->free_list;
    allocator->free_list = block;
    
    pthread_mutex_unlock(&allocator->lock);
}
