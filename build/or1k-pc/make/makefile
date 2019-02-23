#
# MIT License
#
# Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
#              2018 Davidson Francis     <davidsondfgl@gmail.com>
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

# Target Configuration
export ARCH = or1k

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR=$(TOOLSDIR)/dev/toolchain/or1k/bin/

# Toolchain
export CC = $(TOOLCHAIN_DIR)/or1k-elf-gcc
export LD = $(TOOLCHAIN_DIR)/or1k-elf-ld
export AR = $(TOOLCHAIN_DIR)/or1k-elf-ar

# Compiler Options
export CFLAGS   = -D __or1k__ -D __pc__
export CFLAGS  += -fextended-identifiers
export CFLAGS  += -nostdlib -nostdinc -fno-stack-protector

# Linker Options
export LDFLAGS  =

#===============================================================================
# Libraries and Binaries
#===============================================================================

# Libraries
export LIBKERNEL = libkernel-$(ARCH).a
export LIBNANVIX = libnanvix-$(ARCH).a
export LIBS += $(LIBDIR)/$(LIBNANVIX) $(LIBDIR)/$(LIBKERNEL)

# Binaries
export EXECBIN = kernel

#===============================================================================

# Builds Nanvix for the or1k PC target.
nanvix-target:
	$(MAKE) -C $(SRCDIR) -f build/makefile.or1k all

# Cleans everything for the or1k PC target.
distclean-target:
	$(MAKE) -C $(SRCDIR) -f build/makefile.or1k distclean
