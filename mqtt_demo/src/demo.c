/*
 ============================================================================
 Name        : demo.c
 Author      : Vald3nir
 Version     : 1.0.0
 Copyright   : Vald3nir 2019
 Description : Library MQTT Demo
 ============================================================================
 */

#include <stdio.h>

#include "mqtt_controller.h"

int main(int argc, char *argv[]) {
	if (mqtt_setup() > 0) {
		mqtt_publish_message("teste");
		puts("Publish Message Finish");
	} else {
		puts("Publish Message Error");
	}
}
