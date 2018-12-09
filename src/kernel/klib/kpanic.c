/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#include <nanvix/hal/hal.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <stdarg.h>

/**
 * The kpanic() function writes the formatted message pointed to by @p
 * fmt to the standard output device and panics the kernel. In panic
 * mode, all interrupts are disabled in the underlying core, and
 * execution loops indefinitely.
 */
PUBLIC void kpanic(const char *fmt, ...)
{
	size_t len;                    /* String length.           */
	va_list args;                  /* Variable arguments list. */
	char buffer[KBUFFER_SIZE + 1]; /* Temporary buffer.        */
	
	kstrncpy(buffer, "PANIC: ", 7);
	
	/* Convert to raw string. */
	va_start(args, fmt);
	len = kvsprintf(buffer + 7, fmt, args) + 7;
	buffer[len++] = '\n';
	buffer[len++] = '\0';
	va_end(args);

	kputs(buffer);

	/* I don't want to be troubled. */
	hal_disable_interrupts();

	/* Stay here, forever. */
	while (TRUE)
		noop();
}
