#
# MIT License
#
# Copyright (c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.  THE SOFTWARE IS PROVIDED
# "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
# LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

BINDIR=$1
IMAGE=$2
BIN="test-driver"

#
# Missing target argument.
#
if [ -z $TARGET ]; then
	echo "$0: missing target architecture"
	exit 1
fi

#
# Missing arguments.
#
if [ -z $BINDIR ] ;
then
	echo "missing argument: binary directory"
	exit 1
fi

# Parse target.
case "$TARGET" in
	"mppa256")
		source "tools/image/arch/mppa256.sh"
		BIN="$BIN-k1bio $BIN-k1bdp"
		;;
	"qemu-x86")
		source "tools/image/arch/qemu-x86.sh"
		BIN="$BIN-i486"
		;;
	"optimsoc")
		source "tools/image/arch/optimsoc.sh"
		BIN="$BIN-optimsoc"
		;;
	"qemu-openrisc")
		source "tools/image/arch/qemu-openrisc.sh"
		BIN="$BIN-or1k"
		;;
	"qemu-riscv32")
		source "tools/image/arch/qemu-riscv32.sh"
		BIN="$BIN-riscv32"
		;;
	*)
        echo "error: unsupported target"
		exit 1
		;;
esac

# Build multi-binaries.
build $BINDIR $BIN $IMAGE
