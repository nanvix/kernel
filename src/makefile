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
# Conflicts
#===============================================================================

.PHONY: kernel
.PHONY: libs
.PHONY: test

#===============================================================================

# Builds Everything
all: kernel test

# Builds the Kernel
kernel:
	$(MAKE) -C kernel all

# Builds User Libraries
libs: kernel
	$(MAKE) -C libs all

# Builds The Test Driver
test: libs kernel
	$(MAKE) -C test all

# Cleans Build Objects
clean: clean-kernel clean-libs clean-test

# Cleans Kernel Build Objects
clean-kernel:
	$(MAKE) -C kernel clean

# Cleans User Libraries Build Objects
clean-libs:
	$(MAKE) -C libs clean

# Cleans Test Driver Build Objects
clean-test:
	$(MAKE) -C test clean

# Cleans Everything
distclean: distclean-kernel distclean-libs distclean-test

# Cleans Kernel Build
distclean-kernel:
	$(MAKE) -C kernel distclean

# Cleans User Libraries Build
distclean-libs:
	$(MAKE) -C libs distclean

# Cleans Test Driver Build
distclean-test:
	$(MAKE) -C test distclean
