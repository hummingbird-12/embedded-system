#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int led_fd;
	char get_number;

	if(argc != 2) { 
		printf("Usage : [Number]\n");
		return -1;
	}

	led_fd = open("/dev/kernel_timer", O_WRONLY);
	if (led_fd<0){

		printf("Open Failured!\n");
		return -1;
	}
	get_number=atoi(argv[1]);
	printf("get %d\n",get_number);

	write(led_fd, &get_number, sizeof(get_number) );
	close(led_fd);

	return 0;
}

