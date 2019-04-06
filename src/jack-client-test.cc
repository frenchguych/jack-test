#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>

#include <jack/jack.h>
#include <jack/control.h>
#include <jack/transport.h>

typedef jack_default_audio_sample_t sample_t;

const double PI = 3.14;

jack_nframes_t wave_length;
jack_port_t* output_port;
sample_t* wave;
jack_client_t* client;

long offset = 0;

static void signal_handler(int sig)
{
    jack_client_close(client);
    fprintf(stderr, "signal received, exiting ...\n");
    exit(0);
}

static void
process_audio (jack_nframes_t nframes)
{
    sample_t *buffer = (sample_t *) jack_port_get_buffer (output_port, nframes);
    jack_nframes_t frames_left = nframes;

    while (wave_length - offset < frames_left) {
        memcpy (buffer + (nframes - frames_left), wave + offset, sizeof (sample_t) * (wave_length - offset));
        frames_left -= wave_length - offset;
        offset = 0;
    }
    if (frames_left > 0) {
        memcpy (buffer + (nframes - frames_left), wave + offset, sizeof (sample_t) * frames_left);
        offset += frames_left;
    }
}

static int
process (jack_nframes_t nframes, void *arg)
{
    process_audio (nframes);
    return 0;
}

int main()
{
	const char *client_name = "jack-client-test";

	jack_status_t status;
	client = jack_client_open(client_name, JackNoStartServer, &status, NULL);
	
	if (client == NULL)
	{
		fprintf(stderr, "jack_client_open() failed, status = 0x%2.0x\n", status);
		if (status & JackServerFailed)
		{
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit(1);
	}

	printf("Jack client successfully connected !\n");

	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}

	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	jack_set_process_callback(client, process, 0);
	output_port = jack_port_register(client, "jack-client-test-output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	/*
	jack_port_t* playback_1 = jack_port_by_name(client, "system:playback_1");
	fprintf(stderr, "%s\n", jack_port_name(playback_1));
	jack_port_t* playback_2 = jack_port_by_name(client, "system:playback_2");
	fprintf(stderr, "%s\n", jack_port_name(playback_2));
	*/
	jack_nframes_t sr = jack_get_sample_rate(client);
	
	int bpm = 60;
	int dur_arg = 100;
	int attack_percent = 1;
	int decay_percent = 10;
	int freq = 880;
	double max_amp = 0.5;

	wave_length = 60 * sr / bpm;
	jack_nframes_t tone_length = sr * dur_arg / 1000;
	jack_nframes_t attack_length = tone_length * attack_percent / 100;
	jack_nframes_t decay_length = tone_length * decay_percent / 100;
	sample_t scale = 2 * PI * freq / sr;
	wave = (sample_t*) malloc (wave_length * sizeof(sample_t));
	double* amp = (double*) malloc (tone_length * sizeof(double));
	for (jack_nframes_t i = 0; i < attack_length; i++) {
	    amp[i] = max_amp * i / ((double) attack_length);
    }
    for (jack_nframes_t i = attack_length; i < (int)tone_length - decay_length; i++) {
        amp[i] = max_amp;
    }
    for (jack_nframes_t i = (int)tone_length - decay_length; i < (int)tone_length; i++) {
        amp[i] = - max_amp * (i - (double) tone_length) / ((double) decay_length);
    }
    for (jack_nframes_t i = 0; i < (int)tone_length; i++) {
        wave[i] = amp[i] * sin (scale * i);
    }
    for (jack_nframes_t i = tone_length; i < (int)wave_length; i++) {
        wave[i] = 0;
    }

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	jack_connect(client, jack_port_name(output_port), "system:playback_1");
	jack_connect(client, jack_port_name(output_port), "system:playback_2");

    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);

    while (1)
    {
    	sleep(1);
    }

	jack_client_close (client);
	printf("Disconnected from server\n");

error:
    free(amp);
    free(wave);
    exit(0);

	return 0;
}
