#ifndef FIFO_H_
#define FIFO_H_

#define TRUE 1
#define FALSE 0

typedef struct
{
	double tensao;
	double corrente;
	double potencia_real;
	double potencia_aparente;
	double fator_potencia;
} DATA;

typedef struct Node_t
{
	DATA data;
	struct Node_t *prev;
} NODE;

typedef struct Queue
{
	NODE *head;
	NODE *tail;
	int size;
} Queue;

Queue *ConstructQueue();

int push(Queue *pQueue, NODE *item);

NODE *pop(Queue *pQueue);

int isEmpty(Queue *pQueue);

#endif
