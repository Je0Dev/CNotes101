/* lf_queue.h - Lock-Free Queue (Michael-Scott algorithm) */
#ifndef LF_QUEUE_H
#define LF_QUEUE_H

#include "common.h"

// --- Structures ---

// Node in the queue's linked list
typedef struct lf_node_t {
    void* value;
    _Atomic(struct lf_node_t*) next; // Niche C: Atomic pointer
} lf_node_t;

// The queue itself
typedef struct {
    // Niche C: Atomic pointers for head and tail
    _Atomic(lf_node_t*) head;
    _Atomic(lf_node_t*) tail;
} lf_queue_t;

// --- Public API ---
lf_queue_t* lf_queue_create();
void lf_queue_destroy(lf_queue_t* q); // Note: Assumes queue is empty
void lf_queue_push(lf_queue_t* q, void* value);
void* lf_queue_pop(lf_queue_t* q);

#endif // LF_QUEUE_H
