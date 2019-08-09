#
# MIT License
#
# Copyright(c) 2011-2019 The Maintainers of Nanvix
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

#===============================================================================
# Sources and Objects
#===============================================================================

# C Source Files
SRC = $(wildcard dev/*.c)       \
      $(wildcard dev/net/net.c) \
      $(wildcard init/*.c)      \
      $(wildcard mm/*.c)        \
      $(wildcard pm/*.c)        \
      $(wildcard sys/*.c)       \
      $(wildcard test/mm.c)

# Object Files
OBJ = $(SRC:.c=.$(OBJ_SUFFIX).o)

#===============================================================================
# Sources (Target Specific)
#===============================================================================

ifeq ($(TARGET),qemu-x86)
SRC += $(wildcard dev/net/rtl8139.c)
SRC += $(wildcard dev/bus/pci.c)
SRC += $(wildcard test/network.c)
endif

ifeq ($(TARGET),qemu-openrisc)
SRC += $(wildcard dev/net/ethoc.c)
SRC += $(wildcard test/network.c)
endif

#===============================================================================

# Builds All Object Files
all: $(OBJ)
ifeq ($(VERBOSE), no)
	@echo [AR] $(LIBKERNEL)
	@$(AR) $(ARFLAGS) $(LIBKERNEL) $(OBJ)
else
	$(AR) $(ARFLAGS) $(LIBKERNEL) $(OBJ)
endif

# Cleans All Object Files
clean:
ifeq ($(VERBOSE), no)
	@echo [CLEAN] $(OBJ)
	@rm -rf $(OBJ)
else
	rm -rf $(OBJ)
endif

# Cleans Everything
distclean: clean
ifeq ($(VERBOSE), no)
	@echo [CLEAN] $(LIBKERNEL)
	@rm -rf $(LIBKERNEL)
else
	rm -rf $(LIBKERNEL)
endif

# Builds a C Source file
%.$(OBJ_SUFFIX).o: %.c
ifeq ($(VERBOSE), no)
	@echo [CC] $@
	@$(CC) $(CFLAGS) $< -c -o $@
else
	$(CC) $(CFLAGS) $< -c -o $@
endif
