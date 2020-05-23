APP		:= timer
CC		:= arm-none-linux-gnueabi-gcc

ADB			:= adb
ADB_DIR		:= /data/local/tmp
ADB_EXEC	:= "$(ADB_DIR)/$(APP) $(foo)"

default:
	$(CC) -static -o $(APP) $(APP).c

push: default
	$(ADB) push $(APP) $(ADB_DIR)
	$(ADB) shell $(ADB_EXEC)

clean:
	rm -rf $(APP)
