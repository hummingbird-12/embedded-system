APP		:= timer
CC		:= arm-none-linux-gnueabi-gcc

ADB			:= adb
ADB_DIR		:= /data/local/tmp

default:
	$(CC) -Wall -static -o $(APP) $(APP).c

push: default
	$(ADB) push -p $(APP) $(ADB_DIR)	
	@printf "\n[APP] Push complete\n\n"

clean:
	rm -rf $(APP)
