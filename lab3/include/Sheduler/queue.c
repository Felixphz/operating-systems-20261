#include <stddef.h>
#include <stdio.h>
#include "Sheduler/queue.h"

void init_queue(Queue *q)
{   /* posición del primer elemento*/
    q->front = 0;
    /* posición donde se insertará el siguiente elemento */
    q->rear = 0;
    /* cantidad de elementos actuales*/
    q->size = 0;
}

int is_empty(Queue *q)
{
    return q->size == 0;
}

void enqueue(Queue *q, Process *p)
{
    if(q->size == MAX_QUEUE)
    {
        fprintf(stderr, "Warning: Cola llena, no se puede agregar proceso\n");
        return;
    }
    
    if(p == NULL) return;  /* No agregar punteros nulos */

    q->data[q->rear] = p;
    q->rear = (q->rear + 1) % MAX_QUEUE;

    q->size++;
}

Process* dequeue(Queue *q)
{
    if(is_empty(q)) return NULL;

    Process *p = q->data[q->front];

    q->front = (q->front + 1) % MAX_QUEUE;

    q->size--;

    return p;
}