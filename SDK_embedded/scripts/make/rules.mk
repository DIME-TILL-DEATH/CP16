include $(SDK_DIR)/scripts/make/tools.mk
include $(SDK_DIR)/scripts/make/platform.mk
include $(SDK_DIR)/scripts/make/options.mk
include $(SDK_DIR)/scripts/make/paths.mk

include $(SDK_DIR)/scripts/make/gnu_make_std_lib/gmsl

LIB_SRC_SUBDIRS = $(addprefix $(LIB_SRC_DIR), $(SUBDIR_WALKER))

#вычисление списков имен файлов исходных кодов ASM/C/C++/Fortran/Binary
ASSRCS        =  $(wildcard $(addsuffix s,   $(LIB_SRC_SUBDIRS))) 
CSRCS         =  $(wildcard $(addsuffix c,   $(LIB_SRC_SUBDIRS))) 
CXXSRCS_CC    =  $(wildcard $(addsuffix cc,  $(LIB_SRC_SUBDIRS))) 
CXXSRCS_CPP   =  $(wildcard $(addsuffix cpp, $(LIB_SRC_SUBDIRS)))
FSRCS         =  $(wildcard $(addsuffix f,   $(LIB_SRC_SUBDIRS)))
DATASRCS      =  $(wildcard $(addsuffix ldr,$(LIB_SRC_SUBDIRS)))

SRCS=$(ASSRCS) $(CSRCS) $(CXXSRCS_CC) $(CXXSRCS_CPP) $(FSRCS) $(DATASRCS)

DEPEND_LIBS = $(wildcard $(LIB_DIR)/*.stamp)
#вычисление списка автоматически собираемых библиотек
AUTO_LIBS = $(basename  $(DEPEND_LIBS))

#вычисление списка имен объектных файлов
OBJS = $(subst $(LIB_SRC_DIR)/,, $(ASSRCS:.s=.o) $(CSRCS:.c=.o) $(CXXSRCS_CC:.cc=.o) $(CXXSRCS_CPP:.cpp=.o) $(FSRCS:.f=.o) $(DATASRCS:.ldr=.o))    
LSTS = $(OBJS:=.lst)

HEX = $(FULLNAME:.elf=.hex)
RAW_BIN = $(FULLNAME:.elf=.bin)
LSS = $(FULLNAME:.elf=.lss)
DUMP = $(FULLNAME:.elf=.dmp)

BUILD_SCRIPTS = $(wildcard $(addsuffix /*.mk   , $(SDK_DIR)/scripts/*/*/*))	\
				$(wildcard $(addsuffix /*.m4   , $(SDK_DIR)/scripts/ld))	\
                $(wildcard $(addsuffix /*.mk   , $(SRC_DIR))) \
                $(wildcard $(addsuffix /*.mk   , $(SRC_DIR)/src )) \
				./GNUmakefile

# перворачивание списка
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

# обход по корню проекта, вычисление списка директорий сборки
BUILD_DIRS=$(wildcard lib* ) application*

view_build_dirs_list:
	@echo $(BUILD_DIRS)

#----------------------------------------------------------------------------------------------------------
alls: $(BUILD_DIRS)
	@for f in $(BUILD_DIRS); do \
		if [ -d $$f ] ; then \
	 		cd $$f; \
	 		if [ -f ../$(call make_file_from_sdk,$$f) ] ; then echo `ln -sf ../$(call make_file_from_sdk,$$f) GNUmakefile` ; fi ; \
	 		$(MAKE) SRC_DIR=../$(SRC_DIR) -j$(MAKE_JOBS) all || exit 1; \
	 		if [ -f ../$(call make_file_from_sdk,$$f) ] ; then rm -f GNUmakefile; fi ; \
	 		cd ../; \
		fi ;\
	done

cleans: $(BUILD_DIRS)
	@for f in $(BUILD_DIRS); do \
		if [ -d $$f ] ; then \
	 		cd $$f; \
	 		if [ -f ../$(call make_file_from_sdk,$$f) ] ; then echo `ln -sf ../$(call make_file_from_sdk,$$f) GNUmakefile` ; fi ; \
	 		$(MAKE) SRC_DIR=../$(SRC_DIR) -j$(MAKE_JOBS) clean || exit 1; \
	 		if [ -f ../$(call make_file_from_sdk,$$f) ] ; then rm -f GNUmakefile; fi ; \
	 		cd ../; \
		fi ;\
	done
	$(RM) -f $(LD_SCRIPT) $(DEPEND_LIBS)

#проверка на наличие GNUmakefile_$(MAKE_DIR) в SDK, в cлучае его наличия - создание ссылки GNUmakefile с ее удалением после сборки
#при отсутствии файла - сборка по умолчанию с поиском в текщей директории GNUmakefile
#данное правило актуально в директории библиотеки
%:
	if [ -f $(call make_file_from_sdk,$(MAKE_DIR)) ] ; then ln -sf ../$(call make_file_from_sdk,$(MAKE_DIR)) $(MAKE_DIR)/GNUmakefile ; fi ; \
	$(MAKE) SRC_DIR="../$(SRC_DIR)" -j$(MAKE_JOBS)  $(MAKE_TARGET) -C $(MAKE_DIR); \
	if [ -f $(call make_file_from_sdk,$(MAKE_DIR)) ] ; then rm -f $(MAKE_DIR)/GNUmakefile; fi ;
	

#----------------------------------------------------------------------------------------------------------

all: $(FULLNAME) $(HEX) $(RAW_BIN)

clean:	clean_dep
	$(RM) -fr $(FULLNAME) $(OBJS) $(LSTS) $(CUSTOM_RM_FILES)  ./.dep

%.ld:$(LD_SCRIPT_M4) $(BUILD_SCRIPTS)
	$(M4) $(LDGENFLAGS) $< > $(LD_SCRIPT)

VPATH = $(LIB_SRC_DIR)

%.o:%.c $(BUILD_SCRIPTS)
	$(CC)  $(CFLAGS) -c -o $@  $<

%.o:%.s $(BUILD_SCRIPTS)
	$(AS) $(ASFLAGS) -o $@  $<

%.o:%.ldr $(BUILD_SCRIPTS)
	$(CP) $(CPFLAGSBINDATA) $< $@

%.o:%.cc $(BUILD_SCRIPTS)
	$(CXX) $(CXXFLAGS) -c -o $@  $<

%.o:%.cpp $(BUILD_SCRIPTS)
	$(CXX) $(CXXFLAGS) -c -o $@  $<

%.o:%.f $(BUILD_SCRIPTS)
	$(FC) $(FFLAGS) -c -o $@  $<

%.a:$(OBJS) $(BUILD_SCRIPTS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RL) $(RLFLAGS) $@
	$(TOUCH)  $@.stamp

%.elf:$(OBJS) $(DEPEND_LIBS) $(BUILD_SCRIPTS) $(LD_SCRIPT)
	$(CXX) $(LD_SCRIPT_OPT) $(LINK_FLAGS)  -Wl,--start-group  $(OBJS)  $(AUTO_LIBS) $(LIBS) -Wl,--end-group -o $@ -Wl,-Map,$(OUT_DIR)/$(APPNAME).map

%.bin: %.elf
	$(CP) $(CPFLAGS_RAW_BIN) $< $@

%.hex: %.elf
	$(CP) $(CPFLAGS_HEX) $< $@
	$(OD) $(ODFLAGS_DUMP) $< > $(<:.elf=.dmp)
	$(OD) $(ODFLAGS_LSS)  $< > $(<:.elf=.lss)
	$(MEM_USAGE) $< $(MEM_USAGE_OPT)

dist:clean
	$(TAR) --exclude={CVS,cvs} -cvzf $(PKG_NAME)-$(DATE).tar.gz *

clean_svn:
	$(CD) ../; $(RM) -fr `$(FIND) | $(GREP) .svn`

clean_dep:
	$(CD) ../; $(RM) -fr `$(FIND) | $(GREP) .dep`

tools_version:
	$(CC)  -v
	$(LD)  -v
	$(GDB) -v

doxy:
	doxygen GNUdoxy

# 
dfu_write_flash:
	$(DFU_UTIL) $(DFUFLAGS_WRITE_FLASH) $(RAW_BIN)
dfu_read_flash:
	$(DFU_UTIL) $(DFUFLAGS_READ_FLASH)  $(RAW_BIN)
gdb_read_flash:
	$(GDB) $(GDB_READ_FLASH)  flash_image.bin
# phony targets
.PHONY:all clean dist tools_version
.PRECIOUS: %.o %.o.lst %.ld

-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
