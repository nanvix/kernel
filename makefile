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

# Installation Prefix
export PREFIX ?= $HOME

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
export EXEC_BENCH := $(BINDIR)/benchmarks

#===============================================================================
# Target-Specific Make Rules
#===============================================================================

include $(MAKEDIR)/makefile

#===============================================================================
# Artifacts
#===============================================================================

export ARTIFACTS := $(shell find $(INCDIR) -name *.h -type f)

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
export IMAGE_BENCHMARKS = nanvix-benchmarks.img

# Builds everything.
all: image-tests image-benchmarks

image-tests: | make-dirs all-target
	bash $(TOOLSDIR)/image/build-image.sh $(BINDIR) $(IMAGE)

image-benchmarks: | make-dirs all-target
	bash $(TOOLSDIR)/image/build-image.sh $(BINDIR) $(IMAGE_BENCHMARKS)

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

# Install
install: | all-target copy-artifacts
	@mkdir -p $(PREFIX)/lib
	@cp -f $(LIBDIR)/libhal*.a $(PREFIX)/lib
	@echo [CP] $(LIBDIR)/libhal*.a
	@cp -f $(LIBDIR)/libkernel*.a $(PREFIX)/lib
	@echo [CP] $(LIBDIR)/libkernel*.a
	@echo "==============================================================================="
	@echo "Nanvix Microkernel Successfully Installed into $(PREFIX)"
	@echo "==============================================================================="

# Uninstall
uninstall: | distclean delete-artifacts
	@rm -f $(PREFIX)/lib/libhal*.a
	@echo [RM] $(PREFIX)/lib/libhal*.a
	@rm -f $(PREFIX)/lib/libkernel*.a
	@echo [RM] $(PREFIX)/lib/libkernel*.a
	@echo "==============================================================================="
	@echo "Nanvix Microkernel Successfully Uninstalled from $(PREFIX)"
	@echo "==============================================================================="

# Copies All Artifacts
copy-artifacts: $(patsubst $(CURDIR)/%, copy/%, $(ARTIFACTS))

# Copy a Single Artifact
copy/%: %
	$(eval file := $(<F))
	$(eval dir := $(<D))
	@echo [CP] $(dir)/$(file)
	@mkdir -p $(PREFIX)/$(dir)
	@cp -f $< $(PREFIX)/$(dir)/$(file)
	@chmod 444 $(PREFIX)/$(dir)/$(file)

# Deletes All Artifacts
delete-artifacts: $(patsubst $(CURDIR)/%, delete/%, $(ARTIFACTS))

# Deletes a Single Artifact
delete/%:
	$(eval file := $(patsubst delete/%, %, $@))
	@echo [RM] $(file)
	@rm -f $(PREFIX)/$(file)

# Builds contrib.
contrib: make-dirs
	$(MAKE) -C $(CONTRIBDIR)/hal install PREFIX=$(ROOTDIR)

# Cleans the HAL.
contrib-uninstall:
	$(MAKE) -C $(CONTRIBDIR)/hal uninstall PREFIX=$(ROOTDIR)

# Runs Unit Tests in all clusters
run:
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) all --no-debug

# Runs Unit Tests in IO Cluster.
run-iocluster:
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) iocluster --no-debug

# Runs Unit Tests in Compute Cluster.
run-ccluster:
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) ccluster --no-debug

# Runs Unit Tests in all clusters in debug mode.
debug:
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) all --debug

# Runs Unit Tests in IO Cluster in debug mode.
debug-iocluster:
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) iocluster --debug

# Runs Unit Tests in Compute Cluster in debug mode.
debug-ccluster:
	bash $(TOOLSDIR)/utils/nanvix-run.sh $(IMAGE) $(EXEC) $(TARGET) ccluster --debug
