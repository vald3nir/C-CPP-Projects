#include "fifo.h"

#include <stdlib.h>

Queue *ConstructQueue()
{

	Queue *queue = (Queue *)malloc(sizeof(Queue));

	if (queue == NULL)
	{
		return NULL;
	}

	queue->size = 0;
	queue->head = NULL;
	queue->tail = NULL;

	return queue;
}

int push(Queue *pQueue, NODE *item)
{

	if ((pQueue == NULL) || (item == NULL))
	{
		return FALSE;
	}

	item->prev = NULL;

	if (pQueue->size == 0)
	{
		pQueue->head = item;
		pQueue->tail = item;
	}
	else
	{
		pQueue->tail->prev = item;
		pQueue->tail = item;
	}

	pQueue->size++;

	return TRUE;
}

NODE *pop(Queue *pQueue)
{

	NODE *item;

	if (isEmpty(pQueue))
		return NULL;

	item = pQueue->head;
	pQueue->head = (pQueue->head)->prev;
	pQueue->size--;

	return item;
}

int isEmpty(Queue *pQueue)
{

	if (pQueue == NULL)
	{
		return FALSE;
	}

	return pQueue->size == 0 ? TRUE : FALSE;
}
