# Copyright(c) 2011-2024 The Maintainers of Nanvix.
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
all: check-fmt
	$(CARGO) build $(CARGO_FLAGS)

# Check code style formatting for Rust.
check-fmt:
	$(CARGO) fmt --all -- --check

# Cleans build objects.
clean:
	$(CARGO) clean $(CARGO_FLAGS)
