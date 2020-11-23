/*
 ============================================================================
 Name        : main.c
 Author      : Vald3nir
 Version     : 2.0
 Description : Firmware of Project MOACI
 ============================================================================
 Libraries   : -lpthread -lmosquitto -lasound -lm
 Linux       : sudo apt-get install build-essential libmosquitto-dev mosquitto mosquitto-clients libasound2-dev python-pyaudio python3-pyaudio python-dev python3-dev python-pyaudio portaudio19-dev python-pip python3-pip python3-setuptools python-setuptools 
 Compile     : gcc  main.c -lasound -lm -o main.o && ./main.o
*/

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysexits.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define DEVISE_CARD_AUDIO "plughw:1,0"
#define FILE_NAME "signal.csv"
#define RUN_SCRIPT "python3 plot_signal.py"
#define CANAL 1
#define NUMBER_SAMPLES 800 * 3

struct sample
{
	int current;
	//	short volt;
};

int main(void)
{

	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;

	int err;
	unsigned int sample_rate = 48000;

	puts("Start....");

	if ((err = snd_pcm_open(&capture_handle, DEVISE_CARD_AUDIO, SND_PCM_STREAM_CAPTURE, 0)) < 0)
	{
		fprintf(stderr, "cannot open audio device %s (%s)\n", DEVISE_CARD_AUDIO, snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0)
	{
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
	{
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &sample_rate, 0)) < 0)
	{
		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, CANAL)) < 0)
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
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_start(capture_handle)) < 0)
	{
		exit(1);
	}

	struct sample *buffer;
	buffer = (struct sample *)malloc((NUMBER_SAMPLES * sizeof(struct sample)));

	struct timeval t0;
	struct timeval t1;
	float elapsed;
	gettimeofday(&t0, 0);

	//sleep(5);
	snd_pcm_sframes_t sframes;
	do
	{
		sframes = snd_pcm_avail_update(capture_handle);
		if (sframes < NUMBER_SAMPLES)
		{
			usleep(1000);
		}

	} while (sframes < NUMBER_SAMPLES);

	if ((err = snd_pcm_readi(capture_handle, buffer, NUMBER_SAMPLES)) < NUMBER_SAMPLES)
	{
		fprintf(stderr, "read from audio interface failed (%s)\n", snd_strerror(err));
	}

	else
	{
		FILE *f = fopen(FILE_NAME, "w");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}

		int i;
		for (i = 0; i < NUMBER_SAMPLES; i++)
		{
			fprintf(f, "%d\n", buffer[i].current);
		}
		fclose(f);
	}

	free(buffer);
	snd_pcm_close(capture_handle);

	system(RUN_SCRIPT);
	puts("FIM");

	return EXIT_SUCCESS;
}
