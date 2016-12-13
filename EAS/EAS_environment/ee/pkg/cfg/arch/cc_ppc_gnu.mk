# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
# 
# Copyright (C) 2002-2014  Evidence Srl
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
## Author: 2014,  Giuseppe Serano
##
## Based on:
## cc_tricore_gnu_new.mk
## Author: 2012,  Errico Guidieri
## Modified: 2012 Francesco Esposito (added Lin support)
##


##
## PPC_GNU GCC compiler version 4
## This file has been generated for HIGHTEC GCC v4.6.4.0
##

ifeq ($(call iseeopt, EE_FIND_COMPILER_IN_PATH), yes) 
## If this EE_OPT is used, the toolchain expects the compiler present in path
## This solution has been introduced because Lin does not like compiler paths
EE_LINK ?= powerpc-elf-gcc
EE_ASM  ?= powerpc-elf-gcc
EE_CC   ?= powerpc-elf-gcc
EE_AR   ?= powerpc-elf-ar
EE_CPP  ?= powerpc-elf-cpp
EE_OBJDUMP ?= powerpc-elf-objdump
else # EE_FIND_COMPILER_IN_PATH
## We will use "control program" gcc instead specific single tool,
## because it understand better implicit options (empirical knowledge)
## and because we can easily use HIGHTEC IDE to get the right options.
EE_LINK ?= $(PPC_GNU_BASEDIR)/bin/powerpc-elf-gcc
EE_ASM  ?= $(PPC_GNU_BASEDIR)/bin/powerpc-elf-gcc
EE_CC   ?= $(PPC_GNU_BASEDIR)/bin/powerpc-elf-gcc
EE_AR   ?= $(PPC_GNU_BASEDIR)/bin/powerpc-elf-ar
EE_CPP  ?= $(PPC_GNU_BASEDIR)/bin/powerpc-elf-cpp
EE_OBJDUMP ?= $(PPC_GNU_BASEDIR)/bin/powerpc-elf-objdump
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
# -fomit-frame-pointer  is needed to avoid that additional instructions are
#                       inserted before _START symbol
# -fno-tree-loop-optimize needed to work around to a compiler bug
# TODO: TO BE ADDED -Wextra
# TODO: use -msmall-pid to introduce support for small data addressing
OPT_CC +=	-c -mcpu=8540 -mno-eabi -mregnames -Wa,-mregnames	\
		-Wa,-mbooke32 -Wa,-me500 -misel=yes -mspe=yes		\
		-mfloat-gprs=no -msoft-float -mno-string -msdata=none	\
		-fno-common -Winline -finline-functions			\
		-fzero-initialized-in-bss -std=gnu99			\
		-fno-tree-loop-optimize -ffunction-sections		\
		-fdata-sections -Wdiv-by-zero -Wcast-align -Wformat	\
		-Wformat-security -Wignored-qualifiers -Wall

ifeq ($(call iseeopt, DEBUG), yes)
OPT_CC += -g3 -gdwarf-2 -O1
else # EE_DEBUG
OPT_CC += -O3
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
OPT_ASM +=	-c -mcpu=8540 -Wall -fshort-double -mno-eabi		\
		-mregnames -Wa,-mregnames -Wa,-mbooke32 -Wa,-me500	\
		-misel=yes -mspe=yes -mfloat-gprs=no -msoft-float	\
		-mno-string -msdata=none \

ifeq ($(call iseeopt, DEBUG), yes)
OPT_ASM += -g -Wa,-g -gdwarf-2 -Wa,-gdwarf-2
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

#ifneq ($(call iseeopt, EE_AS_OSAPPLICATIONS__), yes)
# apps.conf is not generated if there is no Memory Protection
#apps.conf:
#	@touch $@
#endif # EE_AS_OSAPPLICATIONS__ 

# Generate configuration files for multicore
ifeq ($(call iseeopt, __MSRP__), yes)
#ifeq ($(call iseeopt, EE_AS_OSAPPLICATIONS__), yes)
# Rule to generate MemMap.h File
#EG: Remember to re-add apps.conf for Memory Protection
#MemMap.h: apps.conf $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/MemMap.h.gnu.tmpl
MemMap.h: $(PKGBASE)/cpu/e200zx/cfg/MemMap.h.gnu.tmpl
# Preprocess MemMap.h.tmpl
	@echo GEN $@ from TEMPLATE $(word 2,$^)
	$(QUIET) awk -f $(PKGBASE)/cpu/common/cfg/memprot_generator.awk $^ > $@

# Add MemMap.h as dependency for OBJS
OBJDEP += MemMap.h
#endif # EE_AS_OSAPPLICATIONS__
endif # __MSRP__

##
## Linker scripts templates handling rules
##
##
## Specific linker option from the application makefile
##
# Custom Linker Script Should be Provided with EE_LINK_SCRIPT variable
ifndef EE_LINKERSCRIPT
# Linker script start folder
ifeq ($(call iseeopt, __MPC5674F__), yes)
PPC_MODEL = freescale_mpc5674f
else
$(error Unsupported PPC Model!)
endif

EE_LINKERSCRIPT_PPC_FOLDER = $(PKGBASE)/cpu/e200zx/cfg
EE_LINKERSCRIPT_MODEL_FOLDER = $(PKGBASE)/mcu/$(PPC_MODEL)/cfg

ifneq ($(call iseeopt, __MSRP__), yes)
EE_LINKERMEMORY	:= $(EE_LINKERSCRIPT_MODEL_FOLDER)/memory_gnu.ld
else #!__MSRP__
ifeq ($(CPU_NUMID),0)
EE_LINKERMEMORY	:= $(EE_LINKERSCRIPT_MODEL_FOLDER)/multicore/memory0_gnu.ld
else # CPU_NUMID eq 0
EE_LINKERMEMORY	:= $(EE_LINKERSCRIPT_MODEL_FOLDER)/multicore/memory1_gnu.ld
endif # CPU_NUMID eq 0
endif#!__MSRP__

# Use The Erika Default One
ifeq ($(call iseeopt, __E200ZX_EXECUTE_FROM_RAM__), yes)

ifneq ($(call iseeopt, __MSRP__), yes)
EE_LINKERSCRIPT	:= ram_gnu.ld
EE_LINKERSCRIPT_TEMPLATE := $(EE_LINKERSCRIPT_PPC_FOLDER)/ram_gnu.ld
else #!__MSRP__
ifeq ($(CPU_NUMID),0)
EE_LINKERSCRIPT	:= ram0_gnu.ld
EE_LINKERSCRIPT_TEMPLATE := $(EE_LINKERSCRIPT_PPC_FOLDER)/multicore/ram0_gnu.ld
else # CPU_NUMID eq 0
EE_LINKERSCRIPT	:= ram1_gnu.ld
EE_LINKERSCRIPT_TEMPLATE := $(EE_LINKERSCRIPT_PPC_FOLDER)/multicore/ram1_gnu.ld
endif # CPU_NUMID eq 0
endif #!__MSRP__

else # EE_EXECUTE_FROM_RAM

ifneq ($(call iseeopt, __MSRP__), yes)
EE_LINKERSCRIPT	:= flash_gnu.ld
EE_LINKERSCRIPT_TEMPLATE := $(EE_LINKERSCRIPT_PPC_FOLDER)/rom_gnu.ld
else #!__MSRP__
ifeq ($(CPU_NUMID),0)
EE_LINKERSCRIPT	:= flash0_gnu.ld
EE_LINKERSCRIPT_TEMPLATE := $(EE_LINKERSCRIPT_PPC_FOLDER)/multicore/rom0_gnu.ld
else # CPU_NUMID eq 0
EE_LINKERSCRIPT	:= flash1_gnu.ld
EE_LINKERSCRIPT_TEMPLATE := $(EE_LINKERSCRIPT_PPC_FOLDER)/multicore/rom1_gnu.ld
endif # CPU_NUMID eq 0
endif # !__MSRP__

endif # EE_EXECUTE_FROM_RAM

# The default linker script is made by a memory description part and a rules part
# (Select the right memory part in case of multicore build)
ifeq ($(call iseeopt, __MSRP__), yes)
# Slave CPUs: add linkerscript with global shared symbols (No more needed)
ifneq ($(CPU_NUMID), 0)
ADDITIONAL_LINKSCRIPT = $(GLOBAL_LINKSCRIPT)
endif # CPU_NUMID not eq 0
endif # __MSRP__

# If shared symbol container in slaves multicore build is defined, it has to be
# concatenated with linker memory descriptions and linker rules to obtain the
# complete linker script
#EG: Remember to re-add apps.conf for Memory Protection
#$(EE_LINKERSCRIPT): apps.conf $(EE_LINKERMEMORY) $(EE_LINKERSCRIPT_TEMPLATE)
$(EE_LINKERSCRIPT): $(EE_LINKERMEMORY) $(EE_LINKERSCRIPT_TEMPLATE) $(ADDITIONAL_LINKSCRIPT)
	@echo GEN   $@
	$(QUIET) awk -f $(PKGBASE)/cpu/common/cfg/memprot_generator.awk $^ > $@
else # !EE_LINKERSCRIPT

# Slave CPUs: queue linkerscript with global shared symbols (No more needed)
ifeq ($(call iseeopt, __MSRP__), yes)
ifneq ($(CPU_NUMID), 0)
ADDITIONAL_LINKSCRIPT = $(GLOBAL_LINKSCRIPT)
endif # CPU_NUMID not eq 0
endif # __MSRP__

# Copy EE_LINKERSCRIPT in building folder eventually enqueuing shared symbols
# informations
$(EE_LINKERSCRIPT) : $(APPBASE)/$(EE_LINKERSCRIPT) $(ADDITIONAL_LINKSCRIPT)
	@echo GEN   $@
	$(QUIET) rm -f $@
	$(QUIET) cat $^ >> $@
endif # !EE_LINKERSCRIPT

# If default compiler behaviour is chosen do not add linkerscript to dependencies and
# do not add it to linker options. Moreover do not disable startup code linking.
ifneq ($(call iseeopt, EE_COMPILER_DEFAULT),yes)

# Add Linker Script to Link Dependencies
LINKDEP += $(EE_LINKERSCRIPT)

OPT_LINK += -Wl,-T$(EE_LINKERSCRIPT)
endif # EE_COMPILER_DEFAULT

#Normal build
OPT_LINK +=	-mcpu=8540 -nostartfiles -nostdlib -Wl,--gc-sections	\
		-Wl,-Map="$(MAP_FILE)" -Wl,--cref -Wl,--no-warn-mismatch

ifeq ($(call iseeopt, DEBUG), yes)
OPT_LINK += -Wl,-g
endif

# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_LINK += $(LDFLAGS)

#Functions to be used to wrap with right options target for Linker:
target_ld_file=$(addprefix -o ,$1)

# Options for Archiver
OPT_AR += -r -c -s

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
#make-depend = sed -e 's_\\\(.\)_/\1_g' -e 's_\<\([a-zA-Z]\):/_/cygdrive/\l\1/_g' < $1_tmp > $1 && rm $1_tmp
make-depend = sed -e 's_\\\(.\)_/\1_g' -e 's_\<\([a-zA-Z]\):/_/cygdrive/\l\1/_g' < $3_tmp > $3 && rm $3_tmp
endif # NODEPS

EE_SRCS += $(EE_CASM_SRCS)
NEED_ASM_TO_C_TRANSLATION = 0
