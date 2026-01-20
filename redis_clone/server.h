/* server.h - Main header for C-Redis */
#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include <sys/epoll.h>
#include <netinet/in.h>

#define MAX_EVENTS 64
#define READ_BUFFER_SIZE 1024
#define WRITE_BUFFER_SIZE 4096

// Forward declarations
typedef struct slab_allocator_t slab_allocator_t;
typedef struct thread_pool_t thread_pool_t;

// Client state machine
typedef enum {
    STATE_READING,
    STATE_PROCESSING, // Handed off to worker thread
    STATE_WRITING,
    STATE_CLOSING
} client_state_t;

// Client connection state
typedef struct {
    int fd;
    client_state_t state;
    
    char read_buffer[READ_BUFFER_SIZE];
    int read_pos;
    
    // Niche C: Use atomic flags if state transitions happen across threads
    // For simplicity, we use a mutex here.
    pthread_mutex_t lock; // Protects write buffer and state
    char write_buffer[WRITE_BUFFER_SIZE];
    int write_pos;
    int total_to_write;
    
} client_t;

// Main server state
typedef struct server_t {
    int epoll_fd;
    int listen_fd;
    
    slab_allocator_t* client_slab; // Slab for client_t structs
    thread_pool_t* pool;         // Worker thread pool
    
} server_t;

// Prototypes
int create_and_bind(int port);
int set_nonblocking(int fd);
void server_run(server_t *s);

#endif // SERVER_H
