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

export CFLAGS  = -D __qemu_x86__    # Target
export CFLAGS += -D __x86__         # Processor
export CFLAGS += -D __x86_cluster__ # Cluster
export CFLAGS += -D __i486__        # Core

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR ?= $(TOOLSDIR)/toolchain/i486/bin

# Toolchain
export CC = $(TOOLCHAIN_DIR)/i486-elf-gcc
export LD = $(TOOLCHAIN_DIR)/i486-elf-ld
export AR = $(TOOLCHAIN_DIR)/i486-elf-ar

# Timeout Variable
export TIMEOUT ?= 5

# Compiler Options
export CFLAGS += -nostdlib -nostdinc -ffreestanding
export CFLAGS += -ansi -pedantic-errors
export CFLAGS += -Wstack-usage=4096
export CFLAGS += -D __HAS_HW_DIVISION
export CFLAGS += -I $(INCDIR)/posix

# Linker Options
export LDFLAGS  = -Wl,--allow-multiple-definition
export LDFLAGS += -nostdlib -nostdinc -ffreestanding

# Suffix for Objects
export OBJ_SUFFIX=qemu-x86

#===============================================================================
# Libraries
#===============================================================================

export THEIR_LIBS := -lgcc
export LIBLWIP = $(LIBDIR)/liblwip-$(TARGET).a

#===============================================================================

# Builds everything.
all-target:
	@$(MAKE) -C $(SRCDIR) all

# Cleans object files.
clean-target:
	@$(MAKE) -C $(SRCDIR) clean

# Cleans everything.
distclean-target:
	@$(MAKE) -C $(SRCDIR) distclean
