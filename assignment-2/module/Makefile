obj-m	:= dev_driver.o
dev_driver-objs := driver.o fpga.o

KDIR	:=  /work/achroimx_kernel
PWD		:= $(shell pwd)

MOD 	:= $(subst .o,.ko,$(obj-m))
ADB			:= adb
ADB_DIR		:= /data/local/tmp

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

push: default
	$(ADB) push -p $(MOD) $(ADB_DIR)
	@printf "\n[MODULE] Push complete\n\n"

clean:
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf .*.cmd
	rm -rf .tmp_versions
