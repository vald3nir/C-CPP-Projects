#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "code_leds/leds.h"
#include "code_mqtt/comunication_mqtt.h"
#include "code_sound_board_usb/board_card.h"
#include "code_sound_board_usb/clear_usb.h"

/*
 Bibliotecas para copilar o projeto:
 mosquitto
 pthread
 asound
 m
 */

int main()
{

	if (clear_devices_usb() < 0)
	{
		return 0;
	}

	if (init_service_mqtt() < 0)
	{
		return 0;
	}

	if (init_fifo() < 0)
	{
		return 0;
	}

	if (init_leds() < 0)
	{
		return 0;
	}

	pthread_t listas_threads[4];
	pthread_create(&(listas_threads[0]), NULL, &aquisicao_dados, NULL);
	pthread_create(&(listas_threads[1]), NULL, &analise_dados, NULL);
	pthread_create(&(listas_threads[2]), NULL, &enviar_dados_mqtt, NULL);
	pthread_create(&(listas_threads[3]), NULL, &receber_configuracao_android, NULL);

	for (;;)
	{
		float p = get_potencia_calculada();

		if (p <= POTENCIA_MINIMA)
		{

			set_led_green();
			usleep(1500000);
			set_leds_off();
			usleep(1500000);
		}
		else
		{

			if (p > POTENCIA_MINIMA && p < get_potencia_media_android())
			{
				set_led_yellow();
				usleep(500000);
				set_leds_off();
				usleep(500000);
			}

			else
			{
				set_led_red();
				usleep(100000);
				set_leds_off();
				usleep(100000);
			}
		}
	}

	return 0;
}
