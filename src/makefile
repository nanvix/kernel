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

# Cleans Everything
clean: clean-kernel clean-libs clean-test

# Cleans Kernel Build
clean-kernel:
	$(MAKE) -C kernel clean

# Cleans User Libraries Build
clean-libs:
	$(MAKE) -C libs clean

# Cleans Test Driver Build
clean-test:
	$(MAKE) -C test clean
