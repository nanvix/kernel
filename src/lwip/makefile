#
# MIT License
#
# Copyright(c) 2011-2020 The Maintainers of Nanvix
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
# Binaries Soucers and Objects
#===============================================================================

# Binary
ARCHIVE = $(LIBLWIP)

# LWIP folder
LWIPDIR = src

# Local includes 
CFLAGS += -I $(CURDIR)/src/include
CFLAGS += -I $(INCDIR)/posix

include src/Filelists.mk

# C Source Files
SRC = $(COREFILES) $(CORE4FILES) \
      src/arch/nic_driver_if.c   \
      src/arch/sys_arch.c        \
      src/netif/ethernet.c       \
      $(wildcard src/string/*.c)

# Object Files
OBJ = $(SRC:.c=.$(OBJ_SUFFIX).o)

#===============================================================================

include $(BUILDDIR)/makefile.archive
