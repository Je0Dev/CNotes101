#ifndef QUEUE_SIM_H
#define QUEUE_SIM_H

// Data for each customer
typedef struct {
    int arrival_time;
    int service_time; // Time they need with a teller
} Customer;

// Node for the queue's linked list
typedef struct Node {
    Customer data;
    struct Node *next;
} Node;

// The Queue itself
typedef struct {
    Node *front;
    Node *rear;
    int size;
} Queue;

// Prototypes
Queue* createQueue();
void enqueue(Queue *q, Customer data);
Customer dequeue(Queue *q);
int isQueueEmpty(Queue *q);
void freeQueue(Queue *q);

#endif
