# Copyright(c) The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Build Options
#===============================================================================

# Target Architecture
export TARGET ?= x86

# Verbose build?
export VERBOSE ?= no

# Release Version?
export RELEASE ?= no

# Timeout
export TIMEOUT ?= 10

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
# Toolchain Configuration
#===============================================================================

export LDFLAGS += -L $(BUILD_DIR)/$(TARGET) -T kernel.ld

#===============================================================================
# Build Artifacts
#===============================================================================

# Objects
export NAME := kernel
export LIB := lib$(NAME).a
export BIN := $(NAME).$(EXEC_FORMAT)

# Asembly Source Files
ASM = $(wildcard src/hal/arch/x86/*.S)

# Object Files
OBJS = $(ASM:.S=.$(TARGET).o)

#===============================================================================
# Build Rules
#===============================================================================

# Builds everything.
all: make-dirs $(OBJS)
	$(CARGO) build --lib $(CARGO_FLAGS)
ifeq ($(RELEASE), yes)
	@echo [CARGO] $(LIB)
	cp --preserve target/$(TARGET)/release/$(LIB) $(LIB)
else
	cp --preserve target/$(TARGET)/debug/$(LIB) $(LIB)
endif
	$(LD) $(LDFLAGS) -o $(BINARIES_DIR)/$(BIN) $(OBJS) $(LIB)

# Performs local initialization.
init:
	@git config --local core.hooksPath .githooks

# Creates build directories.
make-dirs: init
	@mkdir -p $(BINARIES_DIR)
	@mkdir -p $(LIBRARIES_DIR)

# Cleans build.
clean:
	$(CARGO) clean
	rm -rf Cargo.lock
	rm -rf $(LIB)
	rm -rf $(OBJS)
	@rm -f $(IMAGE_DIR)/*.$(EXEC_FORMAT)
	@rm -f $(IMAGE)
	rm -rf $(BINARIES_DIR)/$(BIN)

# Builds an assembly source file.
%.$(TARGET).o: %.S
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif
