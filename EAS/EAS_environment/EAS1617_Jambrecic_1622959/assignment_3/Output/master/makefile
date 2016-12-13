

############################################################################
#
# Makefile created by RT-Druid
# 
# Cpu 0(master)
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
EEOPT += EE_START_CPU1
EEOPT += EE_START_CPU2
EEOPT += __OO_ORTI_LASTERROR__
EEOPT += __OO_ORTI_SERVICETRACE__
EEOPT += __OO_ORTI_PRIORITY__
EEOPT += __OO_ORTI_RES_LOCKER_TASK__
EEOPT += __OO_ORTI_RES_ISLOCKED__
EEOPT += __OO_ORTI_STACK__
EEOPT += __OO_ORTI_ALARMTIME__
EEOPT += __OO_ORTI_RUNNINGISR2__
EEOPT += __RN__
EEOPT += __RN_TASK__
EEOPT += __OO_NO_ALARMS__
EEOPT += __OO_NO_RESOURCES__
EEOPT += __OO_NO_EVENTS__
EEOPT += __OO_AUTOSTART_TASK__
EEOPT += __ALLOW_NESTED_IRQ__


############################################################################
#
# Sources
#
############################################################################
APP_SRCS +=  common.c src/master.c eecfg.c
EE_USER_EXPORT_FILE := export_master.exp

# Add a flag for the linkerscript to set the minimum size of system stack
LDFLAGS += -Wl,--defsym=__USTACK_SIZE=4096
LDFLAGS += -Wl,--defsym=__CSA_SIZE=16384



############################################################################
#
# Default rules
#
############################################################################
include $(EEBASE)/pkg/cfg/rules.mk

