

############################################################################
#
# Makefile created by RT-Druid
#
############################################################################
# Current Project
export PROJECT := myProject



############################################################################
#
# Master CPU
#
############################################################################
export CPU_MASTER_NAME := master
export CPU_MASTER_PORT := 10000
export CPU_MASTER_ID   := 0
export CPU_MASTER_ELF  := $(PROJECT)_master.elf
export CPU_MASTER_DIR  := master



############################################################################
#
# Slave CPUs
#
############################################################################
# Slave cpu slave1
export CPU1_NAME := slave1
export CPU1_PORT := 10001
export CPU1_ID   := 1
export CPU1_ELF  := $(PROJECT)_slave1.elf
export CPU1_DIR  := slave1

# Slave cpu slave2
export CPU2_NAME := slave2
export CPU2_PORT := 10002
export CPU2_ID   := 2
export CPU2_ELF  := $(PROJECT)_slave2.elf
export CPU2_DIR  := slave2


# List of all the slave CPU dirs
export CPU_DIRS :=  $(CPU1_DIR) $(CPU2_DIR)
# List of all CPU
export CPU_LIST := CPU_MASTER  CPU1 CPU2



############################################################################
#
# Orti
#
############################################################################
export EE_ORTI_SUPPORT := 1


############################################################################
#
# Include common and standard makefiles ...
#
############################################################################
include common.mk

export EEBASE
__ERIKA_INNER_MAKEFILE_CALL__:=yes
export __ERIKA_INNER_MAKEFILE_CALL__
__BASE_MAKEFILE__ = yes
include $(EEBASE)/pkg/cfg/rules.mk

