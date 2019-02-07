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
#include <arch/k1b/spinlock.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/* These are written in assembly. */
EXTERN NORETURN void _k1b_core_reset(void);

/**
 * @brief Event line used for signals.
 */
#define K1B_EVENT_LINE 0

/**
 * @brief Cores table.
 */
PRIVATE struct
{
	int initialized;        /**< Initialized?      */
	int state;              /**< State.            */
	int wakeups;            /**< Wakeup signals.   */
	void (*start)(void);    /**< Starting routine. */
	spinlock_t lock;        /**< Lock.             */
} __attribute__((aligned(K1B_CACHE_LINE_SIZE))) cores[K1B_NUM_CORES] = {
	{ TRUE,  K1B_CORE_RUNNING,  0, NULL, K1B_SPINLOCK_UNLOCKED }, /* Master Core   */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 1  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 2  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 3  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 4  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 5  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 6  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 7  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 8  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 9  */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 10 */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 11 */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 12 */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 13 */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 14 */
	{ FALSE, K1B_CORE_RESETTING, 0, NULL, K1B_SPINLOCK_LOCKED },  /* Slave Core 15 */
};

/*============================================================================*
 * k1b_core_notify()                                                          *
 *============================================================================*/

/**
 * @brief Sends a signal.
 *
 * The k1b_core_notify() function sends a signal to the core whose ID
 * equals to @p coreid.
 *
 * @param coreid ID of the target core.
 *
 * @bug No sanity check is performed in @p coreid.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE inline void k1b_core_notify(int coreid)
{
	mOS_pe_notify(
		1 << coreid,    /* Target cores.                            */
		K1B_EVENT_LINE, /* Event line.                              */
		1,              /* Notify an event? (I/O clusters only)     */
		0               /* Notify an interrupt? (I/O clusters only) */
	);
}

/*============================================================================*
 * k1b_core_idle()                                                            *
 *============================================================================*/

/**
 * The k1b_core_idle() function suspends instruction execution in the
 * underlying core until a start signal is received. While is
 * suspended mode, the underlying core is placed in a low-power state
 * to save energy.
 *
 * @see k1b_core_start(), k1b_core_run().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_idle(void)
{
		int coreid = k1b_core_get_id();

		cores[coreid].state = K1B_CORE_IDLE;

		/*
		 * The lock of this core was
		 * acquired when resetting, in
		 * k1b_core_reset().
		 */

	k1b_dcache_inval();
	k1b_spinlock_unlock(&cores[coreid].lock);

	while (TRUE)
	{
		k1b_spinlock_lock(&cores[coreid].lock);
		k1b_dcache_inval();

			/* Awaken. */
			if (cores[coreid].state != K1B_CORE_IDLE)
			{
				k1b_spinlock_unlock(&cores[coreid].lock);
				break;
			}

			mOS_pe_event_clear(K1B_EVENT_LINE);

		k1b_dcache_inval();
		k1b_spinlock_unlock(&cores[coreid].lock);

		mOS_pe_event_waitclear(K1B_EVENT_LINE);
	}

	kprintf("[hal] core awaken");
}

/*============================================================================*
 * k1b_core_sleep()                                                           *
 *============================================================================*/

/**
 * The k1b_core_sleep() function suspends instruction execution in the
 * underlying core until a wakeup signal is received. While is
 * suspended mode, the underlying core is placed in a low-power state
 * to save energy.
 *
 * @see k1b_core_wakeup().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_sleep(void)
{
	int coreid = k1b_core_get_id();

	while (TRUE)
	{
		k1b_spinlock_lock(&cores[coreid].lock);
		k1b_dcache_inval();

			/* Awaken. */
			if (cores[coreid].wakeups > 0)
			{
				cores[coreid].wakeups--;
				cores[coreid].state = K1B_CORE_RUNNING;

				k1b_dcache_inval();
				k1b_spinlock_unlock(&cores[coreid].lock);

				break;
			}

			cores[coreid].state = K1B_CORE_SLEEPING;
			mOS_pe_event_clear(K1B_EVENT_LINE);

		k1b_dcache_inval();
		k1b_spinlock_unlock(&cores[coreid].lock);

		mOS_pe_event_waitclear(K1B_EVENT_LINE);
	}
}

/*============================================================================*
 * k1b_core_wakeup()                                                          *
 *============================================================================*/

/**
 * The k1b_core_wakeup() function sends a wakeup signal to the
 * sleeping core whose ID equals to @p coreid.
 *
 * @see k1b_core_sleep().
 *
 * @todo Check if the calling core is not the target core.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_wakeup(int coreid)
{
	k1b_spinlock_lock(&cores[coreid].lock);
	k1b_dcache_inval();

		/* Wakeup target core. */
		cores[coreid].wakeups++;
		k1b_core_notify(coreid);

	k1b_dcache_inval();
	k1b_spinlock_unlock(&cores[coreid].lock);
}

/*============================================================================*
 * k1b_core_start()                                                          *
 *============================================================================*/

/**
 * The k1b_core_start() function sets the starting routine of the
 * sleeping core whose ID equals to @p coreid to @p start and sends a
 * wakeup signal to this core.
 *
 * @see k1b_core_idle(), k1b_core_run().
 *
 * @todo Check if the calling core is not the target core.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_start(int coreid, void (*start)(void))
{
again:

	k1b_spinlock_lock(&cores[coreid].lock);
	k1b_dcache_inval();

	/* Wait for reset. */
	if (cores[coreid].state == K1B_CORE_RESETTING)
	{
		k1b_spinlock_unlock(&cores[coreid].lock);
		goto again;
	}

	/* Wakeup target core. */
	if (cores[coreid].state == K1B_CORE_IDLE)
	{
		cores[coreid].state = K1B_CORE_RUNNING;
		cores[coreid].start = start;
		cores[coreid].wakeups = 0;
		k1b_dcache_inval();

		k1b_core_notify(coreid);
	}

	k1b_spinlock_unlock(&cores[coreid].lock);
}

/*============================================================================*
 * k1b_core_run()                                                             *
 *============================================================================*/

/**
 * The k1b_core_run() function resumes instruction execution in the
 * underlying core, by calling the starting routine which was
 * previously registered with k1b_core_wakeup(). Furthermore, in the
 * first call ever made to k1b_core_run(), architectural structures of
 * the underlying core are initialized.
 *
 * @see k1b_core_idle(), k1b_core_start().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_run(void)
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
 * k1b_core_reset()                                                           *
 *============================================================================*/

/**
 * The k1b_core_reset() function resets execution instruction in
 * the underlying core by reseting the kernel stack to its initial
 * location and relaunching the k1b_slave_setup() function.
 *
 * @note This function does not return.
 *
 * @see k1b_slave_setup()
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_reset(void)
{
	int coreid = k1b_core_get_id();

	k1b_spinlock_lock(&cores[coreid].lock);
	k1b_dcache_inval();

		cores[coreid].state = K1B_CORE_RESETTING;

		k1b_dcache_inval();

		kprintf("[hal] resetting core");

		_k1b_core_reset();

		/*
		 * The lock of this core will
		 * be released when resetting
		 * is completed, in k1b_core_idle().
		 */
}

/*============================================================================*
 * k1b_core_shutdown()                                                        *
 *============================================================================*/

/**
 * The k1b_core_shutdown() function powers off the underlying core.
 * Afeter powering off a core, instruction execution cannot be
 * resumed. The status code @p status is handled to the remote spawner
 * device.
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
