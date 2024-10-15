include $(SRC_DIR)/options.mk
include $(SDK_DIR)/scripts/make/rules.mk

mkfirmware_cp16_bt:
	pwd
	cd ../out && mv -f firmware firmware_CP16_$(shell date +%Y%m%d)
