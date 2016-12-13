
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

RTDRUID_OUTPUT = $(addprefix $(OUTPUT_DIR)/, makefile common.mk master/common.c\
master/eecfg.h master/eecfg.c master/makefile\
slave1/eecfg.h slave1/eecfg.c slave1/makefile\
slave2/eecfg.h slave2/eecfg.c slave2/makefile)



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
	