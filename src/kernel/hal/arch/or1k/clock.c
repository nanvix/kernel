/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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
#include <arch/or1k/core.h>
#include <arch/or1k/clock.h>

/**
 * The or1k_clock_init() function initializes the clock driver in the
 * or1k architecture. The frequency of the device is set to @p freq
 * Hz.
 */
PUBLIC void or1k_clock_init(unsigned freq)
{
	unsigned upr;  /* Unit Present Register. */
	unsigned rate; /* Timer rate.            */

	upr = or1k_mfspr(OR1K_SPR_UPR);
	if ( !(upr & OR1K_SPR_UPR_TTP) )
		while (1);

	/* Clock rate. */
	rate = (CPU_FREQUENCY << 2)/freq;

	/* Ensures that the clock is disabled. */
	or1k_mtspr(OR1K_SPR_TTCR, 0);
	or1k_mtspr(OR1K_SPR_TTMR, OR1K_SPR_TTMR_CR | OR1K_SPR_TTMR_IE | rate);
}
