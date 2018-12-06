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

#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <nanvix/hal/clock.h>
#include <nanvix/hal/interrupt.h>
#include <nanvix/hal/memory.h>

/**
 * @brief Clock interrupts since system initialization.
 */
PRIVATE unsigned ticks = 0;

/**
 * @brief Handles a clock interrupt.
 *
 * @param num Number of interrupt (currently unused).
 */
PRIVATE void do_clock(int num)
{
	UNUSED(num);

	if (hal_processor_get_core_id() == 0)
	{
		ticks++;
		hal_dcache_invalidate();
		kprintf("tick %d!", ticks);
	}
}

/**
 * The clock_init() function initializes the hardware independent
 * clock driver. First, it invokes the hardware dependent driver
 * routine to bring up the device and adjust the operating frequency;
 * and then it registers the clock interrupt handler.
 */
PUBLIC void clock_init(unsigned freq)
{
	hal_clock_init(freq);
	KASSERT(interrupt_register(HAL_INT_CLOCK, do_clock) == 0);
}
