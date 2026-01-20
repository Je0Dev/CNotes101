/* server.c - Main epoll loop for C-Redis */
#include "server.h"
#include "slab.h"
#include "thread_pool.h"
#include <fcntl.h>
#include <sys/socket.h>

/**
 * @brief Sets a file descriptor to non-blocking mode.
 */
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)"); return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)"); return -1;
    }
    return 0;
}

/**
 * @brief Creates and binds the main listening socket.
 */
int create_and_bind(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) ERROR_EXIT("socket");

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(listen_fd); ERROR_EXIT("bind");
    }
    if (set_nonblocking(listen_fd) == -1) {
        close(listen_fd); ERROR_EXIT("set_nonblocking listen_fd");
    }
    if (listen(listen_fd, SOMAXCONN) == -1) {
        close(listen_fd); ERROR_EXIT("listen");
    }
    return listen_fd;
}

/**
 * @brief Removes a client: closes socket, removes from epoll, frees struct.
 */
static void remove_client(server_t *s, int client_fd, client_t* client) {
    LOG("Removing client %d", client_fd);
    epoll_ctl(s->epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
    pthread_mutex_destroy(&client->lock);
    slab_free(s->client_slab, client); // Return struct to slab
}

/**
 * @brief Accepts new connections and adds them to epoll.
 */
static void accept_new_connection(server_t *s) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(s->listen_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return; // No more pending
            perror("accept"); return;
        }

        if (set_nonblocking(client_fd) == -1) {
            close(client_fd); continue;
        }

        // Allocate a client_t struct from the slab
        client_t *client = (client_t*)slab_alloc(s->client_slab);
        if (!client) {
            fprintf(stderr, "Slab allocator failed, dropping connection %d.\n", client_fd);
            close(client_fd);
            continue;
        }

        // Initialize client state
        client->fd = client_fd;
        client->state = STATE_READING;
        client->read_pos = 0;
        client->write_pos = 0;
        client->total_to_write = 0;
        pthread_mutex_init(&client->lock, NULL);

        // Add to epoll, watch for read and edge-triggered events
        // Niche C: EPOLLET (Edge Triggered) requires careful non-blocking code.
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.ptr = client; // Store pointer to client struct

        if (epoll_ctl(s->epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
            perror("epoll_ctl ADD client");
            slab_free(s->client_slab, client);
            close(client_fd);
        }
        LOG("Accepted client %d", client_fd);
    }
}

/**
 * @brief Handles readable event on a client socket.
 */
static void handle_client_read(server_t *s, client_t *client) {
    LOG("Handling read for fd %d", client->fd);
    bool connection_closed = false;
    
    while (true) {
        ssize_t bytes_read = read(client->fd,
                                  client->read_buffer + client->read_pos,
                                  READ_BUFFER_SIZE - client->read_pos);
        
        if (bytes_read == 0) {
            // Client closed connection
            connection_closed = true;
            break;
        } else if (bytes_read > 0) {
            client->read_pos += bytes_read;
            
            // --- Simple Protocol Parsing (find \r\n) ---
            char *newline = strstr(client->read_buffer, "\r\n");
            while (newline) {
                *newline = '\0'; // Null-terminate the command line
                
                // --- Dispatch to Thread Pool ---
                work_item_t* work = (work_item_t*)malloc(sizeof(work_item_t));
                work->client_fd = client->fd;
                strncpy(work->request, client->read_buffer, sizeof(work->request) - 1);
                work->request[sizeof(work->request) - 1] = '\0';
                work->server = s; // Pass server pointer
                
                client->state = STATE_PROCESSING; // Mark as busy
                thread_pool_add_work(s->pool, work);
                
                // Shift remaining data in buffer
                int cmd_len = (newline - client->read_buffer) + 2; // Include \r\n
                memmove(client->read_buffer, newline + 2, client->read_pos - cmd_len);
                client->read_pos -= cmd_len;
                
                // Look for next command in buffer
                newline = strstr(client->read_buffer, "\r\n");
            }

            // If buffer is full and we still haven't found \r\n, cmd too long
            if (client->read_pos == READ_BUFFER_SIZE && !strstr(client->read_buffer, "\r\n")) {
                // Simplified: Just close connection
                LOG("Command too long for fd %d", client->fd);
                connection_closed = true;
                break;
            }
            
            // If we read less than the buffer size, we're done reading for now
            if (bytes_read < READ_BUFFER_SIZE - client->read_pos) {
                break;
            }
            // Otherwise, loop again to try reading more (because of EPOLLET)

        } else { // bytes_read < 0
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data to read right now
                break;
            }
            perror("read client");
            connection_closed = true;
            break;
        }
    }

    if (connection_closed) {
        remove_client(s, client->fd, client);
    }
}


/**
 * @brief Handles writable event on a client socket.
 */
static void handle_client_write(server_t *s, client_t *client) {
    LOG("Handling write for fd %d", client->fd);
    bool connection_closed = false;
    
    pthread_mutex_lock(&client->lock);
    
    while (client->write_pos < client->total_to_write) {
        ssize_t bytes_sent = send(client->fd,
                                  client->write_buffer + client->write_pos,
                                  client->total_to_write - client->write_pos,
                                  MSG_NOSIGNAL); // Avoid SIGPIPE
                                  
        if (bytes_sent >= 0) {
            client->write_pos += bytes_sent;
        } else { // bytes_sent < 0
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Cannot write more now, wait for next EPOLLOUT
                break;
            }
            perror("send client");
            connection_closed = true;
            break;
        }
    }

    // If we finished writing everything
    if (!connection_closed && client->write_pos == client->total_to_write) {
        // We finished sending the response. Go back to reading state.
        client->state = STATE_READING;
        client->read_pos = 0; // Reset read buffer
        
        // Modify epoll interest back to EPOLLIN
        struct epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.ptr = client;
        epoll_ctl(s->epoll_fd, EPOLL_CTL_MOD, client->fd, &event);
        LOG("Finished writing to fd %d, switching back to read", client->fd);
    }
    
    pthread_mutex_unlock(&client->lock);

    if (connection_closed) {
        remove_client(s, client->fd, client);
    }
}

/**
 * @brief The main server event loop.
 */
void server_run(server_t *s) {
    struct epoll_event events[MAX_EVENTS];
    
    LOG("Server running. Waiting for events...");
    
    while (1) {
        int n = epoll_wait(s->epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) {
            if (errno == EINTR) continue; // Interrupted by signal, just retry
            ERROR_EXIT("epoll_wait");
        }
        
        for (int i = 0; i < n; i++) {
            client_t* client = (client_t*)events[i].data.ptr;
            int fd = (client == NULL) ? events[i].data.fd : client->fd; // Handle listen_fd vs client_fd

            if (fd == s->listen_fd) {
                accept_new_connection(s);
            } else {
                if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    remove_client(s, fd, client);
                    continue;
                }
                if (events[i].events & EPOLLIN) {
                    handle_client_read(s, client);
                }
                if (events[i].events & EPOLLOUT) {
                    handle_client_write(s, client);
                }
            }
        }
    }
}
