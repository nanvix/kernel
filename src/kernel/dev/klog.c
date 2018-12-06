/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2017 Clement Rouquier <clementrouquier@gmail.com>
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

#include <nanvix/hal/debug.h>
#include <nanvix/hal/memory.h>
#include <nanvix/hal/cpu.h>
#include <nanvix/const.h>
#include <nanvix/dev.h>
#include <nanvix/klib.h>
#include <nanvix/klog.h>
#include <stdint.h>

/**
 * @brief Kernel log.
 */
PRIVATE struct
{
	size_t head;         /**< First element in the buffer.  */
	size_t tail;         /**< Next free slot in the buffer. */
	char buf[KLOG_SIZE]; /**< Ring buffer.                  */
} klog = { 0, 0, {0, }};

/**
 * @brief Flushes the kernel log buffer.
 */
PRIVATE void klog_flush(void)
{	
	ssize_t i;              /* Number of bytes to flush. */
	char buf[KBUFFER_SIZE]; /* Temporary buffer.         */

	/* No data, so nothing to do. */
	if (klog.head == klog.tail)
		return;

	/* Copy data from ring buffer. */
	for (i = 0; i < KBUFFER_SIZE; noop())
	{
		buf[i] = klog.buf[klog.head];

		i++;
		klog.head = (klog.head + 1)%KLOG_SIZE;
		hal_dcache_invalidate();

		if (klog.head == klog.tail)
			break;
	}
	
	hal_stdout_write(buf, i);
}

/**
 * @brief Writes to kernel log.
 * 
 * @param buffer Target buffer.
 * @param n      Number of characters to write.
 */
PUBLIC void klog_write(const char *buf, size_t n)
{
	/* Copy data to ring buffer. */
	if (hal_processor_get_core_id() == 0)
	{
		for (size_t i = 0; i < n; i++)
		{
			klog.buf[klog.tail] = buf[i];
			hal_dcache_invalidate();

			klog.tail = (klog.tail + 1)%KLOG_SIZE;
			hal_dcache_invalidate();
		}
	}

	hal_stdout_write(buf, n);
}

/**
 * @brief Setups the kernel log device.
 */
PUBLIC void klog_setup(void)
{
	hal_stdout_init();

	klog_flush();
}
