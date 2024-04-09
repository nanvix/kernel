# Copyright(c) 2011-2024 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Artifacts
#===============================================================================

# Asembly Source Files
SRC_ASM := $(BUILD_DIR)/$(TARGET)/crt0.S

# Object Files
OBJ = $(SRC_ASM:.S=.$(TARGET).o)

# Linker Options
export LDFLAGS += -L $(BUILD_DIR)/$(TARGET) -T user.ld
export LDFLAGS += --gc-sections

# Cargo Options
export CARGO_FLAGS += --target=$(BUILD_DIR)/$(TARGET)/target.json

# Libraries
export LIBS += $(LIBRARIES_DIR)/$(LIBCORE)

export NAME := test
export LIB := lib$(NAME).a
export BIN := $(NAME).$(EXEC_FORMAT)

#===============================================================================
# Build Rules
#===============================================================================

# Builds binary file.
all: check-fmt $(OBJ) $(LIB)
ifeq ($(VERBOSE), no)
	@echo [CC] $(BINARIES_DIR)/$(BIN)
	@$(LD) $(LDFLAGS) -o $(BINARIES_DIR)/$(BIN) $(OBJ) $(LIB) $(LIBS)
	@echo [CLEAN] $(LIB)
	@rm -rf $(LIB)
else
	$(LD) $(LDFLAGS) -o $(BINARIES_DIR)/$(BIN) $(OBJ) $(LIB) $(LIBS)
	rm -rf $(LIB)
endif

# Check code style formatting for Rust.
check-fmt:
	$(CARGO) fmt --all -- --check

# Cleans build objects.
clean:
ifeq ($(VERBOSE), no)
	@echo [CLEAN] $(OBJ) $(LIB) target
	@rm -rf $(OBJ) $(LIB) target
	@echo [CLEAN] $(BINARIES_DIR)/$(BIN)
	@rm -rf $(BINARIES_DIR)/$(BIN)
else
	rm -rf $(OBJ)
	rm -rf $(BINARIES_DIR)/$(BIN)
endif

# Compile rust kernel object
$(LIB):
	$(CARGO) build --lib $(CARGO_FLAGS)
ifeq ($(RELEASE), yes)
	cp --preserve target/target/release/$(LIB) $@
else
	cp --preserve target/target/debug/$(LIB) $@
endif

# Builds an assembly source file.
%.$(TARGET).o: %.S
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif
