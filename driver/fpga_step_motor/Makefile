#Makefile for a basic kernel module

obj-m   := fpga_step_motor_driver.o

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)
APP		:= fpga_test_step_motor

all: driver app
#all: driver 

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

app:
	arm-none-linux-gnueabi-gcc -static -o $(APP) $(APP).c

install:
	cp -a fpga_step_motor_driver.ko /nfsroot
	cp -a $(APP) /nfsroot
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf $(APP)
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
	rm -rf .fpga*
