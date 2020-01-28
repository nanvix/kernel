#
# MIT License
#
# Copyright(c) 2011-2020 The Maintainers of Nanvix
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

export TARGET    ?= unix64
export PROCESSOR := linux64
export CLUSTER   := linux64-cluster
export CORE      := linux64-core

export CFLAGS := -D __unix64__            # Target
export CFLAGS += -D __linux64_processor__ # Processor
export CFLAGS += -D __linux64_cluster__   # Cluster
export CFLAGS += -D __linux64_core__      # Core

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR ?= $(TOOLSDIR)/toolchain/unix/bin

# Toolchain
export CC := gcc
export LD := ld
export AR := ar

# Timeout Variable
export TIMEOUT ?= 60

# Compiler Options
export CFLAGS += -D_GNU_SOURCE -D_POSIX_C_SOURCE=200112L
export CFLAGS += -ansi -Wno-error=pedantic
export CFLAGS += -Wstack-usage=4096
export CFLAGS += -D __HAS_HW_DIVISION=1 -D__HAS_HW_MULTIPLICATION=1

# Linker Options
export LDFLAGS :=

# Suffix for Objects
export OBJ_SUFFIX := unix64

#===============================================================================
# Libraries
#===============================================================================

export THEIR_LIBS := -lpthread -lm -lrt

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
