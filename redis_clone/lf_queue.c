/* lf_queue.c - Implementation of the Michael-Scott Lock-Free Queue */
#include "lf_queue.h"

lf_queue_t* lf_queue_create() {
    lf_queue_t* q = (lf_queue_t*)malloc(sizeof(lf_queue_t));
    if (!q) ERROR_EXIT("malloc lf_queue_t");
    
    // Allocate a dummy node
    lf_node_t* dummy_node = (lf_node_t*)malloc(sizeof(lf_node_t));
    if (!dummy_node) ERROR_EXIT("malloc dummy_node");
    
    dummy_node->value = NULL;
    atomic_init(&dummy_node->next, NULL); // Initialize atomic pointer
    
    // Initialize head and tail to point to the dummy node
    atomic_init(&q->head, dummy_node);
    atomic_init(&q->tail, dummy_node);
    
    return q;
}

void lf_queue_destroy(lf_queue_t* q) {
    // IMPORTANT: This assumes the queue is empty and quiescent.
    // A production queue needs safe memory reclamation (e.g., Hazard Pointers).
    lf_node_t* head = atomic_load_explicit(&q->head, memory_order_relaxed);
    free(head); // Free the dummy node
    free(q);
}

void lf_queue_push(lf_queue_t* q, void* value) {
    // 1. Create the new node
    lf_node_t* new_node = (lf_node_t*)malloc(sizeof(lf_node_t));
    if (!new_node) ERROR_EXIT("malloc new_node for push");
    new_node->value = value;
    atomic_init(&new_node->next, NULL);

    lf_node_t* tail;
    lf_node_t* next;

    while (true) {
        // 2. Read current tail and the node after it
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        // 3. Check if tail is still the actual tail
        if (tail == atomic_load_explicit(&q->tail, memory_order_relaxed)) {
            if (next == NULL) {
                // 4. Try to link the new node after the current tail
                // Niche C: The core atomic operation!
                if (atomic_compare_exchange_weak_explicit(&tail->next, &next, new_node,
                                                        memory_order_release, memory_order_relaxed)) {
                    // Success! Break the loop.
                    break;
                }
            } else {
                // 5. Tail pointer is lagging. Try to swing tail to the next node.
                atomic_compare_exchange_strong_explicit(&q->tail, &tail, next,
                                                        memory_order_release, memory_order_relaxed);
                // We don't break; loop again to ensure our node is added after the *new* tail.
            }
        }
    }
    // 6. Push succeeded. Try to swing tail to our newly added node.
    // (This helps other pushers; doesn't affect correctness if it fails).
    atomic_compare_exchange_strong_explicit(&q->tail, &tail, new_node,
                                            memory_order_release, memory_order_relaxed);
}


void* lf_queue_pop(lf_queue_t* q) {
    lf_node_t* head;
    lf_node_t* tail;
    lf_node_t* next;
    void* value;

    while (true) {
        // 1. Read head, tail, and the node after head
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);

        // 2. Check consistency: is head still the head?
        if (head == atomic_load_explicit(&q->head, memory_order_relaxed)) {
            if (head == tail) {
                // 3. Queue is empty or tail is lagging
                if (next == NULL) {
                    return NULL; // Queue is empty
                }
                // Tail is lagging, try to advance it
                atomic_compare_exchange_strong_explicit(&q->tail, &tail, next,
                                                        memory_order_release, memory_order_relaxed);
            } else {
                // 4. Queue is not empty. Try to move head forward.
                value = next->value; // Get value from the *next* node
                // Niche C: The core atomic operation!
                if (atomic_compare_exchange_weak_explicit(&q->head, &head, next,
                                                        memory_order_release, memory_order_relaxed)) {
                    // Success! We popped 'next'. Break loop.
                    break;
                }
            }
        }
    }
    // 5. We successfully CAS'd the head pointer.
    // The old head ('head') is now detached (it's the dummy node or a previously popped node).
    // We MUST free it, but ONLY when we are SURE no other thread is still accessing it.
    // This requires a safe memory reclamation scheme (like Hazard Pointers or epoch-based).
    // For this example, we will LEAK the old head node.
    // free(head); // <--- THIS IS UNSAFE without a reclamation scheme!
    
    return value;
}
