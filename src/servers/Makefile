# Copyright(c) 2011-2024 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Build Rules
#===============================================================================

all: all-init all-test

clean: clean-init clean-test

all-init:
	@$(MAKE) -C init all

clean-init:
	@$(MAKE) -C init clean

all-test:
	@$(MAKE) -C test all

clean-test:
	@$(MAKE) -C test clean
