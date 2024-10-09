LIBNAME = libusb

include $(SRC_DIR)/rules.mk

CFLAGS += -I./ \
	-I./class/msc \
	-I./class/cdc \
	-I./device \
	-I./otg \
	-I$(SRC_DIR)/include \
	-I$(SRC_DIR)/libstorage \
	-I$(STM32_SDCARD_COMMON_SRC_DIR) \
	-I$(STM32_PERIFERIAL_DRV_SRC_DIR)  \
	-I$(STM32_CMSIS_COMMON_SRC_DIR) \
	-I$(STM32_CMSIS_SRC_DIR)	\
	-I$(FATFS_SRC_DIR) \
	$(HARDWARE_DEFS)
