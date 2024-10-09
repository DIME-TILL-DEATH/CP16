include $(SRC_DIR)/rules.mk

LIB_FLAGS=	-I$(READLINE_SRC_DIR) \
			-I$(CONSOLE_SRC_DIR) \
			-I$(CONSOLE_HANDLERS_SRC_DIR) \
			-I$(RTCOUNTER_SRC_DIR) \
			-I$(FREERTOS_KERNEL_SRC_DIR)/include \
			-I$(FREERTOS_PORTABLE_SRC_DIR) \
			-I$(MMGR_SRC_DIR) \
			-I$(ITM_TRACE_SRC_DIR) \
			-I$(READLINE_SRC_DIR) \
			-I$(UTILS_SRC_DIR) \
			-I$(CONSOLE_SRC_DIR) \
			-I$(VMATH_SRC_DIR) \
			-I$(STM32_SDCARD_COMMON_SRC_DIR) \
        	-I$(FATFS_SRC_DIR) \
			-I$(KLIBC_SRC_DIR) \
			-I$(SUNSET_SRC_DIR) \
			-I$(VMATH_SRC_DIR) \
			-I$(SRC_DIR)/libusb \
			-I$(SRC_DIR)/libstorage \
			-I$(SRC_DIR)/libusb/device \
			-I$(SRC_DIR)/libusb/otg \
			-DUSE_STDPERIPH_DRIVER \
			$(APP_DEFS) \
			$(HARDWARE_DEFS) \
			$(FREERTOS_DEFS) \
			$(MEM_POOLS_DEFS) \
						\
			-DFIRMWARE_NAME="\$(FIRMWARE_NAME)\" \
			-DFIRMWARE_VER="\$(FIRMWARE_VER)\" \

CXXFLAGS += $(LIB_FLAGS)
CFLAGS += $(LIB_FLAGS)

LIBS =	-lm

LD_EXT_FLAGS=


CUSTOM_RM_FILES=$(OUT_DIR)/image.*






