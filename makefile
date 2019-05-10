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
# Build Options
#===============================================================================

# Verbose Build?
export VERBOSE ?= no

# Release Version?
export RELEASE ?= no

# Installation Prefix
export PREFIX ?= $(HOME)

#===============================================================================
# Directories
#===============================================================================

# Directories
export ROOTDIR    := $(CURDIR)
export BINDIR     := $(ROOTDIR)/bin
export BUILDDIR   := $(ROOTDIR)/build
export CONTRIBDIR := $(ROOTDIR)/contrib
export LINKERDIR  := $(BUILDDIR)/$(TARGET)/linker
export MAKEDIR    := $(BUILDDIR)/$(TARGET)/make
export DOCDIR     := $(ROOTDIR)/doc
export INCDIR     := $(ROOTDIR)/include
export LIBDIR     := $(ROOTDIR)/lib
export SRCDIR     := $(ROOTDIR)/src
export TOOLSDIR   := $(ROOTDIR)/utils

#===============================================================================
# Libraries and Binaries
#===============================================================================

# Libraries
export LIBHAL    = $(LIBDIR)/libhal-$(TARGET).a
export LIBKERNEL = $(LIBDIR)/libkernel-$(TARGET).a
export LIBNANVIX = $(LIBDIR)/libnanvix-$(TARGET).a

# Binaries
export EXEC := test-driver

#===============================================================================
# Target-Specific Make Rules
#===============================================================================

include $(MAKEDIR)/makefile

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Compiler Options
export CFLAGS  += -std=c99 -fno-builtin
export CFLAGS  += -pedantic-errors
export CFLAGS  += -Wall -Wextra -Werror -Wa,--warn
export CFLAGS  += -Winit-self -Wswitch-default -Wfloat-equal
export CFLAGS  += -Wundef -Wshadow -Wuninitialized -Wlogical-op
export CFLAGS  += -Wno-unused-function
export CFLAGS  += -fno-stack-protector
export CFLAGS  += -Wvla # -Wredundant-decls
export CFLAGS  += -I $(INCDIR)

# Additional C Flags
include $(BUILDDIR)/makefile.cflags

# Archiver Options
export ARFLAGS = rc

#===============================================================================

# Image Name
export IMAGE = nanvix-debug.img

# Builds everything.
all: image-tests

# Make Directories
make-dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(LIBDIR)

image-tests: | make-dirs all-target
	bash $(TOOLSDIR)/nanvix-build-image.sh $(IMAGE) $(BINDIR) $(EXEC)

# Cleans builds.
clean: clean-target

# Cleans everything.
distclean: distclean-target
	 @rm -f $(LIBDIR)/libkernel-* $(LIBDIR)/libnanvix-**
	 @rm -rf $(BINDIR)
	 @find $(SRCDIR) -name "*.o" -exec rm -rf {} \;

#===============================================================================
# Contrib Install and Uninstall Rules
#===============================================================================

include $(BUILDDIR)/makefile.contrib

#===============================================================================
# Install and Uninstall Rules
#===============================================================================

include $(BUILDDIR)/makefile.install

#===============================================================================
# Debug and Run Rules
#===============================================================================

include $(BUILDDIR)/makefile.run
