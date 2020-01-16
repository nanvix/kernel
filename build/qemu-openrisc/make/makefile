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

export TARGET    ?= qemu-openrisc
export PROCESSOR := or1k-processor
export CLUSTER   := or1k-cluster
export CORE      := or1k

export CFLAGS := -D __qemu_openrisc__ # Target
export CFLAGS += -D __openrisc__      # Processor
export CFLAGS += -D __or1k_cluster__  # Cluster
export CFLAGS += -D __or1k__          # Core
export CFLAGS += -D __or1200__        # Core Variant

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Toolchain Directory
export TOOLCHAIN_DIR ?= $(TOOLSDIR)/toolchain/or1k/bin/

# Toolchain
export CC := $(TOOLCHAIN_DIR)/or1k-elf-gcc
export LD := $(TOOLCHAIN_DIR)/or1k-elf-ld
export AR := $(TOOLCHAIN_DIR)/or1k-elf-ar

# Timeout Variable
export TIMEOUT ?= 120

# Compiler Options
export CFLAGS += -nostdlib -nostdinc -ffreestanding
export CFLAGS += -ansi
export CFLAGS += -Wstack-usage=4096
export CFLAGS += -Wno-type-limits
export CFLAGS += -D __HAS_HW_DIVISION=1 -D__HAS_HW_MULTIPLICATION=1

# Linker Options
export LDFLAGS := -nostdlib -nostdinc -ffreestanding
export LDFLAGS += -Wl,--allow-multiple-definition

# Suffix for Objects
export OBJ_SUFFIX := qemu-openrisc

#===============================================================================
# Libraries
#===============================================================================

export THEIR_LIBS := -lgcc
export LIBLWIP := liblwip-qemu-openrisc.a

#===============================================================================

# Number of images to be build
NB_IMAGES ?= 1
export BINARIES = $(addprefix test-driver, $(shell seq 1 ${NB_IMAGES}) )

# Builds everything.
all-target: make-dirs
	for number in $$(seq 1 $(NB_IMAGES)); do \
		$(MAKE) one-image image_ID=$$number; \
	done; \

one-image: clean-target
one-image: export CFLAGS += -D NUMBER_OF_GUESTS=$(NB_IMAGES)
one-image: export CFLAGS += -D GUEST_ID=$(image_ID)
one-image: export CFLAGS += -D'NETWORK_REMOTE_ADDR="192.169.66."'
one-image: export CFLAGS += -D'NETWORK_DEFAULT_IPADDR="192.169.66.$(image_ID)"'
one-image: export CFLAGS += -D'NETWORK_DEFAULT_NETMASK="255.255.255.0"'
one-image: export CFLAGS += -D'NETWORK_DEFAULT_GATEWAY="192.169.66.$(image_ID)"'
one-image: export CFLAGS += -D'MAC_DEFAULT_ADDR=0x$(image_ID)'
one-image: export CFLAGS += -D'NET_PROCESSOR_CCLUSTERS_NUM=$(shell echo $$(($(NB_IMAGES) - 1)))'
one-image: export CFLAGS += -D'NET_PROCESSOR_IOCLUSTERS_NUM=1'
one-image: export CFLAGS += -D'NET_PROCESSOR_CLUSTERNUM_MASTER=0'
one-image: export CFLAGS += -D'NET_PROCESSOR_CLUSTERNUM=$(shell echo $$(($(image_ID) - 1)))'
one-image: export CFLAGS += -D'NET_PROCESSOR_NOC_CNODES_NUM=$(shell echo $$(($(NB_IMAGES) - 1)))'
one-image: export CFLAGS += -D'NET_PROCESSOR_NOC_IONODES_NUM=1'
one-image: export CFLAGS += -D'NET_PROCESSOR_NODENUM_MASTER=0'
one-image: export CFLAGS += -D'NET_PROCESSOR_NODENUM=$(shell echo $$(($(image_ID) - 1)))'
one-image: export EXEC = test-driver$(image_ID)
one-image: export LIBKERNEL := libkernel-$(TARGET)-cluster$(image_ID).a
one-image:
	@$(MAKE) -C $(SRCDIR) all

# Cleans object files.
clean-target:
	@$(MAKE) -C $(SRCDIR) clean

# Cleans everything.
distclean-target:
	@$(MAKE) -C $(SRCDIR) distclean
