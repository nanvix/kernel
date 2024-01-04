# Copyright(c) 2011-2023 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Build Rules
#===============================================================================

all: all-libcore all-kernel all-servers

clean: clean-libcore clean-kernel clean-servers

all-libcore:
	@$(MAKE) -C libcore all

clean-libcore:
	@$(MAKE) -C libcore clean

all-libdev:
	@$(MAKE) -C libdev all

clean-libdev:
	@$(MAKE) -C libdev clean

all-libnanvix: all-libcore
	@$(MAKE) -C libnanvix all

clean-libnanvix: clean-libcore
	@$(MAKE) -C libnanvix clean

all-kernel: all-libcore all-libdev
	@$(MAKE) -C kernel all

clean-kernel: clean-libcore clean-libdev
	@$(MAKE) -C kernel clean

all-servers: all-libnanvix
	@$(MAKE) -C servers all

clean-servers: clean-libnanvix
	@$(MAKE) -C servers clean
