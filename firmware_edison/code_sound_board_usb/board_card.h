#ifndef BOARD_CARD_H_
#define BOARD_CARD_H_

#include "../code_mqtt/comunication_mqtt.h"
#include "../code_data_structure/fifo.h"
#include "../code_leds/leds.h"

#define DEVISE_CARD_AUDIO "plughw:1,0"

#define CANAL 2
#define AMOSTRAS_PERIODO 800

#define POTENCIA_MINIMA 5

//double CONSTANTE_CORRENTE = 0.000288227625752632f;
//double CONSTANTE_TENSAO = 0.00005242999923704890f * 615.223734739f;

#define CONSTANTE_POTENCIA_REAL 		0.00000929712216081484;
#define CONSTANTE_TENSAO_INSTANTANEA  	0.00104046114451391000;
#define CONSTANTE_CORRENTE_INSTANTANEA  0.00000008307516424700;

struct amostra_ad {
	short corrente;
	short tensao;
};

int init_fifo();

// Threads
void* analise_dados(void *args);
void* aquisicao_dados(void *args);
void* enviar_dados_mqtt(void *args);
float get_potencia_calculada();

#endif
