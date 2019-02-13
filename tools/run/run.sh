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

# Command Parameters
test=$1  # Target test.
mode=$2  # Test mode.
debug=$3 # Launch GDB?
type=$4  # Target type.

if [ -z $TARGET ]; then
	echo "$0: missing target architecture"
	exit 1
fi

if [ $TARGET == "mppa256" ]; then
	source "tools/run/arch/mppa256.sh"
else
	echo "unknown target architecture"
	exit 1
fi

# Missing parameters.
if [ -z $test ];
then
	echo "Missing arguments!"
	echo "Usage: $0 <test name> [mode]"
	exit 1 
fi

#
# Long test mode.
#
if [ -z $mode ];
then
	mode="--long"
fi

#
# No debug mode.
#
if [ -z $debug ];
then
	debug="--no-debug"
fi

#
# All Types.
#
if [ -z $type ];
then
	type="--all"
fi

#
# Stops regression test if running running short test.
#
function stop_if_short_test
{
	if [ $mode == "--short" ];
	then
		exit 0
	fi
}

case $test in
	all)
		mode="--long"
	;&
	kernel-core)
		echo "=== Running Core and NoC Interface Tests"
		run_hw "nanvix-debug.img" "bin/test-driver" "--debug --hal-core" "$debug" "$type"
		stop_if_short_test $mode
	;&
esac
