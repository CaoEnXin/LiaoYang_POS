#
# Makefile v1.1
# Copyright (C) 2008, NEW POS TECHNOLOGY LIMITED
# All rights reserved.
# http://www.newpostech.com
# Written by LiHua <linua_338@163.com>
# Automatically generated at 2015-07-22 15:46:38
#

#
# Project name
#
PROJECT	= LiaoYang_POS

#
# Project type
# [ bootloader boot kernel mapp sapp library linux ] can be selected
#
TYPE	= mapp

#
# Product type
# [ NEW8110 NEW6110 ] can be selected
# If build application, please set as NEW8110
#
PRODUCT	= NEW8110

#
# Root directory of current Project
#
TOPDIR	:= ../

#
# other necessary files
#
include config.mk
include make.config

#
# The directory of souce code and generated middle files.
#
src		= $(TOPDIR)
obj		= $(TOPDIR)obj/

#
# The generated obj file
#
TARGET 	= $(obj)$(PROJECT)
TAR_ELF	= $(TARGET).elf
TAR_BIN	= $(TARGET).bin
TAR_MAP	= $(TARGET).map
TAG_BIN	= $(TARGET)_tag.bin
SIG_BIN	= $(TARGET)_sig.bin
HEX_BIN	= $(TARGET)_sig.hex
LIBRARY = $(obj)lib$(PROJECT).a

#
# The definition of tools
#
CC         = $(CROSS_COMPILE)gcc
CXX        = $(CROSS_COMPILE)g++
AS         = $(CROSS_COMPILE)gcc
AR         = $(CROSS_COMPILE)ar
LINK       = $(CROSS_COMPILE)gcc
RANLIB     = $(CROSS_COMPILE)ranlib
OBJCOPY    = $(CROSS_COMPILE)objcopy
STRIP      = $(CROSS_COMPILE)strip
MAKE       = $(TOOL_PATH)make
MKDIR      = $(TOOL_PATH)mkdir
ECHO       = $(TOOL_PATH)echo
DEL        = $(TOOL_PATH)rm -f
LIST       = $(TOOL_PATH)ls -l
TAG        = $(TOOL_PATH)AppTag
SIGN       = $(TOOL_PATH)SIGN

#
# Set parameter for compiling project
#
INCLUDE =  -I$(dir $(shell $(CC) -print-libgcc-file-name))../../../../include

INCLUDE	+=	-I../SDK/inc
CFLAGS	+= -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -Wdeclaration-after-statement -Wno-pointer-sign
CFLAGS	+= -fno-strict-aliasing -fno-common -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-omit-frame-pointer
CFLAGS	+= -mcpu=arm9tdmi -mapcs -mno-sched-prolog -mabi=apcs-gnu -mno-thumb-interwork -Uarm
CFLAGS	+= -fsigned-char -msoft-float -mlittle-endian -g -ggdb -O1
CFLAGS	+= $(INCLUDE)
EXT_CFLAGS	=	-DWITH_NOSTDLIB -USOAP_DEBUG -DHAVE_CONFIG_H -DUSE_IN_SEOS -DMALLOC_DEBUG
CFLAGS	+= $(EXT_CFLAGS)


CPPFLAGS	+= -Wall -Wundef -Wno-trigraphs
CPPFLAGS	+= -fno-strict-aliasing -fno-common -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-omit-frame-pointer
CPPFLAGS	+= -mcpu=arm9tdmi -mapcs -mno-sched-prolog -mabi=apcs-gnu -mno-thumb-interwork -Uarm
CPPFLAGS	+= -fsigned-char -msoft-float -mlittle-endian -g -ggdb -O1
CPPFLAGS	+= $(INCLUDE)

AFLAGS	=  $(CFLAGS)
ARFLAGS	=  cr

#
# Set parameter for linking project
#
LDFLAGS	=   -static -msoft-float 
LDLIBS	=	../SDK/lib/libseos.a
LDLIBS	+=	../SDK/lib/libemv.a
LDLIBS	+=	../SDK/lib/libcrypto.a
LDLIBS	+=	../SDK/lib/liballoc.a
LDLIBS	+=	../SDK/lib/libTMS.a
LDLIBS	+=	../SDK/lib/libftp.a


ifneq ($(TYPE),linux)
LDFLAGS	+=	-Ttext $(TEXT_BASE) -Wl,-Tza9_gcc.ld,-Map,$(TAR_MAP),--cref -nostartfiles
endif

BIN_FLAGS	= -O binary -S
_all		= info

ifeq ($(TYPE),bootloader)
SIGN_FLAGS	=	-K $(PRIVATE_KEY) -T BOOTLOADER
TAG_FLAGS	=	-T $(PRODUCT)-BOOTLOADER
_all		+=	$(SIG_BIN)
endif

ifeq ($(TYPE),boot)
SIGN_FLAGS	= -K $(PRIVATE_KEY) -T BOOT
TAG_FLAGS	= -T $(PRODUCT)-BOOT
_all		+=	$(SIG_BIN)
endif

ifeq ($(TYPE),kernel)
SIGN_FLAGS	= -K $(PRIVATE_KEY) -T KERNEL
TAG_FLAGS	= -T $(PRODUCT)-KERNEL
_all		+=	$(SIG_BIN)
endif

ifeq ($(TYPE),sapp)
SIGN_FLAGS	= -K $(PRIVATE_KEY) -T APP
TAG_FLAGS	= -T NEW8110-SAPP
_all		+=	$(SIG_BIN)
endif

ifeq ($(TYPE),mapp)
SIGN_FLAGS	= -K $(PRIVATE_KEY) -T APP
TAG_FLAGS	= -T NEW8110-MAPP
_all		+=	$(SIG_BIN)
endif

ifeq ($(TYPE),linux)
TAR_ELF		=	$(TARGET)
_all		+=	$(TAR_ELF)
endif

ifeq ($(TYPE),library)
_all		+=	$(LIBRARY)
endif

#
# add asm file to be compiled
#
_source	:=	Src/init.s


#
# add C file to be compiled
#
_source	+=	Src/wireless.c	\
			Src/appsetup.c	\
			Src/ChkOption.c	\
			Src/Commlib.c	\
			Src/devicetest.c	\
			Src/Display.c	\
			Src/getcard.c	\
			Src/globalvar.c	\
			Src/main.c	\
			Src/MathLib.c	\
			Src/New8110_ICC.c	\
			Src/NewPosApi.c	\
			Src/opermanage.c	\
			Src/otherfunc.c	\
			Src/posfiles.c	\
			Src/PosLib.c	\
			Src/posparams.c	\
			Src/pospin.c	\
			Src/Rs232lib.c	\
			Src/trancomm.c	\
			Src/tranprint.c	\
			Src/tranproc.c	\
			Src/m1_card/m1_card_proc.c	\
			Src/showDebug/showDebug.c	\
			Src/m1_tranproc.c	\
			Src/LocalFtp.c	\
			Src/NewTime.c	\
			Src/st8583.c	\
			Src/emvtranproc.c	\
			Src/emvtrantest.c	\
			Src/Menu.c	\
			Src/otherTxn.c	\
			Src/pack8583.c	\
			Src/qpbocProc.c


#
# add CPP file to be compiled
#
_source	+=	


#
# Do not modify the below content
#
c_object= $(patsubst %.c,%.o,$(_source))
p_object= $(patsubst %.cpp,%.o,$(c_object))
a_object= $(patsubst %.s,%.o,$(p_object))
_object	= $(a_object)
source	= $(foreach var,$(_source),$(src)$(var))
object	= $(foreach var,$(_object),$(obj)$(var))
depend	= $(foreach target,$(_source),$(obj)$(target).dep)

#
# if no cpp file in project, use gcc for link
#
cpp_source = $(filter %.cpp, $(_source))
ifneq ($(cpp_source),)
    AS         = $(CXX)
    LINK       = $(CXX)
    AFLAGS     = $(CPPFLAGS)
endif
#
# The rules for outputing debug information, Do not modify them.
#
quiet_do_cc        = $(Q)$(ECHO)	"  CC       $(patsubst $(src)%,%,$<)" && $(CC)
quiet_do_cxx       = $(Q)$(ECHO)	"  CXX      $(patsubst $(src)%,%,$<)" && $(CXX)
quiet_do_as        = $(Q)$(ECHO)	"  AS       $(patsubst $(src)%,%,$<)" && $(AS)
quiet_do_ar        = $(Q)$(ECHO)	"  AR       $(patsubst $(obj)%,%,$@)" && $(AR)
quiet_do_ranlib    = $(Q)$(ECHO)	"  RANLIB   $(patsubst $(obj)%,%,$@)" && $(RANLIB)
quiet_do_copy      = $(Q)$(ECHO)	"  OBJCOPY  $(patsubst $(obj)%,%,$@)" && $(OBJCOPY)
quiet_do_link      = $(Q)$(ECHO)	"  LINK     $(patsubst $(obj)%,%,$@)" && $(LINK)
quiet_do_strip     = $(Q)$(ECHO)	"  STRIP    $(patsubst $(obj)%,%,$@)" && $(STRIP)
quiet_do_tag       = $(Q)$(ECHO)	"  APPTAG   $(patsubst $(obj)%,%,$@)" && $(TAG)
quiet_do_sign      = $(Q)$(ECHO)	"  SIGN     $(patsubst $(obj)%,%,$@)" && $(SIGN)
quiet_do_dep       = $(Q)$(ECHO)	"  MKDEP    $(patsubst $(src)%,%,$<)" &&
do_cc              = $(CC)
do_cxx             = $(CXX)
do_as              = $(AS)
do_ar              = $(AR)
do_ranlib          = $(RANLIB)
do_copy            = $(OBJCOPY)
do_link            = $(LINK)
do_strip           = $(STRIP)
do_tag             = $(TAG)
do_sign            = $(SIGN)
do_dep             =


#
# generating obj file
#
all: $(_all)
	@$(ECHO) '=========================================='
	@$(ECHO) '*                                        *'
	@$(ECHO) '*            COMPILE SUCCESS             *'
	@$(ECHO) '*                                        *'
	@$(ECHO) '=========================================='
ifeq ($(TYPE),library)
	$(LIST) $(LIBRARY)
else
	$(LIST) $(obj)
endif

info:
ifeq ($(KBUILD_VERBOSE), 0)
	@$(ECHO)	''
	@$(ECHO)	'-------------------------------------------------------------------------------'
	@$(ECHO)	'  AS         = $(AS)'
	@$(ECHO)	'  CC         = $(CC)'
	@$(ECHO)	'  CXX        = $(CXX)'
	@$(ECHO)	'  LINK       = $(LINK)'
	@$(ECHO)	'  OBJCOPY    = $(OBJCOPY)'
	@$(ECHO)	'  BIN_FLAGS  = $(BIN_FLAGS)'
	@$(ECHO)	'  SIGN_FLAGS = $(SIGN_FLAGS)'
	@$(ECHO)	'  TAG_FLAGS  = $(TAG_FLAGS)'
	@$(ECHO)	'  CFLAGS     = $(CFLAGS)'
	@$(ECHO)	'  CPPFLAGS   = $(CPPFLAGS)'
	@$(ECHO)	'  AFLAGS     = $(AFLAGS)'
	@$(ECHO)	'  ARFLAGS    = $(ARFLAGS)'
	@$(ECHO)	'  LDFLAGS    = $(LDFLAGS)'
	@$(ECHO)	'  LDLIBS     = $(LDLIBS)'
	@$(ECHO)	'-------------------------------------------------------------------------------'
	@$(ECHO)	''
endif

ifeq ($(TYPE),bootloader)
$(SIG_BIN): $(TAR_BIN)
else
ifeq ($(TYPE),boot)
$(SIG_BIN): $(TAR_BIN)
else
$(SIG_BIN): $(TAG_BIN)
endif
endif
	$($(quiet)do_sign) $(SIGN_FLAGS) -I $< -O $@

$(TAG_BIN) : $(TAR_BIN)
	$($(quiet)do_tag) $(TAG_FLAGS) -I $< -O $@

$(TAR_BIN): $(TAR_ELF)
	$($(quiet)do_copy) $(BIN_FLAGS) $< $@

$(TAR_ELF): $(object)
	$($(quiet)do_link) $(LDFLAGS) -o $@ $^ $(LDLIBS)
ifeq ($(TYPE),linux)
	$($(quiet)do_strip) $@
endif

$(LIBRARY): $(object)
	$($(quiet)do_ar) $(ARFLAGS) $@ $^
	$($(quiet)do_ranlib) $@

clean:
	$(DEL) $(object)
ifeq ($(TYPE),library)
	$(DEL) $(LIBRARY)
else
	$(DEL) $(TARGET)*
endif

distclean: clean
	$(DEL) -r $(obj)

#
# The rules for compiling  c file
#
$(obj)%.o: $(src)%.c
	$($(quiet)do_cc) $(CFLAGS) -o $@ -c $<

#
# The rules for compiling  cpp file
#
$(obj)%.o: $(src)%.cpp
	$($(quiet)do_cxx) $(CPPFLAGS) -o $@ -c $<

#
#  The rules for compiling  asm file
#
$(obj)%.o: $(src)%.s
	$($(quiet)do_as) $(AFLAGS) -o $@ -c $<

#
# The rule for generating depend relation for *.c and *.h
#
$(obj)%.c.dep : $(src)%.c
	@$(MKDIR) -p $(dir $@)
	$($(quiet)do_dep) $(CC) -M $(CFLAGS) -MQ $(patsubst %.c.dep,%.o,$@) $< >> $@

#
# The rule for generating depend relation for *.cpp and *.h
#
$(obj)%.cpp.dep : $(src)%.cpp
	@$(MKDIR) -p $(dir $@)
	$($(quiet)do_dep) $(CXX) -M $(CPPFLAGS) -MQ $(patsubst %.cpp.dep,%.o,$@) $< >> $@

#
# The rule for generating depend relation for *.s and *.h
#
$(obj)%.s.dep : $(src)%.s
	@$(MKDIR) -p $(dir $@)
	$($(quiet)do_dep) $(AS) -M $(AFLAGS) -MQ $(patsubst %.s.dep,%.o,$@) $< >> $@

sinclude ${depend}

