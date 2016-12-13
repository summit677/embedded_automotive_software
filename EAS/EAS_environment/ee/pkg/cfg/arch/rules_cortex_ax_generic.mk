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

## Author: 2013 Bruno Morelli

ifeq ($(call iseeopt, VERBOSE), yes)
VERBOSE = 1
endif

include $(EEBASE)/pkg/cfg/dir.mk
include $(PKGBASE)/cfg/verbose.mk
include $(PKGBASE)/cfg/compiler.mk

# The above part is needed for the base makefile for multicore building
# The part below containes rules for `all' and `clean', and will interfere
# with the rules that handle the per-cpu building process
ifeq ($(and $(call iseeopt, __MSRP__), $(__BASE_MAKEFILE__)), yes)
$(info "TODO: multi core makefile???")
include $(PKGBASE)/cfg/arch/rules_cortex_ax_multi_base.mk
else  # __MSRP__ and __BASE_MAKEFILE__

# Explicit the default target
.DEFAULT_GOAL := all

# Erika LIB name
EELIB    ?= ee
ERIKALIB  = lib$(EELIB).a

# Add application file to dependencies
ifneq ($(ONLY_LIBS), TRUE)
# OPT_LIBS is used to link additional libraries (e.g., for C++ support)
ifneq ($(call iseeopt, __BIN_DISTR), yes) 
# the EE library is built in the current directory
LIBDEP = $(ERIKALIB)
else # EEOPT __BIN_DISTR
# the EE library is stored in the EE lib directory
LIBDEP += $(EEBASE)/lib/$(ERIKALIB)
endif # EEOPT __BIN_DISTR
endif # ONLY_LIBS

# Specific option from the libs dependencies
LIBDEP += $(ALL_LIBS)

# Define the TARGET
TARGET_NAME ?= $(CORTEX_AX_MODEL)

# Add application file to dependencies
ifneq ($(call iseeopt, __BUILD_LIBS__), yes)
TARGET := $(TARGET_NAME).elf
endif

# Continue build system configuration
# If the environment tag ERIKA as FLAT, use the right configuration file
include $(PKGBASE)/cfg/cfg.mk

#
# --------------------------------------------------------------------------
#

##
## Common Rules
##

# Boot code containing _start should stay outside of the library in
# case of normal compilation
ifeq ($(call iseeopt, __BIN_DISTR), yes)
LIBSRCS += $(EE_BOOT_SRCS)
else
SRCS += $(EE_BOOT_SRCS)
endif

LIBEESRCS += $(EE_SRCS)
LIBEEOBJS += $(addprefix $(OBJDIR)/, $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(LIBEESRCS))))

LIBEESRCS += $(LIB_SRCS)
LIBOBJS += $(addprefix $(OBJDIR)/, $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(LIBSRCS))))

SRCS += $(APP_SRCS)
OBJS += $(addprefix $(OBJDIR)/, $(patsubst %.c,%.o,$(patsubst %.S,%.o, $(SRCS))))

# Variable used to import dependencies
ALLOBJS = $(LIBEEOBJS) $(LIBOBJS) $(OBJS) 

OBJDIRS=$(sort $(dir $(ALLOBJS)))

# Add basic include paths
INCLUDE_PATH += $(PKGBASE) $(APPBASE) .

vpath %.c $(EE_VPATH) $(APPBASE)
vpath %.S $(EE_VPATH) $(APPBASE)

## Select input filename format
SOURCEFILE = $(call native_path,$<)
TARGETFILE = $(call native_path,$@)
## source_c_file and target_c_file, must be defined inside compiler
## make
SOURCE_C_FILE = $(call source_c_file,$(SOURCEFILE))
TARGET_C_FILE = $(call target_c_file,$(TARGETFILE))
## source_c_file and target_c_file, must be defined inside compiler
## make
SOURCE_ASM_FILE = $(call source_asm_file,$(SOURCEFILE))
TARGET_ASM_FILE = $(call target_asm_file,$(TARGETFILE))
## target_ld_file, must be defined inside compiler
## make
TARGET_LD_FILE = $(call target_ld_file,$(TARGETFILE))

##
## Main rules: all clean
##

.PHONY: all clean

all: make_directories $(ALL_LIBS) $(TARGET)
# The success message is printed by the $(TARGET) rule, so we get a "Nothing
# do be done" message when everything is up to date

clean:
	@printf "CLEAN\n" ;
	@-rm -rf *.a *.ld *.map *.elf *.objdump obj *.men *.sh *.bin

#
# --------------------------------------------------------------------------
#

### Target file creation ###
$(TARGET_NAME).elf: $(OBJS) $(LIBDEP) $(LINKDEP)
	@echo "LD $@";
	$(QUIET)$(EE_LINK) $(OPT_LINK) $(TARGET_LD_FILE) --start-group $(OBJS) $(LIBDEP) $(EXT_LIBS) --end-group $(LINK_REDIRECT)
	@echo "************************************"
	@echo "Compilation terminated successfully!"

# produce the object file from assembly code in a single step
$(OBJDIR)/%.o: %.S $(OBJDEP)
	$(VERBOSE_PRINTASM) $(EE_ASM) $(DEFS_ASM) $(OPT_INCLUDE) $(OPT_ASM) $(DEPENDENCY_OPT_ASM) \
	$(TARGET_ASM_FILE) $(SOURCE_ASM_FILE)
	$(QUIET)$(call make-depend, $(subst .o,.d,$(@)))

# produce the object file from C code in a single step
$(OBJDIR)/%.o: %.c $(OBJDEP)
	$(VERBOSE_PRINTCC) $(EE_CC) $(DEFS_CC) $(OPT_INCLUDE) $(OPT_CC) $(DEPENDENCY_OPT) \
	$(TARGET_C_FILE) $(SOURCE_C_FILE)
	$(QUIET)$(call make-depend, $(subst .o,.d,$(@)))

##
## EE Library
##
$(ERIKALIB): $(LIBEEOBJS)
	@echo "AR $@"
	$(QUIET)rm -f $@
	$(QUIET)$(EE_AR) $(OPT_AR) $(ERIKALIB) $(LIBEEOBJS)

##
## Directories
##

.PHONY: make_directories

## Objects depend on directories, but they are not remade if directories change
$(ALLOBJS): | make_directories

make_directories: $(OBJDIRS)

## Directories are (re)created only when some of them don't exist already
$(OBJDIRS): 
	@echo "MAKE_DIRECTORIES"
	$(QUIET)mkdir -p $(OBJDIRS)

# the eeopt file is generated when dealing with ONLY_LIBS!
#
# this is a phony because the source code does not depend on this file
# and its content higly depends on the EEOPT variables...
#
.PHONY: generate_eeopt
generate_eeopt:
	@echo "GEN   eeopt.h"
	$(QUIET)for x in $(EEOPT); do \
		echo \#ifndef $${x}      >> eeopt.h; \
		echo \#define $${x}      >> eeopt.h; \
		echo \#endif             >> eeopt.h; \
	done;

##
## Automatic Generation of dependencies
##
dependencies=$(subst .o,.d,$(ALLOBJS))
ifneq ($(call iseeopt, NODEPS), yes) 
ifneq ($(MAKECMDGOALS),clean)
-include $(dependencies)
endif
endif

endif  # __MSRP__ and __BASE_MAKEFILE__
