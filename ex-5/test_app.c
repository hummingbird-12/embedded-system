#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int test_fd;
	char get_number[4];
	char sum;

	if(argc != 2) { 
		printf("Usage : [Number]\n");
		return -1;
	}

	test_fd = open("/dev/driver_test", O_RDWR);
	if (test_fd<0){

		printf("Open Failured!\n");
		return -1;
	}
	memcpy(get_number, argv[1], sizeof(get_number));

	write(test_fd, &get_number, sizeof(get_number));
	read(test_fd, &sum, 1);

	printf("Read data form kernel buffer: %d\n", sum);

	close(test_fd);

	return 0;
}

