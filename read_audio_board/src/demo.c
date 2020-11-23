/*
 ============================================================================
 Name        : demo.c
 Author      : Vald3nir
 Version     : 1.0.0
 Copyright   : Vald3nir 2019
 Description : Library Audio Test
 Libraries   : -lpthread -lmosquitto -lasound
 Commands    : sudo apt-get install build-essential libmosquitto-dev mosquitto mosquitto-clients libasound2-dev
 : apla -l // Show audio cards
 : arecord -f S16_LE -c 2 -r 48000 -d 1 amostra.wav
 Compile     : gcc  demo.c -lasound -o demo.o

 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#define DEVISE_CARD_AUDIO "plughw:1,0"
#define FILE_NAME "teste.csv"

#define CANAL 1
#define AMOSTRAS_PERIODO 800*4

struct sample {
	short current;
//	short volt;
};

int main(void) {

	snd_pcm_sframes_t amostras;
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;

	int err, i = 0;
	unsigned int sample_rate = 48000;

	puts("Start....");

	if ((err = snd_pcm_open(&capture_handle, DEVISE_CARD_AUDIO,
			SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf(stderr, "cannot open audio device %s (%s)\n", DEVISE_CARD_AUDIO,
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params,
			SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params,
			SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params,
			&sample_rate, 0)) < 0) {
		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, CANAL))
			< 0) {
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
		exit(1);
	}

	snd_pcm_hw_params_free(hw_params);

	if ((err = snd_pcm_prepare(capture_handle)) < 0) {
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_start(capture_handle)) < 0) {
		exit(1);
	}

	struct sample *buffer;

//	while (1) {

	buffer = (struct sample *) malloc(
			(AMOSTRAS_PERIODO * sizeof(struct sample)));

	do {

		amostras = snd_pcm_avail_update(capture_handle);

		if (amostras < AMOSTRAS_PERIODO) {
			usleep(1000);
		}

	} while (amostras < AMOSTRAS_PERIODO);

	if ((err = snd_pcm_readi(capture_handle, buffer, AMOSTRAS_PERIODO))
			< AMOSTRAS_PERIODO) {
		fprintf(stderr, "read from audio interface failed (%s)\n",
				snd_strerror(err));
	}

	else {

		FILE *f = fopen(FILE_NAME, "w");
		if (f == NULL) {
			printf("Error opening file!\n");
			exit(1);
		}

		for (i = 0; i < AMOSTRAS_PERIODO; i++) {
			fprintf(f, "%d\n", buffer[i].current);
		}

		fclose(f);
	}

	free(buffer);

//	usleep(1000);
//	}

	snd_pcm_close(capture_handle);
	puts("FIM");
	return EXIT_SUCCESS;
}
