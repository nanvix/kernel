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

#include <arch/or1k/core.h>
#include <arch/or1k/cache.h>
#include <arch/or1k/cpu.h>
#include <arch/or1k/ompic.h>
#include <arch/or1k/pic.h>
#include <arch/or1k/spinlock.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

PRIVATE int pending_ipis[OR1K_NUM_CORES] = {0};

/* These are written in assembly. */
EXTERN NORETURN void _or1k_core_reset(void);

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
} __attribute__((aligned(OR1K_CACHE_LINE_SIZE))) cores[OR1K_NUM_CORES] = {
	{ TRUE,  OR1K_CORE_RUNNING,   0, NULL, OR1K_SPINLOCK_UNLOCKED }, /* Master Core   */
	{ FALSE, OR1K_CORE_RESETTING, 0, NULL, OR1K_SPINLOCK_UNLOCKED }, /* Slave Core 1  */
};

/*============================================================================*
 * or1k_core_clear()                                                          *
 *============================================================================*/

/**
 * @brief Clears the current IPIs pending of the underlying core.
 *
 * @author Davidson Francis
 */
PRIVATE inline void or1k_core_clear(void)
{
	int mycoreid = or1k_core_get_id();

	/*
	 * Although pending_ipis should only be used
	 * within a critical section, this is already
	 * done by the caller function, hence, there's
	 * no need to do locks here.
	 */

	/* Clear pending IPIs in the current core. */
	pending_ipis[mycoreid] = 0;
}

/*============================================================================*
 * or1k_core_waitclear()                                                      *
 *============================================================================*/

/**
 * @brief Wait and clears the current IPIs pending of the underlying core.
 *
 * @author Davidson Francis
 */
PRIVATE inline void or1k_core_waitclear(void)
{
	int mycoreid = or1k_core_get_id();

	while (TRUE)
	{
		or1k_spinlock_lock(&cores[mycoreid].lock);

			if (pending_ipis[mycoreid])
				break;

		or1k_spinlock_unlock(&cores[mycoreid].lock);
	}

		/* Clear IPI. */
		for (int i = 0; i < OR1K_NUM_CORES; i++)
		{
			if (pending_ipis[mycoreid] & (1 << i))
			{
				pending_ipis[mycoreid] &= ~(1 << i);
				break;
			}
		}

	or1k_spinlock_unlock(&cores[mycoreid].lock);
}

/*============================================================================*
 * or1k_core_notify()                                                          *
 *============================================================================*/

/**
 * @brief Sends a signal.
 *
 * The or1k_core_notify() function sends a signal to the core whose ID
 * equals to @p coreid.
 *
 * @param coreid ID of the target core.
 *
 * @bug No sanity check is performed in @p coreid.
 *
 * @author Davidson Francis
 */
PRIVATE inline void or1k_core_notify(int coreid)
{
	int mycoreid = or1k_core_get_id();

	or1k_spinlock_lock(&cores[coreid].lock);

		/* Set the pending IPI flag. */
		pending_ipis[coreid] |= (1 << mycoreid);

	or1k_spinlock_unlock(&cores[coreid].lock);
}

/*============================================================================*
 * or1k_core_idle()                                                            *
 *============================================================================*/

/**
 * The or1k_core_idle() function suspends instruction execution in the
 * underlying core until a start signal is received. While is
 * suspended mode, the underlying core is placed in a low-power state
 * to save energy.
 *
 * @see or1k_core_start(), or1k_core_run().
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_core_idle(void)
{
	int coreid = or1k_core_get_id();

	or1k_spinlock_lock(&cores[coreid].lock);
	or1k_dcache_inval();

		cores[coreid].state = OR1K_CORE_IDLE;

	or1k_dcache_inval();
	or1k_spinlock_unlock(&cores[coreid].lock);


	while (TRUE)
	{
		or1k_spinlock_lock(&cores[coreid].lock);
		or1k_dcache_inval();

			/* Awaken. */
			if (cores[coreid].state != OR1K_CORE_IDLE)
			{
				or1k_spinlock_unlock(&cores[coreid].lock);
				break;
			}

			or1k_core_clear();

		or1k_dcache_inval();
		or1k_spinlock_unlock(&cores[coreid].lock);

		or1k_core_waitclear();
	}
}

/*============================================================================*
 * or1k_core_sleep()                                                           *
 *============================================================================*/

/**
 * The or1k_core_sleep() function suspends instruction execution in the
 * underlying core until a wakeup signal is received. While is
 * suspended mode, the underlying core is placed in a low-power state
 * to save energy.
 *
 * @see or1k_core_wakeup().
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_core_sleep(void)
{
	int coreid = or1k_core_get_id();

	while (TRUE)
	{
		or1k_spinlock_lock(&cores[coreid].lock);
		or1k_dcache_inval();

			/* Awaken. */
			if (cores[coreid].wakeups > 0)
			{
				cores[coreid].wakeups--;
				cores[coreid].state = OR1K_CORE_RUNNING;

				or1k_dcache_inval();
				or1k_spinlock_unlock(&cores[coreid].lock);

				break;
			}

			cores[coreid].state = OR1K_CORE_SLEEPING;
			or1k_core_clear();

		or1k_dcache_inval();
		or1k_spinlock_unlock(&cores[coreid].lock);

		or1k_core_waitclear();
	}
}

/*============================================================================*
 * or1k_core_wakeup()                                                          *
 *============================================================================*/

/**
 * The or1k_core_wakeup() function sends a wakeup signal to the
 * sleeping core whose ID equals to @p coreid.
 *
 * @see or1k_core_sleep().
 *
 * @todo Check if the calling core is not the target core.
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_core_wakeup(int coreid)
{
	or1k_spinlock_lock(&cores[coreid].lock);
	or1k_dcache_inval();

		/* Wakeup target core. */
		cores[coreid].wakeups++;
		or1k_core_notify(coreid);

	or1k_dcache_inval();
	or1k_spinlock_unlock(&cores[coreid].lock);
}

/*============================================================================*
 * or1k_core_start()                                                          *
 *============================================================================*/

/**
 * The or1k_core_start() function sets the starting routine of the
 * sleeping core whose ID equals to @p coreid to @p start and sends a
 * wakeup signal to this core.
 *
 * @see or1k_core_idle(), or1k_core_run().
 *
 * @todo Check if the calling core is not the target core.
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_core_start(int coreid, void (*start)(void))
{
again:

	or1k_spinlock_lock(&cores[coreid].lock);
	or1k_dcache_inval();

	/* Wait for reset. */
	if (cores[coreid].state == OR1K_CORE_RESETTING)
	{
		or1k_spinlock_unlock(&cores[coreid].lock);
		goto again;
	}

	/* Wakeup target core. */
	if (cores[coreid].state == OR1K_CORE_IDLE)
	{
		cores[coreid].state = OR1K_CORE_RUNNING;
		cores[coreid].start = start;
		cores[coreid].wakeups = 0;
		or1k_dcache_inval();

		or1k_spinlock_unlock(&cores[coreid].lock);
		or1k_core_notify(coreid);
	}

	or1k_spinlock_unlock(&cores[coreid].lock);
}

/*============================================================================*
 * or1k_core_run()                                                             *
 *============================================================================*/

/**
 * The or1k_core_run() function resumes instruction execution in the
 * underlying core, by calling the starting routine which was
 * previously registered with or1k_core_wakeup(). Furthermore, in the
 * first call ever made to or1k_core_run(), architectural structures of
 * the underlying core are initialized.
 *
 * @see or1k_core_idle(), or1k_core_start().
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_core_run(void)
{
	int coreid = or1k_core_get_id();

	or1k_spinlock_lock(&cores[coreid].lock);
	or1k_dcache_inval();

		/* Initialize core. */
		if (!cores[coreid].initialized)
		{
			or1k_core_setup();
			cores[coreid].initialized = TRUE;
			or1k_dcache_inval();
		}

	or1k_spinlock_unlock(&cores[coreid].lock);

	cores[coreid].start();
}

/*============================================================================*
 * or1k_core_reset()                                                           *
 *============================================================================*/

/**
 * The or1k_core_reset() function resets execution instruction in
 * the underlying core by reseting the kernel stack to its initial
 * location and relaunching the or1k_slave_setup() function.
 *
 * @note This function does not return.
 *
 * @see or1k_slave_setup()
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_core_reset(void)
{
	int coreid = or1k_core_get_id();

	or1k_spinlock_lock(&cores[coreid].lock);
	or1k_dcache_inval();

		cores[coreid].state = OR1K_CORE_RESETTING;

		or1k_dcache_inval();

		kprintf("[hal] resetting core");

		_or1k_core_reset();

		/*
		 * The lock of this core will
		 * be released when resetting
		 * is completed, in or1k_core_idle().
		 */
}

/*============================================================================*
 * or1k_core_shutdown()                                                        *
 *============================================================================*/

/**
 * The or1k_core_shutdown() function powers off the underlying core.
 * Afeter powering off a core, instruction execution cannot be
 * resumed. The status code @p status is handled to the remote spawner
 * device.
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_core_shutdown(int status)
{
	int coreid = or1k_core_get_id();

	UNUSED(status);

	or1k_spinlock_lock(&cores[coreid].lock);

		cores[coreid].state = OR1K_CORE_OFFLINE;

	or1k_dcache_inval();
	or1k_spinlock_unlock(&cores[coreid].lock);

	/* Disable all interrupts. */
	or1k_pic_lvl_set(OR1K_INTLVL_0);

	/* If Power Management Present. */
	if (or1k_mfspr(OR1K_SPR_UPR) & OR1K_SPR_UPR_PMP)
		or1k_mtspr(OR1K_SPR_PMR, OR1K_SPR_PMR_DME);
	else
	{
		while (TRUE)
			/* noop(). */;
	}
}
