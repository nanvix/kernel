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

# QEMU Version
export QEMU_VERSION=2.12.0

# Working Directory
export CURDIR=`pwd`
export WORKDIR=$CURDIR/nanvix-toolchain

# Number of Cores
NCORES=`grep -c "^processor" /proc/cpuinfo`

# Set working directory.
mkdir -p $WORKDIR
cd $WORKDIR

# Get qemu.
wget "http://wiki.qemu-project.org/download/qemu-$QEMU_VERSION.tar.bz2"

# Build qemu
tar -xjvf qemu-$QEMU_VERSION.tar.bz2
cd qemu-$QEMU_VERSION
./configure --target-list=i386-softmmu,or1k-softmmu,or1k-linux-user --enable-sdl --enable-curses
make -j $NCORES all
make install

# Cleans files.
cd $WORKDIR/..
rm -R -f $WORKDIR
