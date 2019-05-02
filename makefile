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

# Object suffix for heterogeneous architectures.
export OBJ_SUFFIX :=

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
export TOOLSDIR   := $(ROOTDIR)/tools

#===============================================================================
# Libraries and Binaries
#===============================================================================

# Libraries
export LIBHAL    = $(LIBDIR)/libhal-$(TARGET).a
export LIBKERNEL = $(LIBDIR)/libkernel-$(TARGET).a
export LIBNANVIX = $(LIBDIR)/libnanvix-$(TARGET).a

# Binaries
export EXEC := $(BINDIR)/test-driver

#===============================================================================
# Target-Specific Make Rules
#===============================================================================

include $(MAKEDIR)/makefile

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Compiler Options.
export CFLAGS  += -std=c99 -fno-builtin
export CFLAGS  += -pedantic-errors
export CFLAGS  += -Wall -Wextra -Werror -Wa,--warn
export CFLAGS  += -Winit-self -Wswitch-default -Wfloat-equal
export CFLAGS  += -Wundef -Wshadow -Wuninitialized -Wlogical-op
export CFLAGS  += -Wno-unused-function
export CFLAGS  += -fno-stack-protector
export CFLAGS  += -Wvla # -Wredundant-decls
export CFLAGS  += -I $(INCDIR)
include $(BUILDDIR)/makefile.oflags

# Archiver Options
export ARFLAGS = rc

#===============================================================================

# Image Name
export IMAGE = nanvix-debug.img

# Builds everything.
all: | make-dirs all-target
	bash $(TOOLSDIR)/image/build-image.sh $(BINDIR) $(IMAGE)

# Make Directories
make-dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(LIBDIR)

# Cleans builds.
clean: clean-target

# Cleans everything.
distclean: | clean-target
	 @rm -f $(BINDIR)/*
	 @find $(SRCDIR) -name "*.o" -exec rm -rf {} \;

# Builds contrib.
contrib: make-dirs
	$(MAKE) -C $(CONTRIBDIR)/hal install PREFIX=$(ROOTDIR)

# Cleans the HAL.
contrib-uninstall:
	$(MAKE) -C $(CONTRIBDIR)/hal uninstall PREFIX=$(ROOTDIR)

# Runs Unit Tests in all clusters
run: | all
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) all --no-debug

# Runs Unit Tests in IO Cluster.
run-iocluster: | all
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) iocluster --no-debug

# Runs Unit Tests in Compute Cluster.
run-ccluster: | all
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) ccluster --no-debug

# Runs Unit Tests in all clusters in debug mode.
debug: | all
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) all --debug

# Runs Unit Tests in IO Cluster in debug mode.
debug-iocluster: | all
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) iocluster --debug

# Runs Unit Tests in Compute Cluster in debug mode.
debug-ccluster: | all
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) ccluster --debug
