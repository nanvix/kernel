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

#
# NOTES:
#   - You should run this script with superuser privileges.
#

# Get Linux distribution.
DISTRO=`cat /etc/os-release | grep "^ID="`

# Parse distribution.
case "$DISTRO" in
    *"ubuntu"*|*"debian"*)
        apt-get install -y  \
			bison           \
			ddd             \
			dh-autoreconf   \
			flex            \
			g++             \
			genisoimage     \
			libglib2.0-dev  \
			libncurses5-dev \
			libpixman-1-dev \
			libsdl2-dev     \
			libtool         \
			texinfo         \
			zlib1g-dev
        ;;
    *"arch"*)
        pacman -S --needed \
			autoconf       \
			bison          \
			cdrtools       \
			flex           \
			gcc            \
			glibz2         \
			libtool        \
			ncurses        \
			pixman         \
			sdl2           \
			texinfo        \
			zlib
        ;;
    *)
        echo "Warning: your distro is not officially supported"
esac
