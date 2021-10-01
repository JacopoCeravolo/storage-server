#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct _node_t {
	int		idx;
	void*	data;			// generic item of queue
	struct _node_t *next;		
} node_t;

typedef struct _queue_t {
	size_t 		size; 			// size of queu
	size_t 		allocationSize;	// allocation size of node
	node_t* 	head;			// head of queue
	node_t* 	tail;			// tail of queue
} queue_t;

/* Create and return an empty queue */
queue_t* createQueue(size_t allocSize);

/* Insert data into the queue (last position) */
int enqueue(queue_t *q, void* data);

/* Remove first element of the queue of save its value to the toRet argument */
int dequeue(queue_t *q, void * toRet);

/* Deletes all data of the queue */
int clearQueue(queue_t *q);

/* Clears and destoys the queue */
int destroyQueue(queue_t *q);

/* Check is queue is empty */
bool isEmpty(queue_t *q);

#endif
