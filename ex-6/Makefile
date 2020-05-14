obj-m = kernel_timer.o 
CC := arm-none-linux-gnueabi-gcc

KDIR    :=/work/achroimx_kernel
PWD := $(shell pwd)

all: driver app
#all: driver 

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

app:
	arm-none-linux-gnueabi-gcc -static -o kernel_timer_test kernel_timer_test.c
	
	
clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf .*.cmd
	rm -rf .tmp_versions
	rm -rf kernel_timer_test
