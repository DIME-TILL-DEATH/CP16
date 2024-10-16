SRC_DIR=.
include rules.mk

mkfirmware:
	cd ../out && ./mkfirmware && cp firmware firmware_$(FIRMWARE_NAME)_$(FIRMWARE_VER)_`date +%Y%m%d` && rm firmware
