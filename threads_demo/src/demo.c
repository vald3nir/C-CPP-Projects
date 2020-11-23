/*
 ============================================================================
 Name        : demo.c
 Author      : Vald3nir
 Version     : 1.0.0
 Copyright   : Vald3nir 2019
 Description : Threads Producer and Consumer
 Libraries   : -lpthread
 Compile     : gcc  demo.c -lpthread -o demo.o
 ============================================================================
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fifo.h"

Queue *queue;

void* add_item(void *args) {

	int value = 0;
	while (1) {

		Data *data = malloc(sizeof(Data));
		data->value = value;

		Node *node = malloc(sizeof(Node));
		node->data = data;

		push(queue, node);

		printf("add new value -> %d\n", (int) node->data->value);
		value++;

		sleep(1);
	}
	return NULL;
}

void* remove_item(void *args) {

	while (1) {

		Node *node = pop(queue);

		if (node != NULL) {
			printf("remove value: %d\n", node->data->value);
			free(node);
		}

		sleep(1);
	}
	return NULL;
}

int main(void) {

	queue = init_queue();
	if (queue == NULL) {
		puts("GAME OVER");
		return EXIT_FAILURE;
	}

	pthread_t threads[2];

	pthread_create(&(threads[0]), NULL, &add_item, NULL);
	pthread_create(&(threads[1]), NULL, &remove_item, NULL);

	while (1) {
		puts("idle");
		sleep(1);
	}

	puts("FINISH");
	return EXIT_SUCCESS;
}
