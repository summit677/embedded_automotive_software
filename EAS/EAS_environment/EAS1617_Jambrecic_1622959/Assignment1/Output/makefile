### Setting Erika's variables:
# ERIKA_FILES= ../../..
# APPBASE = .
# 
# EEOPT =


RTDRUID_CONFIGURATOR_NUMBER:=1278



############################################################################
#
# User options
#
############################################################################
EEOPT += __ASSERT__
EEOPT += EE_EXECUTE_FROM_RAM
EEOPT += EE_BUILD_SINGLE_ELF
EEOPT += EE_USE_DISPLAY
EEOPT += EE_USE_LEDS
EEOPT += EE_USE_BUTTONS


############################################################################
#
# Automatic options
#
############################################################################
EEOPT += __RTD_CYGWIN__
EEOPT += EE_APPKIT_TC2X7
EEOPT += EE_TRICORE__
EEOPT += EE_TC29X__
EEOPT += EE_GNU__
EEOPT += ENABLE_SYSTEM_TIMER
EEOPT += __OO_ECC1__
EEOPT += __OO_EXTENDED_STATUS__
EEOPT += __IRQ_STACK_NEEDED__
EEOPT += __MULTI__
EEOPT += __OO_NO_RESOURCES__
EEOPT += __OO_AUTOSTART_TASK__
EEOPT += __OO_AUTOSTART_ALARM__
EEOPT += __ALLOW_NESTED_IRQ__
EEOPT += __DISABLE_EEOPT_DEFINES__


############################################################################
#
# Erika base directory
#
############################################################################
ifdef ERIKA_FILES
ifdef EEBASE
    $(warning EEBASE is set, but it has been overridden by ERIKA_FILES)
endif
EEBASE := $(shell cygpath `cygpath -ms '${ERIKA_FILES}'`)

else # ERIKA_FILES

ifndef EEBASE
        ${error RT-Druid was not able to find a default value for erika file. Please set ERIKA_FILES before run the makefile}
else
        $(warning The usage of EEBASE is deprecated. Please use ERIKA_FILES)
endif
endif # ERIKA_FILES
# ERIKA_FILES has fulfilled its role. Make sure it's not used inside Erika makefiles
ERIKA_FILES :=
$(info Using erika files in $(EEBASE))


############################################################################
#
# Tricore
#
############################################################################
export PLATFORM := CYGWIN

APPBASE := ..
OUTBASE := Output

-include $(EEBASE)/pkg/cfg/path_helper.mk
$(eval $(call check_and_set_cygwin_compiler_path,TRICORE_GCCDIR,$(shell dirname "$(shell which tricore-gcc)")/../))
TRICORE_MODEL  := tc29x

# Add a flag for the linkerscript to set the minimum size of system stack
LDFLAGS += -Wl,--defsym=__USTACK_SIZE=8192
LDFLAGS += -Wl,--defsym=__CSA_SIZE=16384



############################################################################
#
# Flags
#
############################################################################
CFLAGS  += 
ASFLAGS += 
LDFLAGS += 
LDDEPS  += 
LIBS    += 



############################################################################
#
# Sources
#
############################################################################
APP_SRCS += eecfg.c src/code.c


############################################################################
#
# end
#
############################################################################
include $(EEBASE)/pkg/cfg/rules.mk
