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

#
# Release version.
#
VERSION=v0.1-beta.1

#===============================================================================
# Directories
#===============================================================================

# Directories
export BINDIR   = $(CURDIR)/bin
export MAKEDIR  = $(CURDIR)/build
export DOCDIR   = $(CURDIR)/doc
export INCDIR   = $(CURDIR)/include
export LIBDIR   = $(CURDIR)/lib
export SRCDIR   = $(CURDIR)/src
export TOOLSDIR = $(CURDIR)/tools

#===============================================================================

# Image Name
export IMAGE = nanvix-debug.img

# Builds everything.
all: image

# Builds a release.
release: image
	tar -cjvf nanvix-$(VERSION).tar.bz2 \
		build/makefile.mppa256          \
		include                         \
		lib                             \
		scripts/arch/mppa256.sh

# Builds image.
image: nanvix
	bash $(TOOLSDIR)/image/build-image.sh $(BINDIR) $(IMAGE)

# Builds binaries and libraries.
nanvix:
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	$(MAKE) -C $(SRCDIR) all

# Cleans everything.
distclean:
	$(MAKE) -C $(SRCDIR) distclean
	rm -rf $(IMAGE)
	rm -rf $(BINDIR) $(LIBDIR)

# Builds documentation.
documentation:
	mkdir -p $(DOCDIR)
	( sed -e "s/ENABLED_SECTIONS=/ENABLED_SECTIONS=$(TARGET)/g"  doxygen/doxygen.conf ) | doxygen -

