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

#include <mOS_vcore_u.h>
#include <nanvix/const.h>

/**
 * Invalidates the data cache of the underlying core.
 *
 * @cond mppa256
 */
PUBLIC void hal_dcache_invalidate(void)
{
	__builtin_k1_wpurge();
	__builtin_k1_fence();
	__builtin_k1_dinval();
}
/**
 * @endcond
 */

/**
 * @brief Enables interrupts in the underlying core.
 */
PUBLIC void hal_enable_interrupts(void)
{
	mOS_set_it_level(0);
	mOS_it_enable();
}

/**
 * @brief Disables interrupts in the underlying core.
 */
PUBLIC void hal_disable_interrupts(void)
{
	mOS_it_disable();
}

/**
 * @brief Gets the ID of the underlying core.
 *
 * @returns The ID of the underlying core.
 */
PUBLIC int get_core_id(void)
{
	return (__k1_get_cpu_id());
}

