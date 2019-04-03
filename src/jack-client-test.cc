#include <stdio.h>
#include <jack/jack.h>
#include <jack/control.h>

int main()
{
	const char *client_name = "jack-client-test";

	jack_status_t status;
	jack_client_t* client = jack_client_open(client_name, JackNoStartServer, &status, NULL);
	
	if (client == NULL)
	{
		fprintf(stderr, "jack_client_open() failed, status = 0x%2.0x\n", status);
		if (status & JackServerFailed)
		{
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit(1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	if (jack_activate (client)) {
	fprintf (stderr, "cannot activate client");
		exit (1);
	}

	jack_client_close (client);

	return 0;
}