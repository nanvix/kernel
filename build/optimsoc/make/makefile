#
# MIT License
#
# Copyright(c) 2011-2019 The Maintainers of Nanvix
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

#===============================================================================
# Target Configuration
#===============================================================================

export TARGET    ?= optimsoc
export PROCESSOR := optimsoc4
export CLUSTER   := or1k-cluster
export CORE      := mor1kx

export CFLAGS := -D __optimsoc__         # Target
export CFLAGS += -D __optimsoc4__        # Processor
export CFLAGS += -D __optimsoc_cluster__ # Cluster
export CFLAGS += -D __or1k__             # Core
export CFLAGS += -D __mor1kx__           # Core Variant

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR ?= $(TOOLSDIR)/toolchain/or1k/bin/

# Toolchain
export CC := $(TOOLCHAIN_DIR)/or1k-elf-gcc
export LD := $(TOOLCHAIN_DIR)/or1k-elf-ld
export AR := $(TOOLCHAIN_DIR)/or1k-elf-ar

# Compiler Options
export CFLAGS += -nostdlib -nostdinc -ffreestanding
export CFLAGS += -ansi -pedantic-errors
export CFLAGS += -Wstack-usage=4096
export CFLAGS += -Wno-type-limits

# Timeout Variable
export TIMEOUT ?= 5

# Linker Options
export LDFLAGS := -nostdlib -nostdinc -ffreestanding

# Suffix for Objects
export OBJ_SUFFIX := optimsoc

#===============================================================================
# Libraries
#===============================================================================

export THEIR_LIBS := -lgcc
export LIBLWIP :=

#===============================================================================

# Builds everything.
all-target: make-dirs
	@$(MAKE) -C $(SRCDIR) all

# Cleans object files.
clean-target:
	@$(MAKE) -C $(SRCDIR) clean

# Cleans everything.
distclean-target:
	@$(MAKE) -C $(SRCDIR) distclean
