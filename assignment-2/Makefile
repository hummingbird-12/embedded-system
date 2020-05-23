MODULE_DIR	:= module
APP_DIR		:= app

ADB			:= adb
ADB_DIR		:= /data/local/tmp

export foo

default:
	$(MAKE) -C $(MODULE_DIR)
	$(MAKE) -C $(APP_DIR)

push: default
	$(MAKE) -C $(MODULE_DIR) push
	$(MAKE) -C $(APP_DIR) push
	$(MAKE) clean

clean:
	$(MAKE) -s -C $(MODULE_DIR) clean
	$(MAKE) -s -C $(APP_DIR) clean
