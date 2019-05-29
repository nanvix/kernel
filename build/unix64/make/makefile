#
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

export TARGET    ?= unix64
export PROCESSOR := linux64

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR ?= $(TOOLSDIR)/dev/toolchain/unix/bin

# Toolchain
export CC = gcc
export LD = ld
export AR = ar

# Compiler Options
export CFLAGS  = -D __unix64__ -D__linux64__
#export CFLAGS += -nostdlib -nostdinc
export CFLAGS += -ansi # -pedantic-errors
export CFLAGS += -Wstack-usage=4096
export CFLAGS += -D __HAS_HW_DIVISION
#export CFLAGS += -D __INTERFACE_CHECK_CORE_AL
#export CFLAGS += -D __INTERFACE_CHECK_CLUSTER_AL
#export CFLAGS += -D __INTERFACE_CHECK_PROCESSOR_AL
#export CFLAGS += -D __INTERFACE_CHECK_TARGET_AL

# Linker Options
export LDFLAGS =

#===============================================================================
# Libraries and Binaries
#===============================================================================

# Libraries
export LIBKERNEL = libhal-$(TARGET).a
export LIBS = $(LIBDIR)/$(LIBKERNEL)
export LIB_THEIRS = -lpthread
