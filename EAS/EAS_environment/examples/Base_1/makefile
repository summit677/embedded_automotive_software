# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
# 
# Copyright (C) 2002-2011  Evidence Srl
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

## Author: 2012, Errico Guidieri


# Destination directory
export OUTPUT_DIR = Output

# Check the environment and use reasonable defaults
ifndef ERIKA_FILES
export ERIKA_FILES := $(abspath ../../../EAS_environment/ee)
$(info using Erika in "ERIKA_FILES=$(ERIKA_FILES)")
else
$(warning "ERIKA_FILES=$(ERIKA_FILES)" is not defined)
endif

ifndef RTDRUID_SCRIPT_DIR
RTDRUID_SCRIPT_DIR := $(abspath ../../../EAS_environment/RT-Druid)
$(info using RT-Druid script in "RTDRUID_SCRIPT_DIR=$(RTDRUID_SCRIPT_DIR)")
else
$(warning "RTDRUID_SCRIPT_DIR=$(RTDRUID_SCRIPT_DIR)" is not defined)
endif

RTDRUID_OUTPUT = $(addprefix $(OUTPUT_DIR)/, makefile eecfg.h eecfg.c)

.PHONY: all clean config

all: $(RTDRUID_OUTPUT)	
	$(MAKE) -C $(shell cygpath -a '${OUTPUT_DIR}') all

clean:
	@echo cleaning...
	rm -f -r $(OUTPUT_DIR)

config: $(RTDRUID_OUTPUT)

$(RTDRUID_OUTPUT): conf.oil
	@echo "Running RT-Druid..."
	@ECLIPSE_HOME="$(RTDRUID_ECLIPSE_HOME)";\
	export ECLIPSE_HOME ;\
	$(RTDRUID_SCRIPT_DIR)/evidence/generate_code.sh $(RTDRUID_SCRIPT_DIR)\
		conf.oil $(OUTPUT_DIR)

