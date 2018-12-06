#
# MIT License
#
# Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
#              2016-2018 Davidson Francis     <davidsondfgl@gmail.com>
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
export WORKDIR=$CURDIR/tools/dev/toolchain/or1k
export PREFIX=$WORKDIR
export TARGET=or1k-elf

mkdir -p $WORKDIR
cd $WORKDIR

# Retrieve the number of processor cores
NCORES=`grep -c ^processor /proc/cpuinfo`

if [ ! "$(ls -A $WORKDIR)" ];
then
	git submodule update --init .
fi

# Build binutils and GDB.
cd binutils*/
./configure --target=$TARGET --prefix=$PREFIX --disable-nls --disable-sim --with-auto-load-safe-path=/ --enable-tui --with-guile=no
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

# GCC for Linux
cd $WORKDIR
wget "https://github.com/openrisc/musl-cross/releases/download/gcc5.3.0-musl1.1.14/or1k-linux-musl_gcc5.3.0_binutils2.26_musl1.1.14.tgz"
tar -xvf or1k-linux-musl_gcc5.3.0_binutils2.26_musl1.1.14.tgz

# Back to the current folder
cd $CURDIR
