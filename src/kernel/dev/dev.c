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
#include <dev/pci.h>
#include <dev/net/rtl8139.h>

/**
 * @brief Timer frequency.
 */
#define TIMER_FREQ 30

/**
 * @brief Timer interrupts since system initialization.
 */
PRIVATE unsigned ticks = 0;

/**
 * @brief Handles a timer interrupt.
 *
 * @param num Number of interrupt (currently unused).
 */
PRIVATE void do_timer(int num)
{
	UNUSED(num);

	if (core_get_id() == 0)
	{
		ticks++;
		dcache_invalidate();
	}
}

/**
 * Initializes the device management subsystem.
 */
PUBLIC void dev_init(void)
{
	timer_init(TIMER_FREQ);
	KASSERT(interrupt_register(INTERRUPT_TIMER, do_timer) == 0);

	dev_pci_init();
	net_rtl8139_init();
	char* message = "TOTO";
	net_rtl8139_send_packet(message, 4);
}
