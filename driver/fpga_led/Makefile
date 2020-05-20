#Makefile for a basic kernel module

obj-m   := fpga_led_driver.o

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)

all: driver app
#all: driver 

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

app:
	arm-none-linux-gnueabi-gcc -static -o fpga_test_led fpga_test_led.c

install:
	cp -a fpga_led_driver.ko /nfsroot
	cp -a fpga_test_led /nfsroot
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf fpga_test_led
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .led*
	rm -rf .tmp*
