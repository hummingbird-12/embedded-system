hw1.out: main.o input.o output.o
	gcc -Wall -lm -o hw1.out main.o input.o output.o
	@echo ">>> To execute, type ./hw1.out"

input.o: core.h input.h input.c
	gcc -Wall -lm -c -o input.o input.c

output.o: core.h output.h output.c
	gcc -Wall -lm -c -o output.o output.c

clean:
	-rm *.o
	-rm hw1.out
	@echo ">>> Cleaned up project"
