#
# MIT License
#
# Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
#              2016-2018 Davidson Francis     <davidsondfgl@gmail.com>
#              2016-2017 Subhra Sarkar        <rurtle.coder@gmail.com>
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

# Required variables.
export CURDIR=`pwd`
export WORKDIR=$CURDIR/tools/dev/toolchain/i386
export PREFIX=$WORKDIR
export TARGET=i386-elf

mkdir -p $WORKDIR
cd $WORKDIR

# Retrieve the number of processor cores
NCORES=`grep -c ^processor /proc/cpuinfo`

# Get binutils, GDB and GCC.
if [ ! "$(ls -A $WORKDIR)" ];
then
	git submodule update --init $WORKDIR
fi

# Build binutils.
cd binutils*/
./configure --target=$TARGET --prefix=$PREFIX --disable-nls
make -j $NCORES all
make install
git checkout .
git clean -f -d

# Build GCC.
cd $WORKDIR
cd gcc*/
./contrib/download_prerequisites
mkdir build
cd build
../configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make -j $NCORES all-gcc
make -j $NCORES all-target-libgcc
make install-gcc
make install-target-libgcc
git checkout .
git clean -f -d

# Build GDB.
cd $WORKDIR
cd gdb*/
./configure --target=$TARGET --prefix=$PREFIX --with-auto-load-safe-path=/ --with-guile=no
make -j $NCORES
make install
git checkout .
git clean -f -d

# Back to the current folder
cd $CURDIR
