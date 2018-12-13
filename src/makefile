#
# MIT License
#
# Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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
# Toolchain Configuration
#===============================================================================

# Compiler Options.
export CFLAGS  += -std=c99 -fno-builtin # -ansi
export CFLAGS  += -Wall -Wextra -Werror -Wa,--warn # -pedantic-errors
export CFLAGS  += -Winit-self -Wswitch-default -Wfloat-equal
export CFLAGS  += -Wundef -Wshadow -Wuninitialized -Wlogical-op
export CFLAGS  += -Wvla # -Wredundant-decls
export CFLAGS  += -I $(INCDIR)
ifeq ($(RELEASE), true)
	export CFLAGS  += -O3 -NDEBUG
else
	export CFLAGS  += -Og -g
endif


# Linker Options
export LDFLAGS +=

# Archiver Options
export ARFLAGS = rc

#===============================================================================
# Binaries and Libraries
#===============================================================================

export EXECBIN = test-driver

#===============================================================================

# Conflicts
.PHONY: kernel
.PHONY: libs
.PHONY: test

# Builds everything.
all: kernel libs
	$(MAKE) -C test $(TARGET)

# Builds the kernel.
kernel: 
	$(MAKE) -C kernel all

# Builds system libraries.
libs:
	$(MAKE) -C libs all

# Cleans compilation files.
distclean:
	$(MAKE) -C kernel clean
	$(MAKE) -C libs clean
	$(MAKE) -C test clean
