/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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
#include <nanvix/dev.h>
#include <nanvix/klib.h>
#include <nanvix/mm.h>
#include <nanvix/thread.h>
#include <nanvix.h>
#include <dev/net/rtl8139.h>

EXTERN void do_syscall2(void);
EXTERN void ___start(int argc, const char *argv[], char **envp);

#if (CLUSTER_IS_MULTICORE)

/**
 * @brief Init thread.
 */
PRIVATE void *init(void *arg)
{
	int argc = 1;
	const char *argv[] = { "init", NULL };

	UNUSED(arg);

#if (CORES_NUM > 2)

	___start(argc, argv, NULL);

#else

	UNUSED(argc);
	UNUSED(argv);

#endif

	/* Power down. */
	shutdown();
	UNREACHABLE();

	return (NULL);
}

#endif

/**
 * @brief Initializes the kernel.
 */
PUBLIC void kmain(int argc, const char *argv[])
{
#if (CLUSTER_IS_MULTICORE)
	int tid;
#endif

	UNUSED(argc);
	UNUSED(argv);

	hal_init();
	dev_init();
	mm_init();

	kprintf("enabling hardware interrupts");
	interrupts_enable();
	
	uint8_t packet[98] =  
	{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x45, 0x00,
	0x00, 0x54, 0x92, 0x4c, 0x40, 0x00, 0x40, 0x01, 0xc0, 0xf6, 0xc0, 0xa8, 0xb3, 0x0a, 0xc0, 0xa8,
	0xb3, 0x0a, 0x08, 0x00, 0x07, 0xed, 0x24, 0x13, 0x00, 0x01, 0x8c, 0x98, 0x1b, 0x5d, 0x00, 0x00,
	0x00, 0x00, 0x61, 0x36, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
	0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
	0x36, 0x37
	};
	
	for(int i = 0; i < 10; i ++) {
		packet[i] = i;
		net_rtl8139_send_packet(packet, 98);
	}

	while(1){};
#if (CLUSTER_IS_MULTICORE)

	thread_create(&tid, init, NULL);
	while (true)
		do_syscall2();

#else

	/* Power down. */
	shutdown();
	UNREACHABLE();

#endif
}
