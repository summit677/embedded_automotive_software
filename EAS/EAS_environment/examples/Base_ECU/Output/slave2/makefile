

############################################################################
#
# Makefile created by RT-Druid
# 
# Cpu 2(slave2)
#
############################################################################

# Include common informations
include ../common.mk

# Base dir
APPBASE ?= ../..


############################################################################
#
# EE options
#
############################################################################
EEOPT += 
EEOPT += EE_GNU__
EEOPT += __OO_ORTI_LASTERROR__
EEOPT += __OO_ORTI_SERVICETRACE__
EEOPT += __OO_ORTI_PRIORITY__
EEOPT += __OO_ORTI_RES_LOCKER_TASK__
EEOPT += __OO_ORTI_RES_ISLOCKED__
EEOPT += __OO_ORTI_STACK__
EEOPT += __OO_ORTI_ALARMTIME__
EEOPT += __OO_ORTI_RUNNINGISR2__
EEOPT += __OO_AUTOSTART_TASK__
EEOPT += __ALLOW_NESTED_IRQ__


############################################################################
#
# Sources
#
############################################################################
APP_SRCS +=  src/slave2.c eecfg.c

# Add a flag for the linkerscript to set the minimum size of system stack
LDFLAGS += -Wl,--defsym=__USTACK_SIZE=4096
LDFLAGS += -Wl,--defsym=__CSA_SIZE=16384



############################################################################
#
# Default rules
#
############################################################################
include $(EEBASE)/pkg/cfg/rules.mk

