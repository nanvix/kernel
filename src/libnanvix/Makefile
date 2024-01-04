# Copyright(c) 2011-2023 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Toolchain Options
#===============================================================================

# Cargo Options
export CARGO_FLAGS += --target=$(BUILD_DIR)/$(TARGET)/target.json

#===============================================================================
# Build Rules
#===============================================================================

# Builds library.
all:
	$(CARGO) build $(CARGO_FLAGS)

# Cleans build objects.
clean:
	$(CARGO) clean $(CARGO_FLAGS)
