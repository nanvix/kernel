# Copyright(c) 2011-2024 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Artifacts
#===============================================================================

# C Source Files
SRC_C = $(wildcard excp/*.c)          \
		$(wildcard hal/arch/msr/*.c)  \
		$(wildcard hal/arch/*.c)      \
		$(wildcard hal/exception/*.c) \
		$(wildcard hal/interrupt/*.c) \
		$(wildcard hal/mmap/*.c)      \
		$(wildcard hal/mmu/*.c)       \
		$(wildcard hal/*.c)           \
		$(wildcard iam/*.c)           \
		$(wildcard ipc/mailbox/*.c)   \
		$(wildcard kcall/*.c)         \
		$(wildcard kcall/ipc/*.c)     \
		$(wildcard kcall/sem/*.c)     \
		$(wildcard lib/*.c)           \
		$(wildcard log/*.c)           \
		$(wildcard pm/*.c)            \
		$(wildcard mm/frame/*.c)      \
		$(wildcard mm/kpool/*.c)      \
		$(wildcard mm/upool/*.c)      \
		$(wildcard mm/vmem/*.c)       \
		$(wildcard mm/*.c)            \
		$(wildcard *.c)

# Asembly Source Files
SRC_ASM = $(wildcard hal/arch/*.S)

# Object Files
OBJ = $(SRC_ASM:.S=.$(TARGET).o) \
      $(SRC_C:.c=.$(TARGET).o)

# Libraries
export LIBS += $(LIBRARIES_DIR)/$(LIBDEV)
export LIBS += $(LIBRARIES_DIR)/$(LIBCORE)

#===============================================================================
# Toolchain Configuration
#===============================================================================

# Linker Options
export LDFLAGS += -L $(BUILD_DIR)/$(TARGET) -T kernel.ld

#===============================================================================
# Build Rules
#===============================================================================

# Builds binary file.
all: $(OBJ)
ifeq ($(VERBOSE), no)
	@echo [CC] $(BINARIES_DIR)/$(KERNEL)
	@$(LD) $(LDFLAGS) -o $(BINARIES_DIR)/$(KERNEL) $(OBJ) $(LIBS)
else
	$(LD) $(LDFLAGS) -o $(BINARIES_DIR)/$(KERNEL) $(OBJ) $(LIBS)
endif

# Cleans build objects.
clean:
ifeq ($(VERBOSE), no)
	@echo [CLEAN] $(OBJ)
	@rm -rf $(OBJ)
	@echo [CLEAN] $(BINARIES_DIR)/$(KERNEL)
	@rm -rf $(BINARIES_DIR)/$(KERNEL)
else
	rm -rf $(OBJ)
	rm -rf $(BINARIES_DIR)/$(KERNEL)
endif

# Builds a C source file.
%.$(TARGET).o: %.c
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif

# Builds an assembly source file.
%.$(TARGET).o: %.S
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif
