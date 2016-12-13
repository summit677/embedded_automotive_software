# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
# 
# Copyright (C) 2002-2012  Evidence Srl
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
## Author: 2012,  Errico Guidieri
## Edited: 2014,  Christoph Kreuzberger
##


##
## TASKING TriCore Compiler support
## This file has been generated for TASKING 4.0r1. 
## You can find TASKING documentation pdf file for compiler at:
## http://www.tasking.com/support/tricore/tc_user_guide_v4.0.pdf
## In following comments, sometime, paragraph reference of this document
## are added.


ifeq ($(call iseeopt, EE_FIND_COMPILER_IN_PATH), yes) 
## If this EE_OPT is used, the toolchain expects the compiler present in path
## This solution has been introduced because Lin does not like compiler paths
EE_LINK ?= cctc.exe
EE_ASM  ?= cctc.exe
EE_CC   ?= cctc.exe
EE_AR   ?= artc.exe
EE_OBJDUMP ?= hldumptc.exe
else # EE_FIND_COMPILER_IN_PATH

## We will use "control program" cctc instead specific single tool,
## because it understand better implicit options (empirical knowledge)
## and because we can easily use TASKING IDE to get the right options.
EE_LINK ?= $(call unix_path,$(TRICORE_TASKINGDIR)/ctc/bin/cctc.exe)
EE_ASM  ?= $(call unix_path,$(TRICORE_TASKINGDIR)/ctc/bin/cctc.exe)
EE_CC   ?= $(call unix_path,$(TRICORE_TASKINGDIR)/ctc/bin/cctc.exe)
EE_AR   ?= $(call unix_path,$(TRICORE_TASKINGDIR)/ctc/bin/artc.exe)
EE_OBJDUMP ?= $(call unix_path,$(TRICORE_TASKINGDIR)/ctc/bin/hldumptc.exe)
endif # EE_FIND_COMPILER_IN_PATH

# WORK AROUND TO PERSUADE LINKER TO PRESERVE SECTIONS IN SLAVE ELFs
ifeq ($(call iseeopt, __MSRP__), yes)
ifneq  ($(call iseeopt, EE_BUILD_SINGLE_ELF), yes)
ifneq ($(call iseeopt, EE_USE_CUSTOM_STARTUP_CODE), yes)
ifneq ($(CPU_NUMID), 0)
#OBJS += ../$(CPU_MASTER_DIR)/$(OBJDIR)/pkg/mcu/infineon_common_tc2Yx/src/ee_tc2Yx_cstart.o
endif # CPU_NUMID not eq 0
endif # !EE_USE_CUSTOM_STARTUP_CODE
endif # !EE_BUILD_SINGLE_ELF
endif # __MSRP__

# Suffix used for Linker Scripts Files for TASKING
CC_LD_SUFFIX := lsl

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

#Defined when multicore support is needed, used to preprocess Multicore Linker Script
ifneq ($(call iseeopt, __MSRP__), yes)
OPT_CC += --lsl-core=tc0
endif # !__MSRP__

## OPT_CC are the options for compiler invocation
ifeq ($(call iseeopt, EE_MM_OPT), yes)

else # EE_MM_OPT


ifeq ($(call iseeopt, EE_DEBUG), yes)
OPT_CC += --emit-locals=+equ,+symbols -g -O1
else # EE_DEBUG
OPT_CC += --emit-locals=-equ,-symbols -O3
endif # EE_DEBUG

endif # EE_MM_OPT

## for the meaning of the options look at documentation paragraph 
## 11.6. Control Program Options
OPT_CC += -C$(TRICORE_MODEL) -t -Wa-gAHLs -Wa-Ogs -Wa--error-limit=42 --no-preprocessing-only -E+comments,+noline \
--iso=99 --language=+gcc,-volatile,+strings --switch=auto --align=0 \
--default-near-size=8 --default-a0-size=0 --default-a1-size=0 --tradeoff=4 --compact-max-size=200 --source

#OPT_CC += -Wc--align=4

# TODO: Find -save-temps=obj equivalent
ifeq ($(call iseeopt, EE_SAVE_TEMP_FILES), yes)
OPT_CC += 
endif

## MISRA-C CHECK Options. This configuration enable required rules only as warning.
ifeq ($(call iseeopt, EE_MISRA_C_CHECK_ON),yes)
OPT_CC += --misrac="1.1,1.2,2.1,2.2,2.3,3.4,3.5,4.1,4.2,5.1,5.2,5.3,5.4,6.1,6.2,6.4,6.5,\
7.1,8.1,8.2,8.3,8.4,8.5,8.6,8.7,8.8,8.9,8.11,8.12,9.1,9.2,9.3,10.1,10.2,10.3,10.4,10.5,10.6,\
11.1,11.2,11.5,12.2,12.3,12.4,12.5,12.7,12.8,12.9,12.10,12.12,13.1,13.3,13.4,13.5,13.6,13.7,\
14.1,14.2,14.3,14.4,14.5,14.6,14.7,14.8,14.9,14.10,15.1,15.2,15.3,15.4,15.5,\
16.1,16.2,16.3,16.4,16.5,16.6,16.8,16.9,16.10,17.3,17.4,17.6,18.1,18.2,18.4,\
19.3,19.4,19.5,19.6,19.8,19.9,19.10,19.11,19.12,19.14,19.15,19.16,19.17,\
20.1,20.2,20.4,20.5,20.6,20.7,20.8,20.9,20.10,20.11,20.12"
OPT_CC += --misrac-version=2004 --misrac-required-warnings --misrac-advisory-warnings 
endif

# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_CC += $(CFLAGS)

# OPT_CC += -Wc--core=tc1.6.x -Wc-D_CTRI -Wc--no-double -Wc--iso=99	\
			# -Wc--language=-gcc,-strings -Wc--switch=auto -Wc--align=4 -Wc--default-near-size=0 		\
			# -Wc--default-a0-size=8 -Wc--default-a1-size=8 -Wc-ONRpfceogvIlywakmsU 			\
			# -Wc--tradeoff=2 -Wc--compact-max-size=200 -Wc--max-call-depth=-1 -Wc--inline-max-incr=35\
			# -Wc--inline-max-size=10 -Wc-g -Wc--misrac-version=2004 -Wc--immediate-in-code

#Functions to be used to select wrap with right options source and target for Compiler:
#Nothing special to do with source file
source_c_file=$(strip $1)
##--create stop  "control program" after object file is created
target_c_file=$(addprefix --create --output=,$1)


##
## OPT_ASM are the options for asm invocation
##
ifeq ($(call iseeopt, EE_MM_OPT), yes)

else # EE_MM_OPT
#For ASM I leave --tasking-sfr option active
OPT_ASM += -C$(TRICORE_MODEL) -t -Wa--tasking-sfr -Wa-gAHLs -Wa-Ogs -Wa--error-limit=42
endif # EE_MM_OPT

#Defined when multicore support is needed, used to preprocess Multicore Linker Script
ifneq ($(call iseeopt, __MSRP__), yes)
OPT_ASM += --lsl-core=tc0
endif # !__MSRP__

ifeq ($(call iseeopt, EE_DEBUG), yes)
OPT_ASM += --emit-locals=+equ,+symbols
else
OPT_ASM += --emit-locals=-equ,-symbols
endif # EE_DEBUG


# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_ASM += $(ASFLAGS)
# OPT_ASM += -Wa-il --optimize=gs

#Functions to be used to wrap with right options source and target for Assembler:
#Nothing special to do with source file
source_asm_file=$(strip $1)
##--create stop  "control program" after object file is created
target_asm_file=$(addprefix --create --output=,$1)

ifneq ($(call iseeopt, EE_AS_OSAPPLICATIONS__), yes)
# apps.conf is not generated if there is no Memory Protection
apps.conf:
	@touch $@
endif # EE_AS_OSAPPLICATIONS__ 

# Generate configuration files for Os-Applications
ifeq ($(call iseeopt, EE_AS_OSAPPLICATIONS__), yes)
# Rule to generate MemMap.h File
MemMap.h: apps.conf $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/MemMap.h.tasking.tmpl
# Preprocess MemMap.h.tmpl
	@echo GEN $@ from TEMPLATE $(word 2,$^)
	$(QUIET) awk -f $(PKGBASE)/cpu/common/cfg/memprot_generator.awk $^ > $@

# Add MemMap.h as dependency for OBJS
OBJDEP += MemMap.h
endif # EE_AS_OSAPPLICATIONS__

##
## Linker scripts templates handling rules
##
ifeq ($(and $(call iseeopt, __MSRP__), $(call iseeopt, EE_BUILD_SINGLE_ELF)), yes)
EE_LINKERSCRIPT_MODEL_FOLDER = $(PKGBASE)/mcu/infineon_$(TRICORE_MODEL)/cfg

ifeq ($(call iseeopt, EE_EXECUTE_FROM_RAM), yes)
EE_LINKERSCRIPT_PREFIX := ram
else # EE_EXECUTE_FROM_RAM
EE_LINKERSCRIPT_PREFIX := flash
endif # EE_EXECUTE_FROM_RAM

## Handle Global Linking in multicore environment with single ELF
ifeq ($(__BASE_MAKEFILE__), yes)

EE_LINKERSCRIPT := ee_$(TRICORE_MODEL)_tasking_$(EE_LINKERSCRIPT_PREFIX)_recollect.ld
EE_LINKERSCRIPT_TEMPLATE := $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/multicore/ee_tc2Yx_tasking_$(EE_LINKERSCRIPT_PREFIX)_recollect.ld.tmpl
EE_LINKERMEMORY += $(EE_LINKERSCRIPT_MODEL_FOLDER)/ee_$(TRICORE_MODEL)_tasking_memory_map.x

EE_RECOLLECT_LINKERSCRIPTS += $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/multicore/ee_tc2Yx_tasking_$(EE_LINKERSCRIPT_PREFIX)_startup_recollect.ld
EE_RECOLLECT_LINKERSCRIPTS += $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/multicore/ee_tc2Yx_tasking_$(EE_LINKERSCRIPT_PREFIX)_recollect_prefix.ld
EE_RECOLLECT_LINKERSCRIPTS += $(addsuffix ee_$(TRICORE_MODEL)_tasking_flash_recollect.ld.frag, $(EE_CORE_DIRS))
EE_RECOLLECT_LINKERSCRIPTS += $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/multicore/ee_tc2Yx_tasking_$(EE_LINKERSCRIPT_PREFIX)_recollect_suffix.ld

$(EE_LINKERSCRIPT): $(EE_LINKERMEMORY) $(EE_RECOLLECT_LINKERSCRIPTS)
	@echo GEN $@ from $(EE_LINKERSCRIPT_TEMPLATE)
	$(QUIET) cat $^ > $@

# Add Linker Script to Link Dependencies
LINKDEP += $(EE_LINKERSCRIPT)
else # __BASE_MAKEFILE__
EE_LINKERSCRIPT       := ee_$(TRICORE_MODEL)_tasking_relocable.ld
EE_LINKERSCRIPT_TEMPLATE := $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/multicore/ee_tc2Yx_tasking_relocable.ld.tmpl

EE_PARTIAL_RECOLLECT_LINKERSCRIPT := ee_$(TRICORE_MODEL)_tasking_flash_recollect.ld.frag
EE_PARTIAL_RECOLLECT_LINKERSCRIPT_TEMPLATE += $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/multicore/ee_tc2Yx_tasking_$(EE_LINKERSCRIPT_PREFIX)_recollect.ld.tmpl

$(EE_LINKERSCRIPT): apps.conf $(EE_LINKERSCRIPT_TEMPLATE)
	@echo GEN $@ from $(EE_LINKERSCRIPT_TEMPLATE)
	$(QUIET) awk -f $(PKGBASE)/cpu/common/cfg/memprot_generator.awk $^ > $@

$(EE_PARTIAL_RECOLLECT_LINKERSCRIPT): apps.conf $(EE_PARTIAL_RECOLLECT_LINKERSCRIPT_TEMPLATE)
	@echo GEN $@ from $(EE_PARTIAL_RECOLLECT_LINKERSCRIPT_TEMPLATE)
	$(QUIET) echo "" > $@;
	$(QUIET) echo "CORE_ID = CPU$(CPU_NUMID);" >> $@ ;
	$(QUIET) awk -f $(PKGBASE)/cpu/common/cfg/memprot_generator.awk $^ >> $@

#$(EE_PARTIAL_RECOLLECT_LINKERSCRIPT): $(EE_PARTIAL_RECOLLECT_LINKERSCRIPT).tbs
#	@echo GEN $@ from $<
#	$(QUIET) sed -e 's-#c#-$(CPU_NUMID)-g' $< > $@

# Add Linker Script to Link Dependencies
LINKDEP += $(EE_LINKERSCRIPT) $(EE_PARTIAL_RECOLLECT_LINKERSCRIPT)
endif # __BASE_MAKEFILE__
else # __MSRP__ && EE_BUILD_SINGLE_ELF
##
## Specific linker option from the application makefile
##
# Custom Linker Script Should be Provided with EE_LINK_SCRIPT variable
ifndef EE_LINKERSCRIPT
# Linker script start folder
EE_LINKERSCRIPT_MODEL_FOLDER = $(PKGBASE)/mcu/infineon_$(TRICORE_MODEL)/cfg

# Use The Erika Default One
ifeq ($(call iseeopt, EE_EXECUTE_FROM_RAM), yes)

EE_LINKERSCRIPT := ee_tc27x_tasking_ram.lsl

else # EE_EXECUTE_FROM_RAM



ifneq ($(call iseeopt, __MSRP__), yes)
EE_LINKERSCRIPT := ee_tc27x_tasking_4.3_flash.lsl
else #!__MSRP__
EE_LINKERSCRIPT := ee_tc27x_tasking_4.3_mc_flash.lsl
endif # !__MSRP__

endif # EE_EXECUTE_FROM_RAM

# The default linker script is made by a memory description part and a rules part
# (Select the right memory part in case of multicore build)
ifeq ($(call iseeopt, __MSRP__), yes)
EE_LINKERMEMORY += $(EE_LINKERSCRIPT_MODEL_FOLDER)/multicore/ee_$(TRICORE_MODEL)_tasking_memory_core$(CPU_NUMID).x
# Slave CPUs: add linkerscript with global shared symbols (No more needed)
ifneq ($(CPU_NUMID), 0)
ADDITIONAL_LINKSCRIPT = $(GLOBAL_LINKSCRIPT)
endif # CPU_NUMID not eq 0
else # __MSRP__

EE_LINKERMEMORY += $(EE_LINKERSCRIPT_MODEL_FOLDER)/ee_$(TRICORE_MODEL)_tasking_memory_singlecore.x
endif # __MSRP__

# If shared symbol container in slaves multicore build is defined, it has to be
# concatenated with linker memory descriptions and linker rules to obtain the
# complete linker script
$(EE_LINKERSCRIPT) : % : $(PKGBASE)/mcu/infineon_$(TRICORE_MODEL)/cfg/%
	@echo CP $@
	$(QUIET) cp $< $@
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
	@echo GEN $@
	$(QUIET) rm -f $@
	$(QUIET) cat $^ >> $@
endif # !EE_LINKERSCRIPT
endif # __MSRP__ && EE_BUILD_SINGLE_ELF

# If default compiler behaviour is chosen do not add linkerscript to dependencies and
# do not add it to linker options. Moreover do not disable startup code linking.
ifneq ($(call iseeopt, EE_COMPILER_DEFAULT),yes)

# Add Linker Script to Link Dependencies
LINKDEP += $(EE_LINKERSCRIPT)

endif # EE_COMPILER_DEFAULT

## Handle relocating code if needed
ifeq ($(and $(call iseeopt, __MSRP__), $(call iseeopt, EE_BUILD_SINGLE_ELF)), yes)
ifneq ($(__BASE_MAKEFILE__), yes)
##Use always the export file to MASK replicated symbols
EXPORT_FILE := export_ee_global
## Create the right path to reach EE_USER_EXPORT_FILE
ifneq ($(EE_USER_EXPORT_FILE),)
EE_USER_EXPORT_FILE := $(APPBASE)/$(EE_USER_EXPORT_FILE)
endif # !EE_USER_EXPORT_FILE

ifeq ($(CPU_NUMID),0)
$(EXPORT_FILE): $(PKGBASE)/mcu/infineon_common_tc2Yx/cfg/multicore/$(EXPORT_FILE) $(EE_USER_EXPORT_FILE)
	@echo GEN symbols export file $@ from $^
	$(QUIET)cat $^ > $@
else # CPU_NUMID eq 0
$(EXPORT_FILE): $(EE_USER_EXPORT_FILE)
ifneq ($(EE_USER_EXPORT_FILE),)
	@echo GEN symbols export file $@ from $^
endif # !EE_USER_EXPORT_FILE
	$(QUIET)echo "EXPORT FUNCTION EE_tc2Yx_cpu$(CPU_NUMID)_start ;" > $@
ifneq ($(EE_USER_EXPORT_FILE),)
	$(QUIET)cat $^ >> $@
endif # !EE_USER_EXPORT_FILE
endif  # CPU_NUMID eq 0

##Put the EXPORT_FILE into linking dependencies
LINKDEP += $(EXPORT_FILE)

##Relocatable linking in case of multicore
OPT_LINK += 

else #  !__BASE_MAKEFILE__
#Recollection in Single elf build
OPT_LINK +=


ifeq ($(call iseeopt, EE_DEBUG), yes)
OPT_LINK +=
endif

# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_LINK += $(LDFLAGS)
endif # !__BASE_MAKEFILE__

else # __MSRP__ && EE_BUILD_SINGLE_ELF
#Normal build
OPT_LINK += -C$(TRICORE_MODEL)

#Defined when multicore support is needed, used to preprocess Multicore Linker Script
ifeq ($(call iseeopt, __MSRP__), yes)
OPT_LINK += -Wl-DCPU_NUMID=$(CPU_NUMID)
else
OPT_LINK += --lsl-core=tc0
endif

OPT_LINK += -t $(EE_LINKERSCRIPT) -Wl-O1 -Wl--map-file=$(TARGET_NAME).mapxml:XML -Wl-m2 -Wl--error-limit=42 -Wl--no-warnings=159 -g

#no warning is needed for following warning message in TC0:
#ltc W159: LSL: section ".text.libc.reset" (function _START) was not selected because it already has an absolute restriction

ifeq ($(call iseeopt, EE_DEBUG), yes)
OPT_LINK +=
endif

# Option to execute code and locate data structures in RAM 
ifeq ($(call iseeopt, EE_EXECUTE_FROM_RAM), yes)
#-Wl--non-romable SHOULD BE ADDED to the options but it doens't work
#(conflict with INTTAB & TRAPTAB) -> TASKING BUG
#OPT_LINK += --no-rom-copy -Wl-DEE_EXECUTE_FROM_RAM
OPT_LINK += -Wl--non-romable -Wl-DEE_EXECUTE_FROM_RAM
endif

# Specific option from the application makefile
# Add those flags at the end should be enough to give to user the option to
# completly customize build process (because last options will ovveride previous ones).
OPT_LINK += $(LDFLAGS) -Wl--lsl-dump
endif # __MSRP__ && EE_BUILD_SINGLE_ELF

#Functions to be used to wrap with right options target for Linker:
target_ld_file=$(addprefix -o ,$1)

# Options for Archiver
OPT_AR += -r -c

# Option for OBJDUMP Command (the following is the default and it's ok, added just to remember that)
OPT_OBJDUMP += --dump-format=dhmsy

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
DEPENDENCY_OPT = --dep-file=$(call native_path,$(subst .o,.d_tmp,$(@))) -Wc--make-target=$(@)
# Create dependency for all headers, and add a target for each header
DEPENDENCY_OPT_ASM = -Wa--dep-file=$(call native_path,$(subst .o,.d_tmp,$(@))) -Wa--make-target=$(@)
# Dependencies on Windows need path translation and quote remotion
make-depend = sed -e 's_"__g' -e 's_\\\(.\)_/\1_g' -e 's_\<\([a-zA-Z]\):/_/cygdrive/\l\1/_g' < $1_tmp > $1 && rm $1_tmp
endif # NODEPS

