#ifndef FIFO_H_
#define FIFO_H_

typedef struct {
	int value;
	double volt;
	double current;
	double real_power;
	double apparent_power;
	double power_factor;
} Data;

typedef struct node {
	Data *data;
	struct node *next;
} Node;

Node create_node(double volt, double current, double real_power,
		double apparent_power, double power_factor);

typedef struct Queue {
	Node *head;
	Node *tail;
	int size;
} Queue;

Queue* init_queue();

void push(Queue *queue, Node *node);

Node* pop(Queue *queue);

int is_empty(Queue *queue);

#endif
