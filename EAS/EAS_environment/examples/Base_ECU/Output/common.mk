

############################################################################
#
# Makefile created by RT-Druid
# 
# Common file
#
############################################################################

# Erika base directory
ifneq ($(__ERIKA_INNER_MAKEFILE_CALL__), yes)
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
endif # __ERIKA_INNER_MAKEFILE_CALL__


RTDRUID_CONFIGURATOR_NUMBER:=1278



############################################################################
#
# Common EE options
#
############################################################################
EEOPT += 
EEOPT += __ASSERT__
EEOPT += EE_EXECUTE_FROM_RAM
EEOPT += EE_ICACHE_ENABLED
EEOPT += EE_DCACHE_ENABLED
EEOPT += EE_BUILD_SINGLE_ELF
EEOPT += EE_USE_DISPLAY
EEOPT += EE_USE_LEDS
EEOPT += EE_USE_BUTTONS
EEOPT += __RTD_CYGWIN__
EEOPT += __MSRP__
EEOPT += EE_APPKIT_TC2X7
EEOPT += EE_TRICORE__
EEOPT += EE_TC29X__
EEOPT += EE_AS_RPC__
EEOPT += EE_AS_USER_SPINLOCKS__
EEOPT += __OO_ECC1__
EEOPT += __OO_EXTENDED_STATUS__
EEOPT += __OO_HAS_STARTUPHOOK__
EEOPT += __OO_HAS_SHUTDOWNHOOK__
EEOPT += __MULTI__
EEOPT += __DISABLE_EEOPT_DEFINES__



############################################################################
#
# Flags
#
############################################################################
CFLAGS   = 

ASFLAGS  = 

LDFLAGS := 

LDDEPS  += 

LIBS    := 

TRICORE_MODEL  := tc29x

-include $(EEBASE)/pkg/cfg/path_helper.mk
$(eval $(call check_and_set_cygwin_compiler_path,TRICORE_GCCDIR,$(shell dirname "$(shell which tricore-gcc)")/../))
