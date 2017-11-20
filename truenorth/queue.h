/*
 * Address Queue
 *
 * To make general purpose queue, I make pointer queue to ensure coping all of data type.
 * you must keep in mind that the data which is enqueued or dequeued in the queue have to
 * malloced or freed by users.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

#define ADDRESS_SIZE    8

typedef struct node_ {
    void* element;
    struct node_* next;
} node;

typedef struct {
    node* head;
    node* tail;
    int max_size;
    int size;
} queue;

void queue_init (queue* myqueue, int size);
int isempty (queue* myqueue);
int enqueue (queue* myqueue, void* element);
void* dequeue (queue* myqueue);
int queue_flush (queue* myqueue);

#endif