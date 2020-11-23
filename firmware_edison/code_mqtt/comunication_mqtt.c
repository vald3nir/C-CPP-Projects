#include "comunication_mqtt.h"

#include <mosquitto.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct mosquitto *mosquitto_handle = NULL;

int static potencia_android = 100;

int get_potencia_media_android()
{
	return potencia_android;
}

int init_service_mqtt()
{

	printf("Starting configuration of the Server MQTT\n");

	mosquitto_lib_init();

	mosquitto_handle = mosquitto_new(NULL, true, NULL);

	if (!mosquitto_handle)
	{
		fprintf(stderr, "Can't initialize Mosquitto library\n");
		return -1;
	}

	mosquitto_username_pw_set(mosquitto_handle, MQTT_USERNAME, MQTT_PASSWORD);

	if (mosquitto_connect(mosquitto_handle, MQTT_HOSTNAME, MQTT_PORT, 0))
	{
		fprintf(stderr, "Can't connect to Mosquitto server\n");
		return -1;
	}

	printf("Setting the server mqtt Ok\n");

	return 0;
}

void enviar_amostras_servidor(float tensao, float corrente, float potencia_real,
							  float potencia_aparente, float fator_potencia)
{

	char mensagem_mqtt[200];

	char string_tensao[32];
	char string_potencia_real[32];
	char string_potencia_aparente[32];
	char string_fator_potencia[32];
	char string_corrente[32];
	char separador[] = "|";
	char data_system[64];

	time_t now = time(NULL);

	strftime(data_system, sizeof(data_system), "%Y-%m-%d %H:%M:%S",
			 localtime(&now));
	snprintf(string_tensao, sizeof(string_tensao), "%g", tensao);
	snprintf(string_corrente, sizeof(string_corrente), "%g", corrente);
	snprintf(string_potencia_real, sizeof(string_potencia_real), "%g",
			 potencia_real);
	snprintf(string_potencia_aparente, sizeof(string_potencia_aparente), "%g",
			 potencia_aparente);
	snprintf(string_fator_potencia, sizeof(string_fator_potencia), "%g",
			 fator_potencia);

	strcpy(mensagem_mqtt, data_system);
	strcat(mensagem_mqtt, separador);

	strcat(mensagem_mqtt, string_tensao);
	strcat(mensagem_mqtt, separador);

	strcat(mensagem_mqtt, string_corrente);
	strcat(mensagem_mqtt, separador);

	strcat(mensagem_mqtt, string_potencia_real);
	strcat(mensagem_mqtt, separador);

	strcat(mensagem_mqtt, string_potencia_aparente);
	strcat(mensagem_mqtt, separador);

	strcat(mensagem_mqtt, string_fator_potencia);

	if (mosquitto_publish(mosquitto_handle, NULL, TOPIC_MQTT_SEND_SAMPLES, strlen(mensagem_mqtt), mensagem_mqtt, 0, false))
	{
		printf("Error publishing to Mosquitto Server!\n");
		return;
	}

	printf("Edison enviou -> %fV %fA %fW %fVA %f\n", tensao, corrente, potencia_real, potencia_aparente, fator_potencia);

	mosquitto_lib_cleanup();
}

void message_callback(struct mosquitto *mosq, void *obj,
					  const struct mosquitto_message *message)
{
	potencia_android = atoi((char *)message->payload);
	mosquitto_topic_matches_sub(TOPIC_MQTT_CONFIG_RESPONSE, message->topic,
								NULL);
}

void *receber_configuracao_android(void *args)
{

	mosquitto_message_callback_set(mosquitto_handle, message_callback);
	mosquitto_subscribe(mosquitto_handle, NULL, TOPIC_MQTT_CONFIG_RESPONSE, 0);

	for (;;)
	{

		if (mosquitto_loop(mosquitto_handle, -1, 1))
		{
			mosquitto_reconnect(mosquitto_handle);
		}

		usleep(10000);
	}

	return NULL;
}
