# Copyright(c) 2011-2023 The Maintainers of Nanvix.
# Licensed under the MIT License.

# Toolchain
export CC := $(TOOLCHAIN_DIR)/i486/bin/i486-elf-gcc
export LD := $(TOOLCHAIN_DIR)/i486/bin/i486-elf-ld
export AR := $(TOOLCHAIN_DIR)/i486/bin/i486-elf-ar

# Target Configuration
export CFLAGS += -D__x86__

# Compiler Options
export CFLAGS += -nostdlib -ffreestanding

# Linker Options
export LDFLAGS += -nostdlib -ffreestanding -Wl,--entry=_do_start

# Libraries
export THEIR_LIBS += -lgcc
