# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
#
# Copyright (C) 2002-2013  Evidence Srl
#
# This file is part of ERIKA Enterprise.
#
# ERIKA Enterprise is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation,
# (with a special exception described below).
#
# Linking this code statically or dynamically with other modules is
# making a combined work based on this code.  Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.
#
# As a special exception, the copyright holders of this library give you
# permission to link this code with independent modules to produce an
# executable, regardless of the license terms of these independent
# modules, and to copy and distribute the resulting executable under
# terms of your choice, provided that you also meet, for each linked
# independent module, the terms and conditions of the license of that
# module.  An independent module is a module which is not derived from
# or based on this library.  If you modify this code, you may extend
# this exception to your version of the code, but you are not
# obligated to do so.  If you do not wish to do so, delete this
# exception statement from your version.
#
# ERIKA Enterprise is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License version 2 for more details.
#
# You should have received a copy of the GNU General Public License
# version 2 along with ERIKA Enterprise; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA.
# ###*E*###



##
## CORTEX_AX_GNU GCC compiler version 4
## This file has been generated for HIGHTEC GCC v4.6.1.2
##

ifeq ($(call iseeopt, EE_FIND_COMPILER_IN_PATH), yes)
## If this EE_OPT is used, the toolchain expects the compiler present in path
## This solution has been introduced because Lin does not like compiler paths
EE_LINK ?= arm-linux-gnueabihf-ld
EE_ASM  ?= arm-linux-gnueabihf-gcc
EE_CC   ?= arm-linux-gnueabihf-gcc
EE_AR   ?= arm-linux-gnueabihf-ar
EE_OBJDUMP ?= arm-linux-gnueabihf-objdump
EE_OBJCOPY ?= arm-linux-gnueabihf-objcopy
else # EE_FIND_COMPILER_IN_PATH
## We will use "control program" gcc instead specific single tool,
## because it understand better implicit options (empirical knowledge)
## and because we can easily use HIGHTEC IDE to get the right options.
EE_LINK ?= $(CORTEX_AX_XENPV_GCCDIR)/bin/arm-linux-gnueabihf-ld
EE_ASM  ?= $(CORTEX_AX_XENPV_GCCDIR)/bin/arm-linux-gnueabihf-gcc
EE_CC   ?= $(CORTEX_AX_XENPV_GCCDIR)/bin/arm-linux-gnueabihf-gcc
EE_AR   ?= $(CORTEX_AX_XENPV_GCCDIR)/bin/arm-linux-gnueabihf-ar
EE_OBJDUMP ?= $(CORTEX_AX_XENPV_GCCDIR)/bin/arm-linux-gnueabihf-objdump
EE_OBJCOPY ?= $(CORTEX_AX_XENPV_GCCDIR)/bin/arm-linux-gnueabihf-objcopy
endif # EE_FIND_COMPILER_IN_PATH

# Suffix used for Linker Scripts Files for TASKING
CC_LD_SUFFIX := ld

ifneq ($(ONLY_LIBS), TRUE)
# OPT_LIBS is used to link additional libraries (e.g., for C++ support)
ifneq ($(call iseeopt, __BIN_DISTR), yes)
OPT_LIBS +=
else  # EEOPT __BIN_DISTR
OPT_LIBS +=
endif # EEOPT __BIN_DISTR
endif # ONLY_LIBS

# INCLUDE_PATH is a colon separated list of directories for source file searching
# -I = adds directories to the source file search path (for both gcc and gas)
# we consider the ee pkg directory and the application dir
# we also consider the current directory because the app could be compiled
# from the config files generated from eclipse...
OPT_INCLUDE = $(foreach d,$(INCLUDE_PATH),$(addprefix -I,$(call native_path,$d)))

## OPT_CC are the options for compiler invocation
# -fomit-frame-pointer is neeeded to avoid that additional instructions are inserted before _START symbol
OPT_CC += -c -mcpu=$(GNUC_CORTEX_AX_XENPV_MODEL) -D__ARM_PCS_VFP -D__XEN_INTERFACE_VERSION__=0x0003020a -Wall -ffunction-sections -fomit-frame-pointer -Winline -finline-functions\
 -fzero-initialized-in-bss -std=gnu99

ifeq ($(call iseeopt, EE_DEBUG), yes)
OPT_CC += -g3 -ggdb -O1
else # EE_DEBUG
OPT_CC += -g -O3
endif # EE_DEBUG

ifeq ($(call iseeopt, EE_SAVE_TEMP_FILES), yes)
OPT_CC += -save-temps=obj
endif

# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_CC += $(CFLAGS)

#Functions to be used to select wrap with right options source and target for Compiler:
#Nothing special to do with source file
source_c_file=$(strip $1)
##--create stop  "control program" after object file is created
target_c_file=$(addprefix -o ,$1)


##
## OPT_ASM are the options for asm invocation
##
OPT_ASM += -c -mcpu=$(GNUC_CORTEX_AX_XENPV_MODEL) -Wall

ifeq ($(call iseeopt, EE_DEBUG), yes)
OPT_ASM += -Wa,--gdwarf2
endif # EE_DEBUG


# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_ASM += $(ASFLAGS)

#Functions to be used to wrap with right options source and target for Assembler:
#Nothing special to do with source file
source_asm_file=$(strip $1)
##--create stop  "control program" after object file is created
target_asm_file=$(addprefix -o ,$1)

##
## Templates handling target and rules
##

ifneq ($(call iseeopt, EE_AS_OSAPPLICATIONS__), yes)
# apps.conf is not generated if there is no Memory Protection
apps.conf:
	@touch $@
endif # EE_AS_OSAPPLICATIONS__

##
## Specific linker option from the application makefile
##

# Custom Linker Script Should be Provided with EE_LINK_SCRIPT variable
#
ifndef EE_LINKERSCRIPT_FOLDER
EE_LINKERSCRIPT_FOLDER = $(PKGBASE)/board/iCoreM6x/cfg
EE_LINKERSCRIPT := ee_cortex_ax_gnu_ram.ld
else
EE_LINKERSCRIPT_FOLDER = $(APPBASE)
endif

# Copy EE_LINKERSCRIPT in building folder eventually enqueuing shared symbols
# informations
$(EE_LINKERSCRIPT) : $(APPBASE)/$(EE_LINKERSCRIPT) $(ADDITIONAL_LINKSCRIPT)
	@echo GEN $@
	$(QUIET) rm -f $@
	$(QUIET) cat $^ >> $@

OPT_LINK += --gc-sections\
 -Map="$(basename $(notdir $@)).map" --cref

ifeq ($(call iseeopt, EE_DEBUG), yes)
OPT_LINK +=
endif

# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_LINK += $(LDFLAGS)

#Functions to be used to wrap with right options target for Linker:
target_ld_file=$(addprefix -o ,$1)

# Options for Archiver
OPT_AR += -r -c

# Defining EEOPT Macros
# Each identifier that is listed in EEOPT is also inserted as a
# command-line macro in the compiler...
DEFS_ASM = $(addprefix -D, $(EEOPT) )
DEFS_CC  = $(addprefix -D, $(EEOPT) )

# Automatic dependency generation
ifeq ($(call iseeopt, NODEPS), yes)
DEPENDENCY_OPT =
make-depend =
else # NODEPS
# Create dependency for all headers, and add a target for each header
DEPENDENCY_OPT = -MD -MF $(call native_path,$(subst .o,.d_tmp,$(@)))
# Create dependency for all headers, and add a target for each header
DEPENDENCY_OPT_ASM = -MD -MF $(call native_path,$(subst .o,.d_tmp,$(@)))
# Dependencies on Windows need path translation and quote remotion
make-depend =
#make-depend = sed -e 's_\\\(.\)_/\1_g' -e 's_\<\([a-zA-Z]\):/_/cygdrive/\l\1/_g' < $1_tmp > $1 && rm $1_tmp
endif # NODEPS
