//
//  queue.h
//  
//
//  Created by Jinyu T. and Santhosh R.
//

#ifndef __QUEUE_H
#define __QUEUE_H

typedef struct Queue
{
    int capacity;
    int size;
    int front;
    int rear;
    char **elements;
} Queue;


Queue *createQueue(int capacity);
int getQueueSize(Queue *queue);
char *dequeue(Queue *queue);
void enqueue(Queue *queue, char **element);
int isQueueFull(Queue *queue);
int isQueueEmpty(Queue *queue);
void destroyQueue(Queue *queue);

#endif