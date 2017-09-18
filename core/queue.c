#include "queue.h"

void queue_init (queue* myqueue, int size) {

    myqueue->head = NULL;
    myqueue->tail = NULL;
    myqueue->max_size = size;
    myqueue->size = 0;

    return;
}

int isempty (queue* myqueue) {
    if (myqueue->size == 0)
        return 1;
    return 0;
}

int enqueue (queue* myqueue, void* element) {

    if (myqueue->size == myqueue->max_size)
        return -1;
    myqueue->size++;
    node* ptr = (node*)malloc(sizeof(node));
    ptr->element = element;
    if (myqueue->head == NULL && myqueue->tail == NULL) {
        myqueue->head = ptr;
        myqueue->tail = ptr;
    }
    else {
        myqueue->tail->next = ptr;
        myqueue->tail = ptr;
    }
    return 0;
}

void* dequeue (queue* myqueue) {

    void* element;
    void* oldhead;

    if (myqueue->size == 0)
        return NULL;
    myqueue->size--;
    element = myqueue->head->element;
    oldhead = myqueue->head;
    if (myqueue->size == 0) {
        myqueue->head = NULL;
        myqueue->tail = NULL;
    }
    else
        myqueue->head = myqueue->head->next;
    free (oldhead);
    return element;
}