#include <stdio.h>
#include <jack/jack.h>
#include <jack/control.h>

bool on_device_acquire(const char* device_name)
{
	printf("Acquiring device %s\n", device_name);
	return true;
}

void on_device_release(const char *device_name)
{
	printf("Releasing device %s\n", device_name);
}

int main()
{
	jackctl_server_t* server = jackctl_server_create(on_device_acquire, on_device_release);
	printf("Jack server created\n");
	jackctl_server_destroy(server);
	printf("Jack server destroyed\n");
	return 0;
}
