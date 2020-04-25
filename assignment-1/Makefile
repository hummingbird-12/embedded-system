# hw1.out: main.o ipc.o input.o output.o
# 	arm-none-linux-gnueabi-gcc -static -Wall -lm -o hw1.out main.o ipc.o input.o output.o
# 	@echo ">>> To execute, type ./hw1.out"

# main.o: core.h main.h main.c
# 	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o main.o main.c

# ipc.o: core.h ipc.h ipc.c
# 	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o ipc.o ipc.c

# input.o: core.h input.h input.c
# 	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o input.o input.c

# output.o: core.h output.h output.c
# 	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o output.o output.c

hw1.host.out: main.host.o ipc.host.o input.host.o output.host.o
	gcc -Wall -lm -o hw1.host.out main.host.o ipc.host.o input.host.o output.host.o
	@echo ">>> To execute, type ./hw1.host.out"

main.host.o: core.h main.h main.c
	gcc -Wall -lm -c -o main.host.o main.c

ipc.host.o: core.h ipc.h ipc.c
	gcc -Wall -lm -c -o ipc.host.o ipc.c

input.host.o: core.h input.h input.c
	gcc -Wall -lm -c -o input.host.o input.c

output.host.o: core.h output.h output.c
	gcc -Wall -lm -c -o output.host.o output.c

clean:
	-rm *.o
	-rm *.out
	@echo ">>> Cleaned up project"
