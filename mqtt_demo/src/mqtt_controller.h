#ifndef MQTT_CONTROLLER
#define MQTT_CONTROLLER

#define BROKER_ADDRESS "localhost"

#define BROKER_PORT 1883
#define KEEP_ALIVE  60

#define TOPIC "MOACI"

int mqtt_publish_message(char *msg);

int mqtt_setup();

#endif
