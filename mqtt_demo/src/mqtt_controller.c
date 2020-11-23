#include "mqtt_controller.h"

#include <mosquitto.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mosquitto *mosq = NULL;

int mqtt_publish_message(char *msg) {
	return mosquitto_publish(mosq, NULL, TOPIC, strlen(msg), msg, 0, 0);
}

int mqtt_setup() {

	bool clean_session = true;

	mosquitto_lib_init();

	mosq = mosquitto_new(NULL, clean_session, NULL);

	if (!mosq) {
		fprintf(stderr, "Error: Out of memory.\n");
		return -1;
	}

	if (mosquitto_connect(mosq, BROKER_ADDRESS, BROKER_PORT, KEEP_ALIVE)) {
		fprintf(stderr, "Unable to connect.\n");
		return -1;
	}

	int loop = mosquitto_loop_start(mosq);

	if (loop != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "Unable to start loop: %i\n", loop);
		return -1;
	}

	return 1;
}
