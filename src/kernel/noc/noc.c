/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

#include <nanvix/kernel/noc.h>
#include <nanvix/kernel/portal.h>
#include <nanvix/kernel/mailbox.h>
#include <nanvix/kernel/sync.h>
#include <nanvix/hlib.h>

/**
 * The noc_init() function initializes the Network-on-Chip (NoC)
 * system. It initializes and setups the IKC structures: (i) Sync.
 *
 * @see ksync_init().
 * @see kmailbox_init().
 *
 * @author Joao Fellipe Uller
 */
PUBLIC void noc_init(void)
{
	kprintf("[kernel][noc] initializing the noc system");

	#if __TARGET_HAS_SYNC
		ksync_init();
	#endif

	#if __TARGET_HAS_MAILBOX
		kmailbox_init();
	#endif

	#if __TARGET_HAS_PORTAL
		kportal_init();
	#endif
}
