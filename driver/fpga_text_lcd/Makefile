#Makefile for a basic kernel module

obj-m   := fpga_text_lcd_driver.o

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)
APP	:= fpga_test_text_lcd

all: driver app
#all: driver 

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

app:
	arm-none-linux-gnueabi-gcc -static -o $(APP) $(APP).c

install:
	cp -a fpga_text_lcd_driver.ko /nfsroot
	cp -a $(APP) /nfsroot
clean:
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf .*.cmd
	rm -rf .tmp_versions
	rm -rf $(APP)
