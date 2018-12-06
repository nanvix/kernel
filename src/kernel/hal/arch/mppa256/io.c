/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#include <arch/k1b/core.h>
#include <arch/k1b/elf.h>
#include <arch/k1b/io.h>
#include <arch/k1b/util.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <vbsp.h>

/**
 * @brief JTAG Write system call.
 */
#define __NR_jtag_write 4094

/**
 * @brief Writes data to the JTAG device.
 *
 * @param buf Buffer.
 * @param n   Number of bytes to write.
 */
void hal_jtag_write(const uint8_t *buf, size_t n)
{
	size_t k;
	char jtag_buf[JTAG_BUFSIZE];

	k = (n > JTAG_BUFSIZE) ? JTAG_BUFSIZE : n;

	kmemcpy(jtag_buf, (const char *)buf, k);

	__k1_club_syscall2(__NR_jtag_write, (unsigned) jtag_buf, k);
}
