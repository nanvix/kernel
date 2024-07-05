# Copyright(c) 2011-2024 The Maintainers of Nanvix.
# Licensed under the MIT License.

# Script Arguments
TARGET=$1   # Target
IMAGE=$2    # Image
MODE=$3     # Run Mode
TIMEOUT=$4  # Timeout

# Global Variables
export SCRIPT_NAME=$0
export SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd)"

#==============================================================================
# usage()
#==============================================================================

#
# Prints script usage and exits.
#
function usage
{
	echo "$SCRIPT_NAME <binary> [mode] [timeout]"
	exit 1
}

#==============================================================================
# check_args()
#==============================================================================

# Check script arguments.
function check_args
{
	# Missing binary?
	if [ -z $IMAGE ];
	then
		echo "$SCRIPT_NAME: missing image"
		usage
	fi
}

#==============================================================================
# run_qemu()
#==============================================================================

# Runs a binary in QEMU.
function run_qemu
{
	local target=$1     # Target architecture.
	local image=$2      # Image.
	local mode=$3       # Spawn mode (run or debug).
	local timeout=$4    # Timeout for test mode.
	local GDB_PORT=1234 # GDB port used for debugging.
	local cmd=""

	# Target configuration.
	local MEMSIZE=256M # Memory Size

	if [ $target == "i386" ]; then
		machine="-machine pc"
	fi

	qemu_cmd="$TOOLCHAIN_DIR/qemu/bin/qemu-system-$target
	  		$machine
			-serial stdio
			-display none
			-cpu pentium2
			-m $MEMSIZE
			-mem-prealloc"

	cmd="$qemu_cmd -gdb tcp::$GDB_PORT"
	cmd="$cmd -cdrom $image"

	# Run.
	if [ $mode == "--debug" ];
	then
		cmd="$cmd -S"
		$cmd
	else

		if [ ! -z $timeout ];
		then
			cmd="timeout -s SIGINT --preserve-status --foreground $timeout $cmd"
		fi

		$cmd
	fi
}

#==============================================================================

# No debug mode.
if [ -z $MODE ];
then
	MODE="--no-debug"
fi

# Verbose mode.
if [[ ! -z $VERBOSE ]];
then
	echo "====================================================================="
	echo "TARGET      = $TARGET"
	echo "SCRIPT_DIR  = $SCRIPT_DIR"
	echo "SCRIPT_NAME = $SCRIPT_NAME"
	echo "IMAGE       = $IMAGE"
	echo "MODE        = $MODE"
	echo "TIMEOUT     = $TIMEOUT"
	echo "====================================================================="
fi

case "$TARGET" in
	"x86")
		run_qemu "i386" $IMAGE $MODE $TIMEOUT
		;;
    *)
        echo "Unsupported target: $TARGET"
        ;;
esac
