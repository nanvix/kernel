/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <nanvix/syscall.h>
#include <sys/types.h>
#include <errno.h>

/**
 * @brief Writes data to a file.
 *
 * @param fd  File descriptor.
 * @param buf Target buffer.
 * @param n   Number of bytes to write.
 *
 * @returns Upon successful completion, the number of bytes written is
 * returned. Upon failure, -1 is returned and @p errno is set to
 * indicate the error.
 */
ssize_t nanvix_write(int fd, const char *buf, size_t n)
{
	register unsigned arg0
		__asm__("r3") = (unsigned) fd;
	register unsigned arg1
		__asm__("r4") = (unsigned) buf;
	register unsigned arg2
		__asm__("r5") = (unsigned) n;

	register ssize_t ret
		__asm__("r11") = NR_write;

	__asm__ volatile (
		"l.sys 1"
		: "=r" (ret)
		: "r" (ret),
		"r" (arg0),
		"r" (arg1),
		"r" (arg2)
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}
