/* thread_pool.c - Implementation of the thread pool */
#include "thread_pool.h"

// --- Forward Declaration ---
void handle_client_command(hash_table_t* db, work_item_t* work, char* response_buf, size_t response_max);

/**
 * @brief The function each worker thread executes.
 */
static void* worker_thread_func(void* arg) {
    thread_pool_t* pool = (thread_pool_t*)arg;
    
    LOG("Worker thread %lu started.", pthread_self());
    
    while (!atomic_load_explicit(&pool->shutdown, memory_order_relaxed)) {
        work_item_t* work = NULL;
        
        // --- Try to get work from the lock-free queue ---
        work = (work_item_t*)lf_queue_pop(pool->work_queue);
        
        if (work == NULL) {
            // --- Queue is empty, wait on condition variable ---
            // Niche C: This avoids busy-waiting and saves CPU.
            pthread_mutex_lock(&pool->queue_lock);
            // Check again *after* acquiring the lock
            work = (work_item_t*)lf_queue_pop(pool->work_queue);
            if (work == NULL && !atomic_load_explicit(&pool->shutdown, memory_order_relaxed)) {
                LOG("Worker %lu waiting...", pthread_self());
                pthread_cond_wait(&pool->queue_cond, &pool->queue_lock);
                LOG("Worker %lu woken up.", pthread_self());
            }
            pthread_mutex_unlock(&pool->queue_lock);
            
            // If we were woken up but shutdown is true, exit
            if (atomic_load_explicit(&pool->shutdown, memory_order_relaxed)) {
                break;
            }
            // If we were woken up because work arrived *after* the pop but *before* the wait
            if (work == NULL) work = (work_item_t*)lf_queue_pop(pool->work_queue);
            if (work == NULL) continue; // Spurious wakeup or shutdown
        }
        
        // --- Process the work item ---
        LOG("Worker %lu processing request for fd %d", pthread_self(), work->client_fd);
        
        char response_buf[WRITE_BUFFER_SIZE]; // Prepare response buffer
        handle_client_command(pool->db, work, response_buf, WRITE_BUFFER_SIZE);
        
        // --- Prepare response to be sent by main thread ---
        // Niche C: We CANNOT send() here as it might block.
        // We need to store the response and tell the main thread
        // (via epoll) that this socket is ready for writing.
        
        client_t* client = &pool->client_slab[work->client_fd]; // Get client struct

        // For simplicity, copy response to client's write buffer
        // A real server might use a separate response queue.
        pthread_mutex_lock(&client->lock); // Need lock to access client buffer
        strncpy(client->write_buffer, response_buf, WRITE_BUFFER_SIZE - 1);
        client->write_buffer[WRITE_BUFFER_SIZE - 1] = '\0';
        client->total_to_write = strlen(client->write_buffer);
        client->write_pos = 0; // Reset write position
        client->state = STATE_WRITING; // Set state
        pthread_mutex_unlock(&client->lock);

        // Tell the main thread's epoll to watch for EPOLLOUT
        struct epoll_event event;
        event.events = EPOLLOUT | EPOLLIN | EPOLLET; // Watch for write-ready + read
        event.data.fd = work->client_fd;
        if (epoll_ctl(work->server->epoll_fd, EPOLL_CTL_MOD, work->client_fd, &event) == -1) {
            perror("epoll_ctl MOD in worker");
            // Handle error, maybe close client?
        }

        // We malloc'd this in the main thread
        free(work);
    }
    
    LOG("Worker thread %lu shutting down.", pthread_self());
    return NULL;
}


thread_pool_t* thread_pool_create(hash_table_t* db, slab_allocator_t *client_slab) {
    thread_pool_t* pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    if (!pool) ERROR_EXIT("malloc thread_pool_t");
    
    pool->work_queue = lf_queue_create();
    pool->db = db;
    pool->client_slab = client_slab; // Store slab allocator pointer
    pthread_mutex_init(&pool->queue_lock, NULL);
    pthread_cond_init(&pool->queue_cond, NULL);
    atomic_init(&pool->shutdown, false);
    
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread_func, pool) != 0) {
            ERROR_EXIT("pthread_create worker");
        }
    }
    
    LOG("Thread pool created with %d workers.", NUM_WORKER_THREADS);
    return pool;
}

void thread_pool_destroy(thread_pool_t* pool) {
    atomic_store_explicit(&pool->shutdown, true, memory_order_release);
    
    // Wake up all threads so they can check the shutdown flag
    pthread_mutex_lock(&pool->queue_lock);
    pthread_cond_broadcast(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_lock);
    
    // Wait for all threads to finish
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    lf_queue_destroy(pool->work_queue); // Assumes queue is empty now
    pthread_mutex_destroy(&pool->queue_lock);
    pthread_cond_destroy(&pool->queue_cond);
    free(pool);
    LOG("Thread pool destroyed.");
}

void thread_pool_add_work(thread_pool_t* pool, work_item_t* work) {
    lf_queue_push(pool->work_queue, work);
    
    // Signal *one* waiting thread
    pthread_mutex_lock(&pool->queue_lock);
    pthread_cond_signal(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_lock);
}


/**
 * @brief Parses the client command and interacts with the hash table.
 * This is the "application logic" executed by the worker thread.
 */
void handle_client_command(hash_table_t* db, work_item_t* work, char* response_buf, size_t response_max) {
    // Simple text protocol: "CMD key [value]\r\n"
    char command[16], key[256], value[512];
    int parsed = sscanf(work->request, "%s %s %s", command, key, value);

    if (parsed >= 2) {
        if (strcasecmp(command, "GET") == 0) {
            char* found_value = ht_get(db, key);
            if (found_value) {
                // Respond with "+VALUE <value>\r\n" (simplified RESP-like)
                snprintf(response_buf, response_max, "+%s\r\n", found_value);
                free(found_value); // ht_get mallocs
            } else {
                // Respond with "$-1\r\n" (Null bulk string)
                snprintf(response_buf, response_max, "$-1\r\n");
            }
        } else if (strcasecmp(command, "SET") == 0 && parsed == 3) {
            ht_set(db, key, value);
            // Respond with "+OK\r\n"
            snprintf(response_buf, response_max, "+OK\r\n");
        } else {
            // Respond with "-ERR Unknown command or wrong args\r\n"
            snprintf(response_buf, response_max, "-ERR Unknown command or wrong args\r\n");
        }
    } else {
        // Respond with "-ERR Invalid command format\r\n"
        snprintf(response_buf, response_max, "-ERR Invalid command format\r\n");
    }
}
