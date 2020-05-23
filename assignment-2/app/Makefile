APP		:= timer
CC		:= arm-none-linux-gnueabi-gcc

default:
	$(CC) -static -o $(APP) $(APP).c

push: default
	adb push $(APP) /data/local/tmp

clean:
	rm -rf $(APP)
