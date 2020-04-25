default: main.o ipc.o input.o output.o devices.o
	arm-none-linux-gnueabi-gcc -static -Wall -lm -o hw1.out main.o ipc.o input.o output.o devices.o
	@echo ">>> To execute, type ./hw1.out"

push: default
	adb push ./hw1.out /data/local/tmp

main.o: core.h main.h main.c
	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o main.o main.c

ipc.o: core.h ipc.h ipc.c
	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o ipc.o ipc.c

input.o: core.h input.h input.c
	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o input.o input.c

output.o: core.h output.h output.c
	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o output.o output.c

devices.o: core.h devices.h devices.c
	arm-none-linux-gnueabi-gcc -static -Wall -lm -c -o devices.o devices.c

clean:
	-rm *.o
	-rm *.out
	@echo ">>> Cleaned up project"
