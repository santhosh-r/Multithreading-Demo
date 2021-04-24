//
//  queue.c
//  
//
//  Created by Jinyu T. and Santhosh R.
//

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getQueueSize(Queue *queue) {
    return queue->size;
}

Queue *createQueue(int capacity) {
    // allocate memory for Queue structure
    Queue *newQueue = (Queue *)malloc(sizeof(Queue));
    
    // set front and size to 0
    newQueue->front = newQueue->size = 0;
    // assign -1 to rear so that enqueuing starts at index 0
    newQueue->rear = -1;
    // assign capacity to new Queue's capacity attribute
    newQueue->capacity = capacity;
    // allocate memory for elements array
    newQueue->elements = (char **)malloc(capacity*sizeof(char *));
    return newQueue;
}

char *dequeue(Queue *queue) {
    int dequeued = queue->front;
    
    // ensure an already empty queue is not dequeued
    if (queue->size == 0) {
        perror("Queue already empty");
        return NULL;
    }
    // ensure queue front circles around
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    // return dequeued element (needs to be freed by calling function)
    return queue->elements[dequeued];
}

void enqueue(Queue *queue, char **element) {
    // ensure a full queue is not enqueued
    if (queue->size == queue->capacity) {
        perror("Queue full");
        return;
    }
    // ensure queue rear circles around
    queue->rear = (queue->rear + 1) % queue->capacity;
    // allocate memory for and copy contents of given element
    queue->elements[queue->rear] = (char *)malloc((strlen(*element)+1)*sizeof(char));
    strcpy(queue->elements[queue->rear], *element);
    queue->size++;
}

int isQueueFull(Queue *queue) {
    return !queue || (queue->size == queue->capacity);
}

int isQueueEmpty(Queue *queue) {
    return !queue || (queue->size == 0);
}

void destroyQueue(Queue *queue) {
    // avoid any operation of a null pointer
    if (!queue)
        return;
    // dequeue and free space allocated for all elements
    while (queue->size > 0)
        free(dequeue(queue));
    // free space allocated for array of pointers
    free(queue->elements);
    // free space allocated for queue structure
    free(queue);
}