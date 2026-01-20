#include <stdio.h>
#include <stdlib.h>
#include "queue_sim.h"

Queue* createQueue() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

void enqueue(Queue *q, Customer data) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    
    if (isQueueEmpty(q)) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->size++;
}

Customer dequeue(Queue *q) {
    if (isQueueEmpty(q)) {
        fprintf(stderr, "Error: Dequeue from empty queue.\n");
        Customer empty = {-1, -1};
        return empty;
    }
    
    Node *temp = q->front;
    Customer data = temp->data;
    
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    q->size--;
    return data;
}

int isQueueEmpty(Queue *q) {
    return (q->size == 0);
}

void freeQueue(Queue *q) {
    while (!isQueueEmpty(q)) {
        dequeue(q);
    }
    free(q);
}
