obj-m	:= driver.o
MOD 	:= $(subst .o,.ko,$(obj-m))

KDIR	:=  /work/achroimx_kernel
PWD		:= $(shell pwd)

ADB			:= adb
ADB_DIR		:= /data/local/tmp

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

push: default
	$(ADB) push $(MOD) $(ADB_DIR)
	echo "[MODULE] Push complete\n"

clean:
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf .*.cmd
	rm -rf .tmp_versions
