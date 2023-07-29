# Copyright(c) 2011-2023 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Build Rules
#===============================================================================

all: all-kernel

clean: clean-kernel

all-libcore:
	@$(MAKE) -C libcore all

clean-libcore:
	@$(MAKE) -C libcore clean

all-libdev:
	@$(MAKE) -C libdev all

clean-libdev:
	@$(MAKE) -C libdev clean


all-kernel: all-libcore all-libdev
	@$(MAKE) -C kernel all

clean-kernel: clean-libcore clean-libdev
	@$(MAKE) -C kernel clean
