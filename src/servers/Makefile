# Copyright(c) 2011-2023 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Artifacts
#===============================================================================

# C Source Files
SRC = $(wildcard *.c)

# Object Files
OBJ = $(SRC:.c=.$(TARGET).o)

# Linker Options
export LDFLAGS += -L $(BUILD_DIR)/$(TARGET) -T user.ld

export BIN = init.$(EXEC_FORMAT)

#===============================================================================
# Build Rules
#===============================================================================

# Builds binary file.
all: $(OBJ)
ifeq ($(VERBOSE), no)
	@echo [CC] $(BINARIES_DIR)/$(BIN)
	@$(CC) $(LDFLAGS) -o $(BINARIES_DIR)/$(BIN) $(OBJ) $(LIBS) $(THEIR_LIBS)
else
	$(CC) $(LDFLAGS) -o $(BINARIES_DIR)/$(BIN) $(OBJ) $(LIBS) $(THEIR_LIBS)
endif

# Cleans build objects.
clean:
ifeq ($(VERBOSE), no)
	@echo [CLEAN] $(OBJ)
	@rm -rf $(OBJ)
	@echo [CLEAN] $(BINARIES_DIR)/$(BIN)
	@rm -rf $(BINARIES_DIR)/$(BIN)
else
	rm -rf $(OBJ)
	rm -rf $(BINARIES_DIR)/$(BIN)
endif

# Builds a C source file.
%.$(TARGET).o: %.c
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif
