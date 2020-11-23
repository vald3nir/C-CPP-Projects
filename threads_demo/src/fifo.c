#include "fifo.h"

#include <stdlib.h>

Queue* init_queue() {

	Queue *queue = (Queue*) malloc(sizeof(Queue));

	if (queue != NULL) {
		queue->size = 0;
		queue->head = NULL;
		queue->tail = NULL;
	}

	return queue;
}
Node create_node(double volt, double current, double real_power,
		double apparent_power, double power_factor) {

	Data data = { volt, current, real_power, apparent_power, power_factor };

	Node node;
	node.data = &data;
	node.next = NULL;

	return node;
}

void push(Queue *pQueue, Node *node) {

	if (pQueue != NULL && node != NULL) {

		node->next = NULL;

		if (pQueue->size == 0) {
			pQueue->head = node;
			pQueue->tail = node;
		} else {
			pQueue->tail->next = node;
			pQueue->tail = node;
		}

		pQueue->size++;
	}
}

Node* pop(Queue *pQueue) {

	Node *item;

	if (is_empty(pQueue))
		return NULL;

	item = pQueue->head;
	pQueue->head = (pQueue->head)->next;
	pQueue->size--;

	return item;
}

int is_empty(Queue *pQueue) {
	if (pQueue == NULL) {
		return 1;
	}
	return pQueue->size == 0 ? 1 : 0;
}
