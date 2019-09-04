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

export CFLAGS  = -D __mppa256__ # Target
export CFLAGS += -D __bostan__  # Processor
export CFLAGS += -D __k1b__     # Cluster and Core

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR ?= /usr/local/k1tools

# Toolchain
export CC = $(TOOLCHAIN_DIR)/bin/k1-gcc
export LD = $(TOOLCHAIN_DIR)/bin/k1-gcc
export AR = $(TOOLCHAIN_DIR)/bin/k1-ar

# Timeout Variable
export TIMEOUT ?= 120

# Compiler Options
export CFLAGS += -mos=bare -march=k1b -mboard=developer
export CFLAGS += -D __HAS_HW_DIVISION
export CFLAGS += -I $(INCDIR)/posix

# Linker Options
export LDFLAGS = -Wl,--allow-multiple-definition

#===============================================================================
# Libraries
#===============================================================================

export THEIR_LIBS = -lvbsp -mhypervisor
export LIBLWIP =

#===============================================================================

# Builds everything.
all-target: all-k1bio all-k1bdp

# Builds everything for node cluster variant.
all-k1bdp: clean-target
	@$(MAKE) -C $(SRCDIR) all                          \
		CFLAGS="$(CFLAGS) -D __k1bdp__ -mcluster=node" \
		LDFLAGS="$(LDFLAGS) -mcluster=node"            \
		LINKERDIR=$(BUILDDIR)/mppa256/linker/k1bdp     \
		LIBHAL=$(LIBDIR)/libhal-k1b-node.a             \
		LIBKERNEL=$(LIBDIR)/libkernel-k1b-node.a       \
		LIBNANVIX=$(LIBDIR)/libnanvix-k1b-node.a       \
		OBJ_SUFFIX=node                                \
		EXEC=test-driver-k1bdp

# Builds everything for node ioddr variant.
all-k1bio: clean-target
	@$(MAKE) -C $(SRCDIR) all                           \
		CFLAGS="$(CFLAGS) -D __k1bio__ -mcluster=ioddr" \
		LDFLAGS="$(LDFLAGS) -mcluster=ioddr"            \
		LINKERDIR=$(BUILDDIR)/mppa256/linker/k1bio      \
		LIBHAL=$(LIBDIR)/libhal-k1b-ioddr.a             \
		LIBKERNEL=$(LIBDIR)/libkernel-k1b-ioddr.a       \
		LIBNANVIX=$(LIBDIR)/libnanvix-k1b-ioddr.a       \
		EXEC=test-driver-k1bio                          \
		OBJ_SUFFIX=ioddr

# Cleans object files.
clean-target:
	@$(MAKE) -C $(SRCDIR) clean

# Cleans everything.
distclean-target: distclean-k1bdp distclean-k1bio

# Cleans everything for node cluster variant.
distclean-k1bdp: clean-target
	@$(MAKE) -C $(SRCDIR) distclean              \
		LIBKERNEL=$(LIBDIR)/libkernel-k1b-node.a \
		LIBNANVIX=$(LIBDIR)/libnanvix-k1b-node.a \
		OBJ_SUFFIX=node                          \
		EXEC=test-driver-k1bdp

# Cleans everything for node ioddr variant.
distclean-k1bio: clean-target
	@$(MAKE) -C $(SRCDIR) distclean               \
		LIBKERNEL=$(LIBDIR)/libkernel-k1b-ioddr.a \
		LIBNANVIX=$(LIBDIR)/libnanvix-k1b-ioddr.a \
		OBJ_SUFFIX=ioddr                          \
		EXEC=test-driver-k1bio
