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
	
	network_test_driver();

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
