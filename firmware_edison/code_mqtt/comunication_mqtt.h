#ifndef COMUNICATION_MQTT_H_
#define COMUNICATION_MQTT_H_

// CONFIGURAÇÕES MQTT
// ======================================================================================

//#define MQTT_HOSTNAME "192.168.0.104"
#define MQTT_HOSTNAME   "192.168.25.217"

#define MQTT_USERNAME "root"
#define MQTT_PASSWORD "root"

#define TOPIC_MQTT_SEND_SAMPLES "edison-interceptor"
#define TOPIC_MQTT_CONFIG_RESPONSE "android-config-response"

#define MQTT_PORT 1883


// ======================================================================================

int init_service_mqtt();

int get_potencia_media_android();

void enviar_amostras_servidor(float, float, float, float, float);

void* receber_configuracao_android(void *args);

#endif
