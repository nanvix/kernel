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

#include <HAL/hal/board/boot_args.h>
#include <mOS_vcore_u.h>

#include <arch/k1b/cache.h>
#include <arch/k1b/core.h>
#include <arch/k1b/cpu.h>
#include <arch/k1b/excp.h>
#include <arch/k1b/int.h>
#include <arch/k1b/ivt.h>
#include <arch/k1b/tlb.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/**
 * @brief Cores.
 */
PRIVATE struct
{
	int initialized;        /**< Initialized core? */
	int state;              /**< State.            */
	void (*start)(void);    /**< Starting routine. */
	spinlock_t lock;        /**< Lock.             */
} __attribute__((aligned(K1B_CACHE_LINE_SIZE))) cores[K1B_NUM_CORES] = {
	{ TRUE,  K1B_CORE_RUNNING,  NULL, K1B_SPINLOCK_UNLOCKED }, /* Master Core   */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 1  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 2  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 3  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 4  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 5  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 6  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 7  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 8  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 9  */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 10 */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 11 */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 12 */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 13 */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 14 */
	{ FALSE, K1B_CORE_SLEEPING, NULL, K1B_SPINLOCK_UNLOCKED }, /* Slave Core 15 */
};

/*============================================================================*
 * k1b_core_sleep()                                                           *
 *============================================================================*/

/**
 * The k1b_core_sleep() function stops instruction execution in the
 * the underlying core and places it in a low-power state. An wakeup
 * notification resumes execution.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_sleep(void)
{
	int coreid = k1b_core_get_id();

	k1b_spinlock_lock(&cores[coreid].lock);

		cores[coreid].state = K1B_CORE_SLEEPING;

	k1b_dcache_inval();
	k1b_spinlock_unlock(&cores[coreid].lock);

	/* Wait for wakeup. */
	do
		k1b_cpu_wait();
	while (cores[coreid].state == K1B_CORE_SLEEPING);
}

/*============================================================================*
 * k1b_core_wakeup()                                                          *
 *============================================================================*/

/**
 * The k1b_core_wakeup() function sends a wakeup notification to the
 * sleeping core whose ID equals @coreid and sets the starting routine
 * of this target core to @p start.
 *
 * @todo Check if the calling core is not the target core.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_wakeup(int coreid, void (*start)(void))
{
	k1b_spinlock_lock(&cores[coreid].lock);
	k1b_dcache_inval();

		/* Wakeup core. */
		if (cores[coreid].state == K1B_CORE_SLEEPING)
		{
			cores[coreid].state = K1B_CORE_RUNNING;
			cores[coreid].start = start;
			k1b_dcache_inval();
		}

	k1b_spinlock_unlock(&cores[coreid].lock);

	k1b_cpu_notify(coreid);
}

/*============================================================================*
 * k1b_core_start()                                                           *
 *============================================================================*/

/**
 * The k1b_core_start() function starts the underlying core by calling
 * the starting routine previously set by a call to k1b_core_wakeup(),
 * made by the master core. Furthermore, in the first call ever made
 * to k1b_core_start(), architectural structures of the slave core are
 * initialized.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_start(void)
{
	int coreid = k1b_core_get_id();

	k1b_spinlock_lock(&cores[coreid].lock);
	k1b_dcache_inval();

		/* Initialize core. */
		if (!cores[coreid].initialized)
		{
			k1b_core_setup();
			cores[coreid].initialized = TRUE;
			k1b_dcache_inval();
		}

	k1b_spinlock_unlock(&cores[coreid].lock);
	
	cores[coreid].start();
}

/*============================================================================*
 * k1b_core_shutdown()                                                        *
 *============================================================================*/

/**
 * The k1b_core_shutdown() function powers off the underlying core
 * with status @p status.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_shutdown(int status)
{
	int coreid = k1b_core_get_id();

	k1b_spinlock_lock(&cores[coreid].lock);

		cores[coreid].state = K1B_CORE_OFFLINE;

	k1b_dcache_inval();
	k1b_spinlock_unlock(&cores[coreid].lock);

	mOS_exit(__k1_spawn_type() != __MPPA_MPPA_SPAWN, status);
}
