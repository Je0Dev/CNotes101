/* thread_pool.h - Simple fixed-size thread pool */
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "common.h"
#include "lf_queue.h"
#include "hash_table.h" // For the database handle
#include "server.h"     // For client_t

// --- Configuration ---
#define NUM_WORKER_THREADS 4

// --- Structures ---

// Represents a work item for the thread pool
typedef struct {
    int client_fd;
    char request[1024]; // Simplified: Assume fixed max request size
    // We need the server struct to modify epoll interest
    server_t *server; 
} work_item_t;

// The thread pool state
typedef struct {
    pthread_t threads[NUM_WORKER_THREADS];
    lf_queue_t* work_queue;         // Queue for incoming requests
    hash_table_t* db;             // Handle to the main database
    slab_allocator_t *client_slab; // Need this to access client data
    
    // Niche C: We need a condition variable to wake up threads efficiently
    pthread_mutex_t queue_lock;     // Lock for the condition variable
    pthread_cond_t queue_cond;      // Condition variable
    atomic_bool shutdown;           // Flag to signal threads to exit
    
} thread_pool_t;

// --- Public API ---
thread_pool_t* thread_pool_create(hash_table_t* db, slab_allocator_t *client_slab);
void thread_pool_destroy(thread_pool_t* pool);
void thread_pool_add_work(thread_pool_t* pool, work_item_t* work);

#endif // THREAD_POOL_H
