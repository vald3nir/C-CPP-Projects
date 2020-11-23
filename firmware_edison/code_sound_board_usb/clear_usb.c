#include "clear_usb.h"

#include <fcntl.h>
#include <linux/usbdevice_fs.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int clear_devices_usb() {

	const char *filename;

	int fd;
	int rc;

	filename = "/dev/bus/usb/001/002";

	printf("Clean Devices USB %s\n", filename);

	fd = open(filename, O_WRONLY);

	if (fd < 0) {
		perror("Error opening output file");
		return -1;
	}

	printf("Resetting USB device %s\n", filename);
	rc = ioctl(fd, USBDEVFS_RESET, 0);

	if (rc < 0) {
		printf("Clean Devices USB Failed\n");
		return -1;
	}

	printf("Reset successful\n");
	close(fd);

	return 0;
}
