# ��������� �������� ��������� ��������
ROOT_DIR=$(SRC_DIR)/..
OUT_DIR=$(ROOT_DIR)/out
LIB_DIR=$(ROOT_DIR)/lib
DOC_DIR=$(ROOT_DIR)/doc
PC_DIR=$(ROOT_DIR)/pc
SCRIPT_DIR=$(ROOT_DIR)/scripts

#функция вычисления GNUmakefile_XXX
make_file_from_sdk=$(SDK_DIR)/scripts/make/libs/stm32/GNUmakefile_$(1)

ifndef TOOLS_VARIIANT 
		TOOLS_VARIIANT = arm-kgp-eabi
endif
#-------------------------------------------------------------------
#	���������� ������� ������ �� ���������
ifeq ($(origin MAKE_JOBS), undefined)
	MAKE_JOBS=12
endif
#-------------------------------------------------------------------
#      ���������� ���������� � ���� ������ make
ifeq ($(origin MAKE_DIR_TARGET_SEPARATOR), undefined)
	MAKE_DIR_TARGET_SEPARATOR=/
endif


MAKE_DIR=   $(word 1,$(subst $(MAKE_DIR_TARGET_SEPARATOR), ,$@))
MAKE_TARGET=$(word 2,$(subst $(MAKE_DIR_TARGET_SEPARATOR), ,$@))


ifeq ($(origin MAKE_DIR_TARGET_SEPARATOR), undefined)
	MAKE_DIR_TARGET_SEPARATOR=/
endif
#-------------------------------------------------------------------
# ��������� ������� ������� �������
ifeq ($(origin LIB_SRC_DIR), undefined)
  LIB_SRC_DIR=.
endif

# find a memutz utilite
#MEM_USAGE = $(word 1, $(notdir $(filter-out $(MEMUTZ): $(SIZE):, $(shell whereis $(MEMUTZ) $(SIZE)))))
MEM_USAGE=memutz
ifeq ($(MEM_USAGE),$(MEMUTZ))
	MEM_USAGE_OPT = $(FLASH_SIZE) $(RAM_SIZE)
endif

#-------------------------------------------------------------------
#������� ����������� ������������� �������� ����� ��������
SUBDIR_WALKER=	/*.
ifeq ($(origin NO_SUBDIR_WALKER), undefined)
	SUBDIR_WALKER+=	/*/*.	\
			/*/*/*.	\
			/*/*/*/*.
endif
#-------------------------------------------------------------------
ifndef LIBNAME
	ifndef APPNAME 
		APPNAME = image
	endif
endif

# ���������� ����� ������
ifdef LIBNAME
	FULLNAME = $(LIB_DIR)/$(LIBNAME).a
endif

ifdef APPNAME
	FULLNAME = $(OUT_DIR)/$(APPNAME).elf
	IHEXNAME = $(OUT_DIR)/$(APPNAME).ihex
endif
#-------------------------------------------------------------------
# memory pools defenition
    
# internal sram memory  
ifdef INTERNAL_SRAM_POOL_SIZE
    MEM_POOLS_DEFS += -DINTERNAL_SRAM_POOL_SIZE=$(INTERNAL_SRAM_POOL_SIZE)
endif

# external memory bank 
	ifdef EXT_MEM_BANK0_SIZE
		ifdef EXT_MEM_BANK0_POOL_SIZE
			MEM_POOLS_DEFS +=-DEXT_MEM_BANK0_POOL_SIZE=$(EXT_MEM_BANK0_POOL_SIZE)
		endif
	endif
	
	ifdef EXT_MEM_BANK1_SIZE
		ifdef EXT_MEM_BANK1_POOL_SIZE
			MEM_POOLS_DEFS +=-DEXT_MEM_BANK1_POOL_SIZE=$(EXT_MEM_BANK1_POOL_SIZE)
		endif
	endif	

	ifdef EXT_MEM_BANK2_SIZE
		ifdef EXT_MEM_BANK2_POOL_SIZE
			MEM_POOLS_DEFS +=-DEXT_MEM_BANK2_POOL_SIZE=$(EXT_MEM_BANK2_POOL_SIZE)
		endif
	endif

	ifdef EXT_MEM_BANK3_SIZE
		ifdef EXT_MEM_BANK3_POOL_SIZE
			MEM_POOLS_DEFS +=-DEXT_MEM_BANK3_POOL_SIZE=$(EXT_MEM_BANK3_POOL_SIZE)
		endif
	endif
#-------------------------------------------------------------------
ifndef LD_SCRIPT
	# ��������� m4 ������� ��������� ld ������� �� ���������

	# ��������� ����� �����. 
	ifndef ENTRY_SYMBOL
		# ���� ������ �� ��������� �� �� ����������� �� ��������� flash_vec_table ,
		# ������������ � ���������� libcrt
		ENTRY_SYMBOL = vec_table
	endif	

	ifndef LD_SCRIPT_M4
		ifeq ($(origin LINK_IN_RAM), undefined)
			LD_SCRIPT_M4 = $(SDK_DIR)/scripts/ld/ld_flash.m4
		else
			LD_SCRIPT_M4 = $(SDK_DIR)/scripts/ld/ld_ram.m4
		endif
	endif
	
	# смещение секции кода относительно начала начала адресного пространсва флеша
	# используется для связки загрузчика и приложения (размещение приложения)
	# в свободной от кода згрузчика части флеша.
	ifndef FLASH_TEXT_SECTION_OFFSET    
	    FLASH_TEXT_SECTION_OFFSET = 0x00000000
	endif

	ifndef STACK_END_OFFSET
		STACK_END_OFFSET = 0x00000000
	endif
	ifndef EEPROM_SIZE
		EEPROM_SIZE = 0K
	endif
	

	#���������� ��������� ������ m4 ��� ��������� ld �������
	LDGENFLAGS+=	-DENTRY_SYMBOL=$(ENTRY_SYMBOL)		\
			-DFLASH_ORIGIN=$(FLASH_ORIGIN)		\
			-DFLASH_TEXT_SECTION_OFFSET=$(FLASH_TEXT_SECTION_OFFSET) \
			-DFLASH_SIZE=$(FLASH_SIZE)		\
			-DRAM_ORIGIN=$(RAM_ORIGIN)		\
			-DRAM_SIZE=$(RAM_SIZE)			\
			-DSTACK_END_OFFSET=$(STACK_END_OFFSET)  \
			-DEEPROM_SIZE=$(EEPROM_SIZE)	
			
	# CCM RAM
	ifndef CCM_RAM_SIZE
		CCM_RAM_SIZE=0K
	endif
	LDGENFLAGS+= -DCCM_RAM_ORIGIN=$(CCM_RAM_ORIGIN)	-DCCM_RAM_SIZE=$(CCM_RAM_SIZE)
		
	# external memory bank 
	ifdef EXT_MEM_BANK0_SIZE
		ifndef EXT_MEM_BANK0_ORIGIN
			EXT_MEM_BANK0_ORIGIN = 0xFFFFFFFF
		endif
		LDGENFLAGS+= -DEXT_MEM_BANK0_ORIGIN=$(EXT_MEM_BANK0_ORIGIN) -DEXT_MEM_BANK0_SIZE=$(EXT_MEM_BANK0_SIZE)
	endif
	
	ifdef EXT_MEM_BANK1_SIZE
		ifndef EXT_MEM_BANK1_ORIGIN
			EXT_MEM_BANK1_ORIGIN = 0xFFFFFFFF
		endif
		LDGENFLAGS+= -DEXT_MEM_BANK1_ORIGIN=$(EXT_MEM_BANK1_ORIGIN) -DEXT_MEM_BANK1_SIZE=$(EXT_MEM_BANK1_SIZE)
	endif
	
	ifdef EXT_MEM_BANK2_SIZE
		ifndef EXT_MEM_BANK2_ORIGIN
			EXT_MEM_BANK2_ORIGIN = 0xFFFFFFFF
		endif
		LDGENFLAGS+= -DEXT_MEM_BANK2_ORIGIN=$(EXT_MEM_BANK2_ORIGIN) -DEXT_MEM_BANK2_SIZE=$(EXT_MEM_BANK2_SIZE)
	endif
	
	ifdef EXT_MEM_BANK3_SIZE
		ifndef EXT_MEM_BANK3_ORIGIN
			EXT_MEM_BANK3_ORIGIN = 0xFFFFFFFF
		endif
		LDGENFLAGS+= -DEXT_MEM_BANK3_ORIGIN=$(EXT_MEM_BANK3_ORIGIN) -DEXT_MEM_BANK3_SIZE=$(EXT_MEM_BANK3_SIZE)
	endif	

	#��� ����� ������� ld � ����� � ��� ��������� 
	LD_SCRIPT=$(SCRIPT_DIR)/$(PRJ_NAME).ld
endif
	
LD_SCRIPT_OPT=-T $(LD_SCRIPT)

#----- hardware auto defs ----------------------------------------
HARDWARE_DEFS+= -DF_OSC=$(F_OCS) -DOSC_TYPE=$(OSC_TYPE)
ifneq ($(origin PLL_N), undefined)
   PLL_CONFIG=4
   HARDWARE_DEFS+=-DPLL_N=$(PLL_N) -DPLL_Q=$(PLL_Q)
else
   ifndef PLL_CONFIG
       PLL_CONFIG=0
   endif
endif
HARDWARE_DEFS+=-DPLL_CONFIG=$(PLL_CONFIG)

#----- CRT auto defines -------------------------------------------------
ifneq ($(origin USE_RAM_VEC_TABLE), undefined)
	CRT_DEFS+= -D__USE_RAM_VEC_TABLE__
endif

ifneq ($(origin DELAY_FOR_GDB), undefined)
	CRT_DEFS+= -DDELAY_FOR_GDB=$(DELAY_FOR_GDB)
endif

ifneq ($(origin USE_FREERTOS), undefined)
	CRT_DEFS+= -D__USE_FREERTOS__
endif

ifneq ($(origin EXT_MEM_BANK0_SIZE), undefined)
	CRT_DEFS+= -D__EXT_MEM_BANK0__
endif

ifneq ($(origin EXT_MEM_BANK1_SIZE), undefined)
	CRT_DEFS+= -D__EXT_MEM_BANK1__
endif

ifneq ($(origin EXT_MEM_BANK2_SIZE), undefined)
	CRT_DEFS+= -D__EXT_MEM_BANK2__
endif

ifneq ($(origin EXT_MEM_BANK3_SIZE), undefined)
	CRT_DEFS+= -D__EXT_MEM_BANK3__
endif

#------application auto defs -------------------------------------

ifneq ($(origin USE_FREERTOS), undefined)
	APP_DEFS+= -D__USE_FREERTOS__
endif

ifndef DEV_DESCRIPTION
	DEV_DESCRIPTION="\"unknown device"\"
endif
APP_DEFS+= -DDEV_DESCRIPTION=$(DEV_DESCRIPTION)

ifndef PROJECT_DESCRIPTION
	PROJECT_DESCRIPTION="\"unknown project"\" 
endif
APP_DEFS+= -DPROJECT_DESCRIPTION=$(PROJECT_DESCRIPTION)

#-----------------------------------------------------------------
# �������� ������
DATE		= $$(date +%Y%m%d)
PKG_NAME	= $(LIBNAME)$(PRJNAME)

# ����� ������������
#ifndef ARFLAGS
	ARFLAGS = -rcs 
#endif

ifndef RLFLAGS
	RLFLAGS = -t 
endif

# ��������� �������� � ������� Intel HEX
ifndef CPFLAGS_HEX
	CPFLAGS_HEX = --strip-all  -O ihex
endif

# ��������� �������� � ������� raw binary
ifndef CPFLAGS_RAW_BIN
	CPFLAGS_RAW_BIN = --strip-all  -O binary
endif

ifndef CPFLAGSBINDATA
	CPFLAGSBINDATA = -I binary -O elf32-littlearm  -B arm --rename-section .data=.rodata
endif

# ��������� �����
ifndef ODFLAGS_DUMP
	ODFLAGS_DUMP	= -x --syms
endif

# ��������� ��������
ifndef ODFLAGS_LSS
	ODFLAGS_LSS	= -h -C -S -d --demangle
endif

# general compilation options -----------------------------------------------
ifndef ASM_LST_FLAGS
	ASM_LST_FLAGS=-adhlns=$@.lst $(ASM_LST_EXT_FLAGS)
endif

# DEPENDENSIS ----------------------------------------------------------------
ifndef DEP_FLAGS
	DEP_FLAGS=-Wp,-M,-MP,-MT,$@,-MF,.dep/$(@F).dep $(DEP_EXT_FLAGS)
endif

# WARNINGS -------------------------------------------------------------------
ifndef WARN_FLAGS
	WARN_FLAGS=-W -Wall -Wno-unused-parameter $(WARN_EXT_FLAGS)
endif

# DFU operation --------------------------------------------------------------
ifndef DFUFLAGS_WRITE_FLASH
	DFUFLAGS_WRITE_FLASH=--device $(DFU_USB_VIDPID) --alt 0 --dfuse-address $(FLASH_ORIGIN) -D 
endif

ifndef DFUFLAGS_READ_FLASH
	DFUFLAGS_READ_FLASH=--device $(DFU_USB_VIDPID) --alt 0 --dfuse-address $(FLASH_ORIGIN)  -U
endif

# OPTIMIZATION ---------------------------------------------------------------
ifndef OPT_LEVEL
	OPT_LEVEL=-Ofast
endif

ifndef OPT_FLAGS
	OPT_FLAGS=$(OPT_LEVEL) -fomit-frame-pointer -finline-functions -ffunction-sections -fdata-sections -fgraphite -funroll-loops
endif

#LTO optimization
ifdef OPT_LTO
	OPT_FLAGS+=-flto=$(OPT_LTO) -ffat-lto-objects
endif

#add user defined optimization flags
OPT_FLAGS+=$(OPT_EXT_FLAGS)

# DEBUG ----------------------------------------------------------------------
ifndef DBG_FLAGS
	DBG_FLAGS=-ggdb3 $(DBG_EXT_FLAGS) -fverbose-asm
endif

# ASM OPT OUTPUT -------------------------------------------------------------
ifndef HL_TO_ASM_FLAGS
	HL_TO_ASM_FLAGS=-Wa,$(ASM_LST_FLAGS) $(HL_TO_ASM_EXT_FLAGS)
endif

ifeq ($(CPU),CORTEX_M4F)
	CPU_FLAGS=-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb $(CPU_EXT_FLAGS)
	LANGUAGES_FLAGS+= -fsingle-precision-constant
else ifeq ($(CPU),CORTEX_M3_MPU)
	CPU_FLAGS=-mcpu=cortex-m3 -mfloat-abi=soft -mthumb $(CPU_EXT_FLAGS)
else ifeq ($(CPU),CORTEX_M3)
	CPU_FLAGS=-mcpu=cortex-m3 -mfloat-abi=soft -mthumb $(CPU_EXT_FLAGS)
else ifeq ($(CPU),ARM7TDMI)
	CPU_FLAGS=-mcpu=arm7tdmi -mfloat-abi=soft -marm $(CPU_EXT_FLAGS)	
endif

COMMON_PROJECT_INCLIDE_PATH=   \
                   -I./ \
                   -I$(SRC_DIR)/include \
                   -I$(SDK_DIR)/libs/common \
			       -I$(STM32_PERIFERIAL_DRV_SRC_DIR)  \
			       -I$(STM32_CRT_COMMON_SRC_DIR) \
			       -I$(STM32_CRT_SRC_DIR)	\
			       -I$(STM32_CMSIS_COMMON_SRC_DIR) \
        	       -I$(STM32_CMSIS_SRC_DIR)	\
        	       -I$(STM32_DSP_SRC_DIR) \

COMPILE_FLAGS+=		$(DEFS) \
					$(CPU_FLAGS) \
					$(OPT_FLAGS) \
					$(DEP_FLAGS) \
				 	$(WARN_FLAGS) \
				 	$(DBG_FLAGS) \
				 	$(COMMON_PROJECT_INCLIDE_PATH) \
				 	$(HL_TO_ASM_EXT_FLAGS) \
				 	$(COMPILE_EXT_FLAGS) \
				 	$(LANGUAGES_FLAGS)

# C STANDARD ---------------------------------------------------------------
ifndef CSTD_FLAGS
	CSTD_FLAGS=-std=gnu99
	#gnu99,c11
endif

# C++ STANDARD ---------------------------------------------------------------
ifndef CPPSTD_FLAGS
	CPPSTD_FLAGS=-std=gnu++23
endif

# FORTRAN STANDARD ---------------------------------------------------------------
ifndef FSTD_FLAGS
	FSTD_FLAGS=-ffree-form
endif

#--------------------------------------------------------------------------------
CFLAGS+=	$(CSTD_FLAGS) 	$(COMPILE_FLAGS)	$(C_EXT_FLAGS)
FFLAGS+=	$(FSTD_FLAGS) 	$(COMPILE_FLAGS)	$(F_EXT_FLAGS)
CXXFLAGS+=	$(CPPSTD_FLAGS)	$(COMPILE_FLAGS)	-fno-exceptions -fno-rtti $(CPP_EXT_FLAGS)
ASFLAGS+=	$(CPU_FLAGS)	$(COMPILE_DEBUG_FLAGS) -mapcs-32 $(ASM_LST_FLAGS) $(CRT_CONFIG) $(AS_EXT_FLAGS)

#LINK FLAGS ----------------------------------------------------------------------
LINK_FLAGS+= -fuse-linker-plugin -nostartfiles  $(CPU_FLAGS) $(OPT_FLAGS) $(DBG_FLAGS) -L$(LIB_DIR) -Wl,-gc-sections $(LINK_EXT_FLAGS)



