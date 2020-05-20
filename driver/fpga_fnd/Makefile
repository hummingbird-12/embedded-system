#Makefile for a basic kernel module

obj-m   := fpga_fnd_driver.o

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)

all: driver app
#all: driver 

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

app:
	arm-none-linux-gnueabi-gcc -static -o fpga_test_fnd fpga_test_fnd.c

install:
	cp -a fpga_fnd_driver.ko /nfsroot
	cp -a fpga_test_fnd /nfsroot
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf fpga_test_fnd
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .fnd*
	rm -rf .tmp*
