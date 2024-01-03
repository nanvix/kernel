# Copyright(c) 2011-2023 The Maintainers of Nanvix.
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

export ROOT_DIR      := $(CURDIR)
export BINARIES_DIR  := $(ROOT_DIR)/bin
export LIBRARIES_DIR := $(ROOT_DIR)/lib
export INCLUDE_DIR   := $(ROOT_DIR)/include
export IMAGE_DIR     := $(ROOT_DIR)/iso
export BUILD_DIR     := $(ROOT_DIR)/build
export SOURCES_DIR   := $(ROOT_DIR)/src
export SCRIPTS_DIR   := $(ROOT_DIR)/scripts
export TOOLCHAIN_DIR ?= $(ROOT_DIR)/toolchain

#===============================================================================
# Libraries and Binaries
#===============================================================================

# File format for executables.
export EXEC_FORMAT := elf

# Binary
export KERNEL := nanvix.$(EXEC_FORMAT)

# Image
export IMAGE := nanvix.iso

# Libraries
export LIBCORE := libcore.a
export LIBDEV := libdev.a
export LIBNANVIX := libnanvix.a

#===============================================================================
# Toolchain
#===============================================================================

include $(ROOT_DIR)/build/$(TARGET)/makefile

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Compiler Options
export CFLAGS += -std=c17
export CFLAGS += -Wall -Wextra -Werror
export CFLAGS += -Winit-self -Wswitch-default -Wfloat-equal -Wno-pointer-arith
export CFLAGS += -Wundef -Wshadow -Wuninitialized -Wlogical-op
export CFLAGS += -Wvla -Wredundant-decls
export CFLAGS += -pedantic-errors
export CFLAGS += -Wstack-usage=4096
export CFLAGS += -I $(INCLUDE_DIR)

# Optimization Flags
ifeq ($(RELEASE), yes)
export CFLAGS += -O3
else
export CFLAGS += -O0
export CFLAGS += -g
endif

# Cargo Options
export CARGO_FLAGS += -Z build-std=core
ifeq ($(RELEASE), yes)
export CARGO_FLAGS += --release
endif

# Archiver Options
export ARFLAGS = rc

# Linker Options
export LDFLAGS += -z noexecstack

#===============================================================================
# Build Rules
#===============================================================================

# Builds everything.
all: check-format make-dirs
	@$(MAKE) -C $(SOURCES_DIR) all

libnanvix: check-format make-dirs
	@$(MAKE) -C $(SOURCES_DIR)/libnanvix all

# Performs local initialization.
init:
	@git config --local core.hooksPath .githooks

# Creates build directories.
make-dirs: init
	@mkdir -p $(BINARIES_DIR)
	@mkdir -p $(LIBRARIES_DIR)

# Cleans build.
clean:
	@$(MAKE) -C $(SOURCES_DIR) clean

# Check formatting.
check-format:
	$(shell find include src -type f \( -name "*.c" -o -name "*.h" \) -print0 | xargs -0 clang-format --sort-includes --dry-run -Werror)
	@exit $(.SHELLSTATUS)

# Builds Doxygen documentation.
doxygen:
	doxygen doc/kernel.doxygen

#===============================================================================
# Run and Debug Rules
#===============================================================================

# Builds the system image.
image: all
	@cp -f $(BINARIES_DIR)/*.$(EXEC_FORMAT) $(IMAGE_DIR)/
	@grub-mkrescue  $(IMAGE_DIR) -o $(IMAGE)

# Runs system in release mode.
run: image
	bash $(SCRIPTS_DIR)/run.sh $(TARGET) $(IMAGE) --no-debug $(TIMEOUT)

# Runs system in debug mode.
debug: image
	bash $(SCRIPTS_DIR)/run.sh $(TARGET) $(IMAGE) --debug $(TIMEOUT)
