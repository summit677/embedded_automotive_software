# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
# 
# Copyright (C) 2011 Steve Langstaff
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

## Author: 2014 Christoph Kreuzberger
## Modified: 2015 Fabian Mauroner


ifeq ($(call iseeopt, EE_APPKIT_TC2X7), yes)
INCLUDE_PATH += $(call short_native_path, $(EEBASE)/pkg/board/infineon_Applikation_Kit_TC2X7/inc)

ifeq ($(call iseeopt, EE_USE_DISPLAY), yes)
EE_SRCS += pkg/board/infineon_Applikation_Kit_TC2X7/src/display.c
EE_SRCS += pkg/board/infineon_Applikation_Kit_TC2X7/src/graph.c
EE_SRCS += pkg/board/infineon_Applikation_Kit_TC2X7/src/font1.c
EE_SRCS += pkg/board/infineon_Applikation_Kit_TC2X7/src/util.c
endif # __USE_DISPLAY__

EE_SRCS += pkg/board/infineon_Applikation_Kit_TC2X7/src/TLF35584.c

endif

