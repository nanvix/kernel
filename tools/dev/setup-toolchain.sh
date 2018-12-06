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

# Missing target platform.
if [ -z $TARGET ];
then
	echo "$0: missing target platform"
	exit 1
fi

# Parse target platform.
case "$TARGET" in
	# Intel 80386
	"i386")
		bash "tools/dev/arch/i386.sh"
		;;
	# OpenRISC
	"or1k")
		bash "tools/dev/arch/or1k.sh"
		;;
	# Unknown
	*)
		echo "$0: unknown target platform"
		;;
esac
