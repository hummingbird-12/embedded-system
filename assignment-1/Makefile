default: main.o ipc.o input.o output.o devices.o
	arm-none-linux-gnueabi-gcc -static -Wall -lm -o hw1_20161577.out main.o ipc.o input.o output.o devices.o
	@echo ">>> BUILD COMPLETE (hw1_20161577.out)"
	@echo ">>> Type 'adb push ./hw1_20161577.out /data/local/tmp' to push to ARM board."
	@echo ">>> Or do 'make push' to build and push."

push: default
	@echo ">>> Pushing build file to ARM board..."
	adb push ./hw1_20161577.out /data/local/tmp
	@echo ">>> PUSH COMPLETE"

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
