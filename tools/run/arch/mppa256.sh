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

export K1_TOOLCHAIN_DIR="/usr/local/k1tools"
export ARCH=k1b
export BOARD=developer

#
# Runs a multi-binary file in a single compute cluster in the hardware.
#
function run_hw
{
	local multibin=$1
	local bin=$2
	local args=$3
	local debug=$4

	if [ $debug == "--no-debug" ];
	then
		$K1_TOOLCHAIN_DIR/bin/k1-jtag-runner \
			--multibinary=$multibin          \
			--exec-file=Cluster0:$bin        \
			-- $args
	else
		$K1_TOOLCHAIN_DIR/bin/k1-jtag-runner \
			--gdb                            \
			--multibinary=$multibin          \
			--exec-file=Cluster0:$bin        \
			-- $args
	fi
}

#
# Runs a binary file in a single compute cluster in the simulator.
#
function run_sim
{
	local bin=$1
	local args=$2

	$K1_TOOLCHAIN_DIR/bin/k1-cluster \
		--mboard=$BOARD           \
		--march=$ARCH             \
		--bootcluster=node0       \
		-- $bin $args
}
