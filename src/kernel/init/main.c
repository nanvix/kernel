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
#include <nanvix/dev.h>
#include <nanvix/klib.h>
#include <nanvix/mm.h>
#include <nanvix/thread.h>

EXTERN int main(int argc, const char *argv[], char **envp);

#ifdef HAL_SMP

/**
 * @brief Init thread.
 */
PRIVATE void *init(void *arg)
{
	int status;
	int argc = 1;
	const char *argv[] = { "init", NULL };

	UNUSED(arg);

	status = main(argc, argv, NULL);

	UNUSED(status);

	kprintf("broadcasting shutdown signal");

	return (NULL);
}

#endif

/**
 * @brief Initializes the kernel.
 */
PUBLIC void kmain(int argc, const char *argv[])
{
#ifdef HAL_SMP
	tid_t tid;
#endif

	UNUSED(argc);
	UNUSED(argv);

	/* Master core. */

	dev_init();
	mm_init();

	kprintf("enabling hardware interrupts");
	hal_enable_interrupts();

#ifndef NDEBUG
	hal_test_driver();
#endif

#ifdef HAL_SMP
	thread_create(&tid, init, NULL);
#endif

	while (TRUE)
		noop();
}
