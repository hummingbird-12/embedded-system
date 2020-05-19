obj-m := interrupt.o
CC := arm-none-linux-gnueabi-gcc

KDIR := /work/achroimx_kernel/
PWD := $(shell pwd)

all: interrupt testapp

interrupt:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

testapp:
	$(CC) -static -o intertest intertest.c

clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf .*.cmd
	rm -rf .tmp_versions
	rm -rf intertest
