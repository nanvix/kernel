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

#include <arch/k1b/core.h>
#include <arch/k1b/cpu.h>
#include <arch/k1b/elf.h>
#include <arch/k1b/excp.h>
#include <arch/k1b/int.h>
#include <arch/k1b/ivt.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/* Import definitions. */
EXTERN NORETURN void kmain(int, const char *[]);
EXTERN void _do_syscall(int, int, int, int, int, int, int, int);

/*============================================================================*
 * k1b_stack_setup()                                                          *
 *============================================================================*/

/**
 * @brief Setups the stack.
 *
 * The k1b_stack_setup() function setups the stack of the underlying
 * core by reseting the stack pointer register to the location defined
 * in the link scripts.
 *
 * It would be safier to do this in asembly code, early in boot.
 * However, we are relying on VBSP and we cannot do so. Thus, we make
 * this function inline and call it as early as possible. Hopefully,
 * it will work.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE inline void k1b_stack_setup(void)
{
	__k1_uint8_t *stack_base;

	stack_base = __k1_tls_pe_base_address(k1b_core_get_id());
	__k1_setup_tls_pe(stack_base);
}

/*============================================================================*
 * k1b_core_setup()                                                           *
 *============================================================================*/

/**
 * The k1b_core_setup() function initializes all architectural
 * structures of the underlying core. It setups the Interrupt Vector
 * Table (IVT) and the Memory Management Unit (MMU) tables.
 *
 * @todo We should move this to the core module.
 * @bug We cannot move this out of this file due to somelinking problem.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void k1b_core_setup(void)
{
	int coreid;

	coreid = k1b_core_get_id();
	kprintf("[hal] booting up core %d", coreid);

	k1b_ivt_setup(
		(k1b_hwint_handler_fn) k1b_do_hwint,
		(k1b_swint_handler_fn) _do_syscall,
		(k1b_excp_handler_fn) _do_excp
	);

	k1b_mmu_setup();
}

/*============================================================================*
 * k1b_slave_setup()                                                          *
 *============================================================================*/

/**
 * @brief Initializes a slave core.
 *
 * The k1b_slave_setup() function initializes the underlying slave
 * core.  It setups the stack and then call the kernel main function.
 * Architectural structures are initialized by the master core and
 * registered later on, when the slave core is started effectively.
 *
 * @note This function does not return.
 *
 * @see k1b_core_setup() and k1b_master_setup().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC NORETURN void k1b_slave_setup(void)
{
	k1b_stack_setup();

	kprintf("[hal] starting core...");

	while (TRUE)
	{
		k1b_core_sleep();
		k1b_core_run();
	}
}

/*============================================================================*
 * k1b_master_setup()                                                         *
 *============================================================================*/

/**
 * @brief Initializes the master core.
 *
 * The k1b_master_setup() function initializes the underlying
 * master core.  It setups the stack, retrieves the boot arguments,
 * initializes architectural structures and then call the kernel main
 * function.
 *
 * @note This function does not return.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE NORETURN void k1b_master_setup(void)
{
	k1_boot_args_t args;

	k1b_stack_setup();

	get_k1_boot_args(&args);

	k1b_core_setup();

	kmain(args.argc, (const char **)args.argv);
}

/*============================================================================*
 * _do_slave_pe()                                                             *
 *============================================================================*/

/**
 * @brief Starts a slave core.
 *
 * @param oldsp Old stack pointer (unused).
 *
 * The _do_slave_pe() function is the entry point for a slave core. It
 * is called by the VBSP routines, once a bare environment is setup.
 *
 * @note This function does not return.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void _SECTION_TEXT NORETURN _do_slave_pe(uint32_t oldsp)
{
	UNUSED(oldsp);

	k1b_slave_setup();
}

/*============================================================================*
 * _do_master_pe()                                                            *
 *============================================================================*/

/**
 * @brief Starts a master core.
 *
 * @param oldsp Old stack pointer (unused).
 *
 * The _do_master_pe() function is the entry point for a master core. It
 * is called by the VBSP routines, once a bare environment is setup.
 *
 * @note This function does not return.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void _SECTION_TEXT NORETURN _do_master_pe(uint32_t oldsp)
{
	UNUSED(oldsp);

	k1b_master_setup();
}
