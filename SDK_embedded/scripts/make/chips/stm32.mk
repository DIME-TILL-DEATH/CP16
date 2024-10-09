#таблица соответствия микросхемы 
# !!!!!!! нада сделать!!!!!

#база размеров флеша и озу
ifeq      ($(CHIP),STM32F103RET6)
	FLASH_SIZE=512K
	RAM_SIZE=64K
else ifeq ($(CHIP),STM32F103RBT6)
	FLASH_SIZE=128K
	RAM_SIZE=20K
else ifeq ($(CHIP),STM32F103C8T6)
	FLASH_SIZE=64K
	RAM_SIZE=20K
else ifeq ($(CHIP),STM32F107VCT6)
	FLASH_SIZE=256K
	RAM_SIZE=64K
else ifeq ($(CHIP),STM32F405RGT6)
	FLASH_SIZE=1M
	RAM_SIZE=128K
	CCM_RAM_SIZE=64K
else ifeq ($(CHIP),STM32F415RGT6)
	FLASH_SIZE=1M
	RAM_SIZE=128K
	CCM_RAM_SIZE=64K
else ifeq ($(CHIP),STM32F407VET6)
	FLASH_SIZE=512K
	RAM_SIZE=128K
	CCM_RAM_SIZE=64K
	else ifeq ($(CHIP),STM32F407VGT6)
	FLASH_SIZE=1M
	RAM_SIZE=128K
	CCM_RAM_SIZE=64K
else ifeq ($(CHIP),STM32F427VI)
	FLASH_SIZE=2M
	RAM_SIZE=192K
	CCM_RAM_SIZE=64K
else ifeq ($(CHIP),STM32L151C8T6)
	FLASH_SIZE=64K
	RAM_SIZE=10K
else ifeq ($(CHIP),STM32F100C4T6)
	FLASH_SIZE=16K
	RAM_SIZE=4K
else ifeq ($(CHIP),STM32F429ZIT6)
	FLASH_SIZE=2M
	RAM_SIZE=192K
	CCM_RAM_SIZE=64K
else ifeq ($(CHIP),STM32F427VIT6)
	FLASH_SIZE=2M
	RAM_SIZE=192K
	CCM_RAM_SIZE=64K
else
        $(error KGP BUILD SYSTEM ERROR: Chip $(CHIP) unknown flash/ram size, add chip flash/ram  info to /scripts/make/chips/stm32.mk)
endif


#определение семейства и процессора по микросхеме
ifneq (,$(findstring STM32L1,$(CHIP)))
	CHIP_FAMILY=STM32L1XX
	#CPU=CORTEX_M3_MPU
	CPU=CORTEX_M3
	
	#определение плотности микросхемы STM32L1xx
	# $(EXT_CHIP_FAMILY_DEF) используется для stm32L1xx для для выбора подсемейства:
	# STM32L1XX_MD
	# STM32L1XX_MDP
	# STM32L1XX_HD
	
	#база тиров подсемейств по размеру флеша
	ifeq (32K,$(FLASH_SIZE))
		EXT_CHIP_FAMILY_DEF=-DSTM32L1XX_MD
	else ifeq (64K,$(FLASH_SIZE))
		EXT_CHIP_FAMILY_DEF=-DSTM32L1XX_MD
	else ifeq (128K,$(FLASH_SIZE))
		EXT_CHIP_FAMILY_DEF=-DSTM32L1XX_MD
	else ifeq (256K,$(FLASH_SIZE))
		EXT_CHIP_FAMILY_DEF=-DSTM32L1XX_MDP
	else ifeq (384K,$(FLASH_SIZE))
		EXT_CHIP_FAMILY_DEF=-DSTM32L1XX_HD
	else 
        $(error KGP BUILD SYSTEM ERROR: Chip $(CHIP) unknown chip subfamily ( STM32L MD/MDP/HD ), add chip subfamily  info to /scripts/make/chips/stm32.mk)
	endif

else ifneq (,$(findstring STM32F0,$(CHIP)))
	CHIP_FAMILY=STM32F0XX
	CPU=CORTEX_M0
else ifneq (,$(findstring STM32F1,$(CHIP)))
	# F1 family defines
	CHIP_FAMILY=STM32F1XX
	CPU=CORTEX_M3
	EXT_CHIP_FAMILY_DEF=-DARM_MATH_CM3
	#определение плотности микросхемы STM32F10x
	# $(EXT_CHIP_FAMILY_DEF) используется для stm32f10x для для выбора подсемейства:
	# STM32F10X_LD
	# STM32F10X_LD_VL
	# STM32F10X_MD
	# STM32F10X_MD_VL
	# STM32F10X_HD
	# STM32F10X_HD_VL
	# STM32F10X_XL
	# STM32F10X_CL
	
	#нада сделать парсинг по маркировке но пока костыл
	ifneq      (,$(findstring 103RET,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F10X_HD 
	else ifneq (,$(findstring 103RBT,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F10X_MD
	else ifneq (,$(findstring 103C8T,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F10X_MD
	else ifneq (,$(findstring 107VCT,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F10X_CL
	else ifneq (,$(findstring 100C4T,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F10X_LD_VL
	endif
	
	

	
else ifneq (,$(findstring STM32F2,$(CHIP)))
	CHIP_FAMILY=STM32F2XX
	CPU=CORTEX_M3_MPU
#else ifneq (,$(findstring STM32F3,$(CHIP)))
#	CHIP_FAMILY=STM32F3XX
#	CPU=CORTEX_M4F
else ifneq (,$(findstring STM32F4,$(CHIP)))
	# F4 family defines
	CHIP_FAMILY=STM32F4XX
	CPU=CORTEX_M4F
	EXT_CHIP_FAMILY_DEF=-DARM_MATH_CM4 -D__FPU_USED=1
	
	# subfamily defines
	ifneq      (,$(findstring STM32F401,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F401xx
	else ifneq (,$(findstring STM32F405,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F405xx -DSTM32F40_41xxx
	else ifneq (,$(findstring STM32F407,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F407xx -DSTM32F40_41xxx
	else ifneq (,$(findstring STM32F415,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F415xx -DSTM32F40_41xxx
	else ifneq (,$(findstring STM32F417,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F417xx -DSTM32F40_41xxx
	else ifneq (,$(findstring STM32F427,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F427xx -DSTM32F427_437xx
	else ifneq (,$(findstring STM32F437,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F437xx -DSTM32F427_437xx	
	else ifneq (,$(findstring STM32F429,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F429xx -DSTM32F429_439xx
	else ifneq (,$(findstring STM32F439,$(CHIP)))
		EXT_CHIP_FAMILY_DEF+=-DSTM32F439xx -DSTM32F429_439xx
	endif
else
        $(error KGP BUILD SYSTEM ERROR: Chip $(CHIP) unknown family and cpu, add chip family/cpu info to /scripts/make/chips/stm32.mk)	
endif

#параметры скрипта линкера по микросхеме
FLASH_ORIGIN=0x08000000
RAM_ORIGIN=0x20000000
CCM_RAM_ORIGIN=0x10000000

DFU_USB_VIDPID=0483:df11



