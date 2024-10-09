ifndef PLATFORM
	PLATFORM=PLATFORM_UNKNOWN
endif

# определение микросхемы и частоты задающего кварца
CHIP=CHIP_UNKNOWN
#F_OCS=F_OCS_UNKNOWN

#тип опорного генератора -  OSC_TYPE_UNKNOWN=0, EXTERNAL_BQ=1, INTERNAL_RC_16MHZ=2
OSC_TYPE_UNKNOWN=0
EXTERNAL_BQ=1
INTERNAL_RC_16MHZ=2
EXTERNAL_CLOCK=3

#OSC_TYPE=$(OSC_TYPE_UNKNOWN)

ifeq ($(PLATFORM),PANGEA)
	CHIP=STM32F427VIT6
	F_OCS=25000000
	OSC_TYPE=$(EXTERNAL_BQ)
#	EXT_MEM_BANK0_ORIGIN=
#	EXT_MEM_BANK0_SIZE=
else ifeq ($(PLATFORM),PANGEA_MINI)
	CHIP=STM32F405RGT6
	F_OCS=25000000
	OSC_TYPE=$(EXTERNAL_BQ)
#	EXT_MEM_BANK0_ORIGIN=
#	EXT_MEM_BANK0_SIZE=
else
       $(error KGP BUILD SYSTEM ERROR: Platform $(PLATFORM) unknown, add platform info to /scripts/make/platform.mk)	
endif


#chip type         family      id     
#STM8
#  |
#
#STM32
#  |-----------------L1
#  |                  |--------151C8T6
#  |
#  |-----------------F0
#  |-----------------F1
#  |                  |--------103RET6
#  |                  |--------103RBT6
#  |
#  |-----------------F2
#  |-----------------F3
#  |-----------------F4
#                     |--------407VET6
#
#LPC2000
#  |


CHIP_FAMILY=CHIP_FAMILY_UNKNOWN
CPU=CPU_UNKNOWN
FLASH_SIZE=FLASH_SIZE_UNKNOWN
RAM_SIZE=RAM_SIZE_UNKNOWN
FLASH_ORIGIN=FLASH_ORIGIN_UNKNOWN
RAM_ORIGIN=RAM_ORIGIN_UNKNOWN

#определение типа микросхемы
ifneq (,$(findstring STM32,$(CHIP)))
	include $(SDK_DIR)/scripts/make/chips/stm32.mk
else ifneq (,$(findstring LPC2,$(CHIP)))
	include $(SDK_DIR)/scripts/make/chips/lpc2000.mk
else
     $(error KGP BUILD SYSTEM ERROR: Chip $(CHIP) is not valid type (stm32 or lpc2000), add type definition file to /scripts/make/platform.mk)	
endif

DEFS=-D__$(CPU)__ -D__$(CHIP)__ -D__$(CHIP_FAMILY)__ $(EXT_CHIP_FAMILY_DEF) -D__$(PLATFORM)__ $(EXT_DEFS)
