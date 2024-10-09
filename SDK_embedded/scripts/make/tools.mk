#-------------------------------------------------------------------
# default tools for project build
CC      = $(TOOLS_VARIIANT)-gcc
CXX     = $(TOOLS_VARIIANT)-g++
FC      = $(TOOLS_VARIIANT)-gfortran
LD      = $(TOOLS_VARIIANT)-ld
AR      = $(TOOLS_VARIIANT)-ar
AS      = $(TOOLS_VARIIANT)-as
CP      = $(TOOLS_VARIIANT)-objcopy
OD	= $(TOOLS_VARIIANT)-objdump
SIZE      = $(TOOLS_VARIIANT)-size
SR      = $(TOOLS_VARIIANT)-strip
GDB     = $(TOOLS_VARIIANT)-gdb
RL	= $(TOOLS_VARIIANT)-ranlib
RM	= rm
TAR     = tar
TOUCH   = touch
M4 	= m4
MEMUTZ	= memutz
FIND	= find
GREP	= grep
CD	= cd
PERL	= perl
DFU_UTIL = dfu-util
