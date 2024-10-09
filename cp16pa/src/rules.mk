include $(SRC_DIR)/options.mk
include $(SDK_DIR)/scripts/make/rules.mk

mkfirmware_cp16_bt:
	pwd
	cd ../out && ../../bootloader_cp16/pc/mkfirmware_cp16_bt
	cd ../out && mv -f firmware firmware_CP16.PA_$(shell date +%Y%m%d)
