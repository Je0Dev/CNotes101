/* main.c - C-Redis Server Entry Point */
#include "server.h"
#include "slab.h"
#include "thread_pool.h"
#include "hash_table.h"
#include <signal.h>

#define DEFAULT_PORT 6379

static server_t server;
static hash_table_t* database;

// Handle Ctrl+C
void handle_shutdown(int sig) {
    (void)sig;
    printf("\nShutting down C-Redis...\n");
    
    // Signal thread pool to shutdown
    thread_pool_destroy(server.pool);
    
    // Close listening socket
    close(server.listen_fd);
    
    // Close epoll fd
    close(server.epoll_fd);

    // Destroy hash table
    ht_destroy(database);
    
    // Destroy slab allocator (this assumes all clients were closed)
    slab_destroy(server.client_slab);
    
    printf("C-Redis shut down gracefully.\n");
    exit(0);
}

int main(void) {
    signal(SIGINT, handle_shutdown);
    signal(SIGPIPE, SIG_IGN); // Important for network servers

    // --- 1. Initialize Core Components ---
    database = ht_create();
    server.client_slab = slab_create(sizeof(client_t));
    server.pool = thread_pool_create(database, server.client_slab); // Pass db and slab

    // --- 2. Create Listening Socket ---
    server.listen_fd = create_and_bind(DEFAULT_PORT);

    // --- 3. Create Epoll Instance ---
    server.epoll_fd = epoll_create1(0);
    if (server.epoll_fd == -1) ERROR_EXIT("epoll_create1");

    // --- 4. Add Listening Socket to Epoll ---
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Edge triggered for listen socket too
    event.data.fd = server.listen_fd; // Use fd directly for listen socket
    
    if (epoll_ctl(server.epoll_fd, EPOLL_CTL_ADD, server.listen_fd, &event) == -1) {
        ERROR_EXIT("epoll_ctl ADD listen_fd");
    }

    // --- 5. Run the Main Event Loop ---
    printf("C-Redis server started on port %d\n", DEFAULT_PORT);
    server_run(&server);

    // Should never be reached
    handle_shutdown(0);
    return 0;
}

