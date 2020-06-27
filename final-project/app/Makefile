APP		:= app
CC		:= arm-none-linux-gnueabi-gcc
OUT		:= app

ADB			:= adb
ADB_DIR		:= /data/local/tmp

default:
	$(CC) -Wall -static -o $(OUT) $(APP).c

push: default
	$(ADB) push -p $(OUT) $(ADB_DIR)	
	@printf "\n[APP] Push complete\n\n"

clean:
	rm -rf $(OUT)
