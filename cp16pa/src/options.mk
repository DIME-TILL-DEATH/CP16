PRJ_NAME=CabSim_mini

SDK_DIR=$(SRC_DIR)/../../SDK_embedded

PLATFORM=PANGEA_MINI
DEV_DESCRIPTION=\"cp16_module\"
PROJECT_DESCRIPTION=\"cp16pa\"
FIRMWARE_NAME="CP16.PA.LA."
FIRMWARE_VER="1.05.04"

PLL_N=336	
PLL_Q=7

FLASH_TEXT_SECTION_OFFSET=0x40000
FLASH_ORIGIN=0x08040000

CPU_EXT_FLAGS=
EXT_DEFS=



# set a RAM vector table define
RAM_VEC_TABLE=0
# add project specific hardware defines
HARDWARE_DEFS+= -DRAM_VEC_TABLE=$(RAM_VEC_TABLE)  -DUSE_USB_OTG_FS -DUSE_EMBEDDED_PHY -DUSE_OTG_MODE -D__PANGEA_MINI_REV2__  -D__LA3_MOD__

# set a FreeRTOS defines
TICK_RATE_HZ_DEFAULT=1000UL
MAX_PRIORITIES=4UL
MINIMAL_STACK_SIZE=64UL
INTERNAL_SRAM_POOL_SIZE="(40*1024UL)"
MAX_TASK_NAME_LEN=16UL

USE_FREERTOS=1
FREERTOS_DEFS= -DTICK_RATE_HZ_DEFAULT=$(TICK_RATE_HZ_DEFAULT)  \
               -DMAX_PRIORITIES=$(MAX_PRIORITIES)              \
               -DMINIMAL_STACK_SIZE=$(MINIMAL_STACK_SIZE)      \
               -DMAX_TASK_NAME_LEN=$(MAX_TASK_NAME_LEN)


TLSF_MAX_FLI=20
TLSF_DEFS=-DTLSF_USE_LOCKS -D__USE_FREERTOS__ -DTLSF_MAX_FLI=$(TLSF_MAX_FLI) -D_DEBUG_TLSF_
#APP_DEFS=  not additition params

#OPT_LTO=8
COMPILE_EXT_FLAGS=  -DSTM32F40_41xxx
#-mlong-calls
OPT_EXT_FLAGS=
#-fno-lto
#LINK_EXT_FLAGS=-Wl,-nostdlib