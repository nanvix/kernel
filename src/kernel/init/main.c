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

#include <nanvix/hal/hal.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <nanvix/mm.h>

EXTERN int main(int argc, const char *argv[], char **envp);
EXTERN void dev_init(void);

#ifdef __mppa256__

/**
 * @brief Init thread.
 */
PRIVATE void init(void)
{
	int status;
	int argc = 1;
	const char *argv[] = { "init", NULL };

	status = main(argc, argv, NULL);

	UNUSED(status);
}

#endif


/**
 * @brief Initializes the kernel.
 */
PUBLIC void kmain(int argc, const char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

#if defined(__mppa256__)

	int coreid;

	coreid = hal_core_get_id();

	/* Slave core. */
	if (coreid != 0)
	{
		do
		{
			core_sleep();
			kprintf("waking up core %d", coreid);
			core_start();
			kprintf("halting core %d", coreid);
		} while(1);
	}
#endif

	/* Master core. */

	dev_init();
	mm_init();

	kprintf("enabling hardware interrupts");
	hal_enable_interrupts();

#ifndef NDEBUG
	hal_test_driver();
#endif

#if defined(__mppa256__)
	init();
#endif

	while (TRUE)
		noop();
}
