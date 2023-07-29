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
export TIMEOUT ?= 1

#===============================================================================
# Directories
#===============================================================================

export ROOT_DIR      := $(CURDIR)
export BINARIES_DIR  := $(ROOT_DIR)/bin
export LIBRARIES_DIR := $(ROOT_DIR)/lib
export INCLUDE_DIR   := $(ROOT_DIR)/include
export BUILD_DIR     := $(ROOT_DIR)/build
export SOURCES_DIR   := $(ROOT_DIR)/src
export SCRIPTS_DIR   := $(ROOT_DIR)/scripts
export TOOLCHAIN_DIR ?= $(ROOT_DIR)/toolchain

#===============================================================================
# Libraries and Binaries
#===============================================================================

# Binary
export KERNEL := nanvix.elf

# Libraries
export LIBCORE := libcore.a
export LIBDEV := libdev.a

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
export CFLAGS += -Winit-self -Wswitch-default -Wfloat-equal
export CFLAGS += -Wundef -Wshadow -Wuninitialized -Wlogical-op
export CFLAGS += -Wvla -Wredundant-decls -Wno-error=switch-default
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

# Archiver Options
export ARFLAGS = rc

# Linker Options
export LDFLAGS += -Wl,-z,noexecstack

#===============================================================================
# Build Rules
#===============================================================================

# Builds everything.
all: check-format make-dirs
	@$(MAKE) -C $(SOURCES_DIR) all

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

# Runs system in release mode.
run:
	bash $(SCRIPTS_DIR)/run.sh $(TARGET) "$(BINARIES_DIR)/$(KERNEL)" --no-debug $(TIMEOUT)

# Runs system in debug mode.
debug:
	bash $(SCRIPTS_DIR)/run.sh $(TARGET) "$(BINARIES_DIR)/$(KERNEL)" --debug $(TIMEOUT)
