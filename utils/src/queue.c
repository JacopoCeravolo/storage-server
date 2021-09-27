#include "queue.h"

queue_t *createQueue(size_t allocSize)
{
	queue_t *q = (queue_t *)malloc(sizeof(queue_t));
	if (q == NULL) {
		return NULL;
	}

	q->allocationSize = allocSize;
	q->size = 0;
	q->head = q->tail = NULL;
	return q;
}

int enqueue(queue_t *q, void *data)
{
	if (q == NULL) {
		fprintf(stderr, "Queue can't be null");
		exit(EXIT_FAILURE);
	}

	node_t *toInsert = (node_t *)malloc(sizeof(node_t));
	if (toInsert == NULL) {
		fprintf(stderr, "Error allocating memory");
		exit(EXIT_FAILURE);
	}
	
	toInsert->data = malloc(q->allocationSize);
	if (toInsert->data == NULL) {
		fprintf(stderr, "Error allocating memory");
		exit(EXIT_FAILURE);
	}

	toInsert->next = NULL;
	memcpy(toInsert->data, data, q->allocationSize);
	if (q->size == 0) { // First insertion
		q->head = q->tail = toInsert;
	} else {
		q->tail->next = toInsert;
		q->tail = toInsert;
	}
	q->size++;
	return 0;
}

int dequeue(queue_t *q, void *toReturn)
{
	if (q == NULL || isEmpty(q)) {
		fprintf(stderr, "Queue is null or empty");
		exit(EXIT_FAILURE);
	}

	node_t *toDelete = q->head;
	if (q->size == 1) {
		memcpy(toReturn, toDelete->data, q->allocationSize);
		free(toDelete->data);
		free(toDelete);
		q->head = q->tail = NULL;
		q->size--;
		return 0;
	}

	q->head = q->head->next;

	memcpy(toReturn, toDelete->data, q->allocationSize);
	free(toDelete->data);
	free(toDelete);
	q->size--;
	return 0;
}

int clearQueue(queue_t *q)
{
	if (q == NULL) {
		fprintf(stderr, "Queue can't be null");
		exit(EXIT_FAILURE);
	}

	while (!isEmpty(q)) {
		node_t *tmp = q->head;
		q->head = q->head->next;
		free(tmp->data);
		free(tmp);
		q->size--;
	}
	return 0;
}

bool isEmpty(queue_t *q)
{
	if (q == NULL) {
		return NULL;
	}
	if (q->size == 0) {
		return true;
	}
	else {
		return false;
	}
}

int destroyQueue(queue_t *q)
{
	clearQueue(q);
	free(q);
	return 0;
}
