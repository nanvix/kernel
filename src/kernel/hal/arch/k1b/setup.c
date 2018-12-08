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
#include <HAL/hal/core/legacy.h>
#include <mOS_common_types_c.h>

#include <arch/k1b/cache.h>
#include <arch/k1b/core.h>
#include <arch/k1b/cpu.h>
#include <arch/k1b/elf.h>
#include <arch/k1b/int.h>
#include <arch/k1b/ivt.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include "mppa256.h"

/* Import definitions. */
EXTERN NORETURN void kmain(int, const char *[]);

/**
 * @brief Core states.
 */
enum core_states
{
	CORE_IDLE, /**< Idle.   */
	CORE_BUSY  /**< In use. */
};

/**
 * @brief Slave Cores.
 */
PRIVATE struct
{
	int initialized;        /**< Initialized core? */
	enum core_states state; /**< State.            */
	void (*start)(void);    /**< Starting routine. */
} cores[K1B_NUM_CORES] = {
	[0 ... (K1B_NUM_CORES - 1)] = {.initialized = 0, .state = CORE_IDLE}
};

/**
 * @brief Initializes the Thread Local Storage (TSS) segment.
 */
PRIVATE void tls_init(void)
{
	int coreid = k1b_core_get_id();

	__k1_uint8_t *tls_base1 = __k1_tls_pe_base_address(coreid);
	__k1_setup_tls_pe(tls_base1);
	k1b_dcache_inval();
}

/**
 * @brief Initializes the underlying core.
 */
PRIVATE void core_setup(void)
{
	int coreid;

	tls_init();

	coreid = k1b_core_get_id();
	kprintf("booting up core %d", coreid);

	k1b_ivt_setup(
		(k1b_hwint_handler_fn) k1b_do_hwint,
		(k1b_swint_handler_fn) _syscall
	);
}

/**
 * @brief Initializes slave core.
 */
PRIVATE NORETURN void setup_slave_core(void)
{
	kmain(0, NULL);
}

/**
 * @brief Initializes master core.
 */
PRIVATE NORETURN void setup_master_core(void)
{
	k1_boot_args_t args;

	core_setup();

	get_k1_boot_args(&args);

	k1b_dcache_inval();

	kmain(args.argc, (const char **)args.argv);
}

/**
 * @brief Hals the underling core.
 */
PUBLIC void core_halt(void)
{
	int coreid = k1b_core_get_id();

	while (cores[coreid].state == CORE_IDLE)
	{
		mOS_it_disable_num(MOS_VC_IT_USER_0);
		k1b_await();
		k1b_dcache_inval();
		mOS_it_clear_num(MOS_VC_IT_USER_0);
		mOS_it_enable_num(MOS_VC_IT_USER_0);
	}
}

/**
 * @brief Starts a core.
 */
PUBLIC void core_start(void)
{
	int coreid = k1b_core_get_id();

	if (!cores[coreid].initialized)
	{
		core_setup();
		cores[coreid].initialized = 1;
		k1b_dcache_inval();
	}
	
	cores[coreid].start();

	cores[coreid].state = CORE_IDLE;
	k1b_dcache_inval();
}

/**
 * @brief Wakes up a core.
 *
 * @param coreid ID of the target core.
 * @param start  Starting routine to execute.
 */
PUBLIC void core_wakeup(int coreid, void (*start)(void))
{
	cores[coreid].state = CORE_BUSY;
	cores[coreid].start = start;
	k1b_dcache_inval();
	
	bsp_inter_pe_event_notify(1 << coreid, BSP_IT_LINE);
}

/**
 * @brief Shutdowns the underlying core cluster.
 *
 * @param status Shutdown status.
 */
PUBLIC void shutdown(int status)
{
	mOS_exit(__k1_spawn_type() != __MPPA_MPPA_SPAWN, status);
}

/**
 * @brief Starting point for slave core.
 */
PUBLIC void _SECTION_TEXT _do_slave_pe(uint32_t old_sp)
{
	UNUSED(old_sp);

	setup_slave_core();
}

/**
 * @brief Starting point for master core.
 */
PUBLIC void _SECTION_TEXT _do_master_pe(uint32_t old_sp)
{
	UNUSED(old_sp);

	setup_master_core();
}
