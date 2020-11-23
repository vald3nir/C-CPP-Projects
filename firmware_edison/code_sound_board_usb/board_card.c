#include "board_card.h"

#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include <alsa/asoundlib.h>

float tensao = 0.0f;
float corrente = 0.0f;
float pot_real = 0.0f;
float pot_aparente = 0.0f;
float fator_pot = 0.0f;

Queue *fifo;

float get_potencia_calculada()
{
	return pot_real;
}

// constantes da casa
//double CONSTANTE_CORRENTE = 0.000255f;
//double CONSTANTE_TENSAO =   0.030100f;

// constantes da unifor
double CONSTANTE_CORRENTE = 0.000288227625752632f;
double CONSTANTE_TENSAO = 0.00005242999923704890f * 615.223734739f;

//double CONSTANTE_CORRENTE = 0.00027f;
//double CONSTANTE_TENSAO =   0.03100f;

// METODO DE INICIALIÇÃO DA ESTRUTURA DE ARMAZENAMENTO DAS AMOSTRAS
//==============================================================================================================================================

int init_fifo()
{

	printf("Start fifo to data acquisition\n");
	fifo = ConstructQueue();

	if (fifo == NULL)
	{
		printf("Start fifo faild\n");
		return -1;
	}

	printf("Start fifo ok\n");
	return 0;
}

//==============================================================================================================================================

int flag_send_samples_server_mqtt = 0;

void *enviar_dados_mqtt(void *args)
{

	for (;;)
	{

		if (flag_send_samples_server_mqtt == 1)
		{
			enviar_amostras_servidor(tensao, corrente, pot_real, pot_aparente,
									 fator_pot);
			flag_send_samples_server_mqtt = 0;
		}

		usleep(10000);
	}

	return NULL;
}

//==============================================================================================================================================

int potencia_referencia = 0;
int tensao_referencia = 0;
int corrente_referencia = 0;

void *analise_dados(void *args)
{

	for (;;)
	{

		if (!isEmpty(fifo))
		{

			NODE *no = pop(fifo);

			int p = (int)no->data.potencia_real;
			int v = (int)no->data.tensao;
			int c = (int)(no->data.corrente * 100);

			if (p != potencia_referencia || v != tensao_referencia || c != corrente_referencia)
			{

				potencia_referencia = p;
				tensao_referencia = v;
				corrente_referencia = c;

				pot_aparente = no->data.potencia_aparente;
				fator_pot = no->data.fator_potencia;
				pot_real = no->data.potencia_real;
				tensao = no->data.tensao;
				corrente = no->data.corrente;

				//printf("%fV %fA %fW %fVA %f\n", tensao, corrente, pot_real, pot_aparente, fator_pot);

				flag_send_samples_server_mqtt = 1;

				free(no);
			}

			usleep(10000);
		}
	}

	return NULL;
}

// THREAD DE AQUISIÇÃO DE DADOS
//==============================================================================================================================================

void *aquisicao_dados(void *args)
{

	snd_pcm_sframes_t amostras;
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;

	int err;
	unsigned int sample_rate = 48000;

	if ((err = snd_pcm_open(&capture_handle, DEVISE_CARD_AUDIO,
							SND_PCM_STREAM_CAPTURE, 0)) < 0)
	{
		fprintf(stderr, "cannot open audio device %s (%s)\n",
				DEVISE_CARD_AUDIO, snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0)
	{
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params,
											SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params,
											SND_PCM_FORMAT_S16_LE)) < 0)
	{
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params,
											   &sample_rate, 0)) < 0)
	{
		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params,
											  CANAL)) < 0)
	{
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0)
	{
		fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
		exit(1);
	}

	snd_pcm_hw_params_free(hw_params);

	if ((err = snd_pcm_prepare(capture_handle)) < 0)
	{
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_start(capture_handle)) < 0)
	{
		exit(1);
	}

	for (;;)
	{

		struct amostra_ad *buffer;

		buffer = (struct amostra_ad *)malloc((AMOSTRAS_PERIODO * sizeof(struct amostra_ad)));

		do
		{

			amostras = snd_pcm_avail_update(capture_handle);

			if (amostras < AMOSTRAS_PERIODO)
			{
				usleep(1000);
			}

		} while (amostras < AMOSTRAS_PERIODO);

		if ((err = snd_pcm_readi(capture_handle, buffer, AMOSTRAS_PERIODO)) < AMOSTRAS_PERIODO)
		{
			fprintf(stderr, "read from audio interface failed (%s)\n", snd_strerror(err));
		}
		else
		{

			double somador_potencia_real = 0.0;
			double somador_tensao_instatanea = 0.0;
			double somador_corrente_instatanea = 0.0;

			int i;
			for (i = 0; i < AMOSTRAS_PERIODO; i++)
			{

				somador_potencia_real += buffer[i].tensao * buffer[i].corrente * CONSTANTE_TENSAO * CONSTANTE_CORRENTE;
				somador_tensao_instatanea += buffer[i].tensao * buffer[i].tensao * CONSTANTE_TENSAO * CONSTANTE_TENSAO;
				somador_corrente_instatanea += buffer[i].corrente * buffer[i].corrente * CONSTANTE_CORRENTE * CONSTANTE_CORRENTE;
			}

			double tensao_rms = sqrt(
				somador_tensao_instatanea / AMOSTRAS_PERIODO);
			double corrente_rms = sqrt(
				somador_corrente_instatanea / AMOSTRAS_PERIODO);
			double potencia_real = (somador_potencia_real / AMOSTRAS_PERIODO);
			double potencia_aparente = tensao_rms * corrente_rms;
			double fator_potencia = potencia_real / potencia_aparente;

			NODE *no = (NODE *)malloc(sizeof(NODE));

			no->data.corrente = corrente_rms;
			no->data.tensao = tensao_rms;
			no->data.potencia_real = potencia_real;
			no->data.potencia_aparente = potencia_aparente;
			no->data.fator_potencia = fator_potencia;

			//printf("%fV %fA %fW %fVA %f\n", tensao_rms, corrente_rms, potencia_real, potencia_aparente, fator_potencia);

			push(fifo, no);
			free(buffer);
		}
	}

	snd_pcm_close(capture_handle);
	return NULL;
}
