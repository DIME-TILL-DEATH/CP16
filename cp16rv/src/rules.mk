include $(SRC_DIR)/options.mk
include $(SDK_DIR)/scripts/make/rules.mk

mkfirmware_cp16_bt:
	pwd
	cd ../out && ../../bootloader_cp16/mkfirmware_cp16_bt/mkfirmware
	cd ../out && mv -f firmware firmware_CP16_$(shell date +%Y%m%d)
