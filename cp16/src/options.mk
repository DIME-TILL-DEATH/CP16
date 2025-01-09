PRJ_NAME=CabSim_mini

SDK_DIR=$(SRC_DIR)/../../SDK_embedded


PLATFORM=PANGEA_MINI
# Possible CP16, LA3
MODIFICATION=CP16
FIRMWARE_VER="2.00.09"

ifeq ($(MODIFICATION), CP16)
	HARDWARE_DEFS+=-D__PA_VERSION__ -DAMT_DEV_ID=16
	DEV_DESCRIPTION=\"cp16_module\"
	PROJECT_DESCRIPTION=\"cp16\"
	FIRMWARE_NAME="CP16"
	FIRMWARE_PREFIX=""	
else ifeq ($(MODIFICATION), LA3)
	HARDWARE_DEFS+=-D__PA_VERSION__ -DAMT_DEV_ID=17 -D__LA3_MOD__
	DEV_DESCRIPTION=\"la3_module\"
	PROJECT_DESCRIPTION=\"la3\"
	FIRMWARE_NAME="LA3"
	FIRMWARE_PREFIX=""
endif

COMPILE_EXT_FLAGS=  -DSTM32F40_41xxx

PLL_N=336	
PLL_Q=7

#FLASH_TEXT_SECTION_OFFSET=0x40000
#FLASH_ORIGIN=0x08040000

INTERNAL_SRAM_POOL_SIZE="(70*1024UL)"

CPU_EXT_FLAGS=
EXT_DEFS=

# set a RAM vector table define
RAM_VEC_TABLE=0
# add project specific hardware defines
HARDWARE_DEFS+= -DRAM_VEC_TABLE=$(RAM_VEC_TABLE)  -DUSE_USB_OTG_FS -DUSE_EMBEDDED_PHY -DUSE_OTG_MODE -D__PANGEA_MINI_REV2__  

# set a FreeRTOS defines
TICK_RATE_HZ_DEFAULT=1000UL
MAX_PRIORITIES=4UL
MINIMAL_STACK_SIZE=64UL
MAX_TASK_NAME_LEN=16UL

USE_FREERTOS=1
FREERTOS_DEFS= -DTICK_RATE_HZ_DEFAULT=$(TICK_RATE_HZ_DEFAULT)  \
               -DMAX_PRIORITIES=$(MAX_PRIORITIES)              \
               -DMINIMAL_STACK_SIZE=$(MINIMAL_STACK_SIZE)      \
               -DMAX_TASK_NAME_LEN=$(MAX_TASK_NAME_LEN)


TLSF_MAX_FLI=20
TLSF_DEFS=-DTLSF_USE_LOCKS -D__USE_FREERTOS__ -DTLSF_MAX_FLI=$(TLSF_MAX_FLI) -D_DEBUG_TLSF_
#APP_DEFS=  not additition params

#-mlong-calls
OPT_EXT_FLAGS=
#-fno-lto
#LINK_EXT_FLAGS=-Wl,-nostdlib