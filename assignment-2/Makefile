MODULE_DIR	:= module
APP_DIR		:= app

default:
	$(MAKE) -C $(MODULE_DIR)
	$(MAKE) -C $(APP_DIR)

push: default
	$(MAKE) -C $(MODULE_DIR) push
	$(MAKE) -C $(APP_DIR) push
	$(MAKE) clean

clean:
	$(MAKE) -C $(MODULE_DIR) clean
	$(MAKE) -C $(APP_DIR) clean
