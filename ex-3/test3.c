#include <unistd.h>
#include <syscall.h>
#include <stdio.h>

struct int_pair {
	int n1;
	int n2;
};

int main(void){
	struct int_pair my_pair;
	my_pair.n1 = 2016;
	my_pair.n2 = 1577;
	
	int code = syscall(378, &my_pair);
	printf("syscall3 %d\n", code);

	return 0;
}
