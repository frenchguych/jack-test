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

	const JSList* driver = jackctl_server_get_drivers_list(server);
	int driver_count = 0;
	while (driver)
	{
		driver_count += 1;
		const char* driver_name = jackctl_driver_get_name((jackctl_driver_t*)driver->data);
		printf("Driver #%d: %s\n", driver_count, driver_name);
		driver = jack_slist_next(driver);
	}
	printf("Found %d drivers\n", driver_count);

	jackctl_server_destroy(server);
	printf("Jack server destroyed\n");

	return 0;
}
