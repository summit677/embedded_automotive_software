# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
# 
# Copyright (C) 2002-2008  Evidence Srl
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

## Author: 2004 Paolo Gai
## CVS: $Id: cfg.mk,v 1.3 2008/07/18 09:53:55 tiberipa Exp $

ifeq ($(call iseeopt, __FRSH__), yes)

INTERNAL_FRSH_PATH := -I"$(shell cygpath -w $(PKGBASE))\\."/kernel/frsh/frsh_include
ALLINCPATH += $(INTERNAL_FRSH_PATH)
## New Include mechanism
INCLUDE_PATH += $(PKGBASE)/kernel/frsh/frsh_include

EE_SRCS += pkg/kernel/frsh/src/ee_cap.c
EE_SRCS += pkg/kernel/frsh/src/ee_dlcheck.c
EE_SRCS += pkg/kernel/frsh/src/ee_end_budget.c
EE_SRCS += pkg/kernel/frsh/src/ee_end_recharging.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_bind.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_unbind.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_bind_utils.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_contracts.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_getvresid.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_getcontract.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_init.c
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_strerror.c
EE_SRCS += pkg/kernel/frsh/src/ee_gettime.c
EE_SRCS += pkg/kernel/frsh/src/ee_irq_sc.c
EE_SRCS += pkg/kernel/frsh/src/ee_mutex.c
EE_SRCS += pkg/kernel/frsh/src/ee_rcg_inser.c
EE_SRCS += pkg/kernel/frsh/src/ee_rq_extract.c
EE_SRCS += pkg/kernel/frsh/src/ee_rq_inser.c
EE_SRCS += pkg/kernel/frsh/src/ee_schedule.c
EE_SRCS += pkg/kernel/frsh/src/ee_thact.c
EE_SRCS += pkg/kernel/frsh/src/ee_thendin.c


ifeq ($(call iseeopt, __FRSH_SINGLEIRQ__), yes)
EE_SRCS += pkg/kernel/frsh/src/ee_frsh_timers.c
endif

ifeq ($(call iseeopt, __FRSH_SYNCHOBJ__), yes)
EE_SRCS +=pkg/kernel/frsh/src/ee_frsh_syncobj_signal.c
EE_SRCS +=pkg/kernel/frsh/src/ee_frsh_syncobj_wait.c
EE_SRCS +=pkg/kernel/frsh/src/ee_frsh_syncobj_waittimeout.c
EE_SRCS +=pkg/kernel/frsh/src/ee_frsh_syncobj_wait_utils.c
EE_SRCS +=pkg/kernel/frsh/src/ee_frsh_syncobj_timedwait.c
endif

endif


