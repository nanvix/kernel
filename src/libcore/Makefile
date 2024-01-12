# Copyright(c) 2011-2024 The Maintainers of Nanvix.
# Licensed under the MIT License.

#===============================================================================
# Artifacts
#===============================================================================

# C Source Files
SRC = $(wildcard *.c)


# Object Files
OBJ = $(SRC:.c=.$(TARGET).o)

#===============================================================================
# Build Rules
#===============================================================================

# Builds binary file.
all: $(OBJ)
ifeq ($(VERBOSE), no)
	@echo [AR] $(LIBRARIES_DIR)/$(LIBCORE)
	 @$(AR) $(ARFLAGS) $(LIBRARIES_DIR)/$(LIBCORE) $(OBJ)
else
	 $(AR) $(ARFLAGS) $(LIBRARIES_DIR)/$(LIBCORE) $(OBJ)
endif

# Cleans build objects.
clean:
ifeq ($(VERBOSE), no)
	@echo [CLEAN] $(OBJ)
	@rm -rf $(OBJ)
	@echo [CLEAN] $(LIBRARIES_DIR)/$(LIBCORE)
	@rm -rf $(LIBRARIES_DIR)/$(LIBCORE)
else
	rm -rf $(OBJ)
	rm -rf $(LIBRARIES_DIR)/$(LIBCORE)
endif

# Builds a C source file.
%.$(TARGET).o: %.c
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif
