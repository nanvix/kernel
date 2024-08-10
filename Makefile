# Copyright(c) The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Build Options
#===============================================================================

# Target Architecture
export TARGET ?= x86

# Target Machine
export MACHINE ?= pc

# Verbose build?
export VERBOSE ?= no

# Release Version?
export RELEASE ?= no

# Timeout
export TIMEOUT ?= 10

# Log Level
export LOG_LEVEL ?= warn

#===============================================================================
# Directories
#===============================================================================

export ROOT_DIR      ?= $(CURDIR)
export BINARIES_DIR  ?= $(ROOT_DIR)/bin
export LIBRARIES_DIR ?= $(ROOT_DIR)/lib
export BUILD_DIR     ?= $(ROOT_DIR)/build
export IMAGE_DIR     ?= $(BUILD_DIR)/iso
export SCRIPTS_DIR   ?= $(BUILD_DIR)/scripts
export SOURCES_DIR   ?= $(ROOT_DIR)/src
export TOOLCHAIN_DIR ?= $(ROOT_DIR)/toolchain

#===============================================================================
# Libraries and Binaries
#===============================================================================

# Binary
export KERNEL := nanvix.$(EXEC_FORMAT)

# Image
export IMAGE := nanvix.iso

#===============================================================================
# Toolchain
#===============================================================================

include $(BUILD_DIR)/makefile

#===============================================================================
# Build Artifacts
#===============================================================================

# Asembly Source Files
ASM = $(wildcard src/hal/arch/x86/*.S)

# Object Files
OBJS = $(ASM:.S=.$(TARGET).o)

# Objects
export NAME := kernel
export BIN := $(NAME).$(EXEC_FORMAT)

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Cargo
ifneq ($(LOG_LEVEL),)
export CARGO_FEATURES += --features $(LOG_LEVEL)
endif

#===============================================================================
# Build Rules
#===============================================================================

all: make-dirs | $(OBJS)
	@PATH=$(GCC_HOME):"$(PATH)" $(CARGO) build --all $(CARGO_FLAGS) $(CARGO_FEATURES)
ifeq ($(RELEASE), yes)
	cp --preserve target/$(TARGET)/release/$(BIN) $(BINARIES_DIR)/$(BIN)
else
	cp --preserve target/$(TARGET)/debug/$(BIN) $(BINARIES_DIR)/$(BIN)
endif

clean:
	$(CARGO) clean
	rm -rf Cargo.lock
	rm -rf $(OBJS)
	rm -rf $(BINARIES_DIR)/$(BIN)

# Creates build directories.
make-dirs:
	@mkdir -p $(BINARIES_DIR)
	@mkdir -p $(LIBRARIES_DIR)

# Builds an assembly source file.
%.$(TARGET).o: %.S
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif
