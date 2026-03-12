#ifndef QUEUE_H
#define QUEUE_H

#include "../process/process.h"

#define MAX_QUEUE 100

typedef struct {

    Process* data[MAX_QUEUE];

    int front;
    int rear;
    int size;

} Queue;

void init_queue(Queue *q);

int is_empty(Queue *q);

void enqueue(Queue *q, Process *p);

Process* dequeue(Queue *q);

#endif