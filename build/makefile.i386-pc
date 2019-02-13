#
# MIT License
#
# Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

export ARCH = i386

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR=$(TOOLSDIR)/dev/toolchain/i386/bin/

# Toolchain
export CC = $(TOOLCHAIN_DIR)/i386-elf-gcc
export LD = $(TOOLCHAIN_DIR)/i386-elf-ld
export AR = $(TOOLCHAIN_DIR)/i386-elf-ar

# Compiler Options
export CFLAGS   = -D __i386__ -D __pc__
export CFLAGS  += -pedantic-errors -fextended-identifiers
export CFLAGS  += -nostdlib -nostdinc -fno-stack-protector
export CFLAGS  += -ansi -pedantic-errors
export CFLAGS  += -Wstack-usage=4096

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
export EXECBIN = test-driver-$(TARGET)

#===============================================================================

# Builds Nanvix for the i386 PC target.
nanvix-target:
	$(MAKE) -C $(SRCDIR) -f build/makefile.i386 all

# Cleans everything for the i386 PC target.
distclean-target:
	$(MAKE) -C $(SRCDIR) -f build/makefile.i386 distclean
