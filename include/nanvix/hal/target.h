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

#ifndef NANVIX_HAL_TARGET_H_
#define NANVIX_HAL_TARGET_H_

	#if defined(__mppa256__)
	#include <target/kalray/mppa256.h>
	#endif

	#if (defined(__i386__) && (__pc__))
	#include <target/ibm/pc.h>
	#endif

/*============================================================================*
 * Processor Interface                                                         *
 *============================================================================*/

	#ifndef _HAL_NUM_CORES
	#error "_HAL_NUM_CORES not defined"
	#endif

	#ifndef __hal_cpu_get_num_cores
	#error "hal_cpu_get_num_cores() not defined?"

/*============================================================================*
 * Core Interface                                                             *
 *============================================================================*/
	#endif

	#ifndef __hal_core_get_id
	#error "hal_core_get_id() not defined?"
	#endif

	#ifndef __hal_core_halt
	#error "hal_core_halt() not defined?"
	#endif

	#ifndef __hal_core_setup
	#error "hal_core_setup() not defined?"
	#endif

/*============================================================================*
 * Clock Device Interface                                                     *
 *============================================================================*/

	#ifndef HAL_INT_CLOCK
	#error "HAL_INT_CLOCK not defined"
	#endif

	#ifndef __hal_clock_init
	#error "hal_clock_init() not defined?"
	#endif

/*============================================================================*
 * Debug Interface                                                            *
 *============================================================================*/

	#ifndef __hal_stdout_init
	#error "hal_stdout_init() not defined?"
	#endif

	#ifndef __hal_stdout_write
	#error "hal_stdout_write() not defined?"
	#endif

/*============================================================================*
 * Interrupt Interface                                                        *
 *============================================================================*/

	#ifndef _HAL_INT_NR
	#error "_HAL_INT_NR not defined"
	#endif

	#ifndef __hal_disable_interrupts
	#error "hal_disable_interrupts() not defined?"
	#endif

	#ifndef __hal_enable_interrupts
	#error "hal_enable_interrupts() not defined?"
	#endif

	#ifndef __hal_interrupt_mask
	#error "hal_interrupt_mask() not defined?"
	#endif

	#ifndef __hal_interrupt_unmask
	#error "hal_interrupt_unmask() not defined?"
	#endif

	#ifndef __hal_interrupt_ack
	#error "hal_interrupt_ack() not defined?"
	#endif

	#ifndef __hal_interrupt_set_handler
	#error "hal_interrupt_set_handler() not defined?"
	#endif

	#ifndef __hal_intlvl_set
	#error "hal_intlvl_set() not defined?"
	#endif

/*============================================================================*
 * Exception Interface                                                        *
 *============================================================================*/

	#ifndef _HAL_NUM_EXCEPTION
	#error "_HAL_NUM_EXCEPTION not defined"
	#endif

	#ifndef __hal_exception_set_handler
	#error "hal_exception_set_handler() not defined?"
	#endif

/*============================================================================*
 * Input/Output Interface                                                     *
 *============================================================================*/

	#ifndef __hal_outputb
	#error "hal_outputb() not defined?"
	#endif

	#ifndef __hal_iowait
	#error "hal_iowait() not defined?"
	#endif

/*============================================================================*
 * Memory Cache Interface                                                     *
 *============================================================================*/

	#ifndef __hal_dcache_invalidate
	#error "hal_dcache_invalidate() not defined?"
	#endif

/*============================================================================*
 * Memory Interface                                                           *
 *============================================================================*/

	#ifndef _UBASE_VIRT
	#error "_UBASE_VIRT not defined"
	#endif

	#ifndef _USTACK_ADDR
	#error "_USTACK_ADDR not defined"
	#endif

	#ifndef _KBASE_VIRT
	#error "_KBASE_VIRT not defined"
	#endif

	#ifndef _KPOOL_VIRT
	#error "_KPOOL_VIRT not defined"
	#endif

	#ifndef _INITRD_VIRT
	#error "_INITRD_VIRT not defined"
	#endif

	#ifndef _KBASE_PHYS
	#error "_KBASE_PHYS not defined"
	#endif

	#ifndef _KPOOL_PHYS
	#error "_KPOOL_PHYS not defined"
	#endif

	#ifndef _UBASE_PHYS
	#error "_UBASE_PHYS not defined"
	#endif

	#ifndef _MEMORY_SIZE
	#error "_MEMORY_SIZE not defined"
	#endif

	#ifndef _KMEM_SIZE
	#error "_KMEM_SIZE not defined"
	#endif

	#ifndef _KPOOL_SIZE
	#error "_KPOOL_SIZE not defined"
	#endif

	#ifndef _UMEM_SIZE
	#error "_UMEM_SIZE not defined"
	#endif

/*============================================================================*
 * MMU Interface                                                              *
 *============================================================================*/

	#ifndef __frame_t
	#error "__frame_t not defined?"
	#endif

	#ifndef __paddr_t
	#error "paddr_t not defined?"
	#endif

	#ifndef __vaddr_t
	#error "vaddr_t not defined?"
	#endif

	#ifndef PAGE_SHIFT
	#error "PAGE_SHIFT not defined"
	#endif

	#ifndef PGTAB_SHIFT
	#error "PGTAB_SHIFT not defined"
	#endif

	#ifndef PAGE_SIZE
	#error "PAGE_SIZE not defined"
	#endif

	#ifndef PGTAB_SIZE
	#error "PGTAB_SIZE not defined"
	#endif

	#ifndef PTE_SIZE
	#error "PTE_SIZE not defined"
	#endif

	#ifndef PDE_SIZE
	#error "PDE_SIZE not defined"
	#endif

	#ifndef __pde_clear_fn
	#error "ipde_clear() not defined?"
	#endif

	#ifndef __pde_frame_set_fn
	#error "ipde_frame_set() not defined?"
	#endif

	#ifndef __pde_frame_get_fn
	#error "ipde_frame_get() not defined?"
	#endif

	#ifndef __pde_get_fn
	#error "pde_get() not defined?"
	#endif

	#ifndef __pde_is_present_fn
	#error "pde_is_present() not defined?"
	#endif

	#ifndef __pde_is_user_fn
	#error "pde_is_user() not defined?"
	#endif

	#ifndef __pde_is_write_fn
	#error "pde_is_write() not defined?"
	#endif

	#ifndef __pde_present_set_fn
	#error "pde_present_set() not defined?"
	#endif

	#ifndef __pde_user_set_fn
	#error "pde_user_set() not defined?"
	#endif

	#ifndef __pde_write_set_fn
	#error "pde_write_set() not defined?"
	#endif

	#ifndef __pte_clear_fn
	#error "pte_clear() not defined?"
	#endif

	#ifndef __pte_frame_set_fn
	#error "pte_frame_set() not defined?"
	#endif

	#ifndef __pte_get_fn
	#error "pte_get() not defined?"
	#endif

	#ifndef __pte_is_present_fn
	#error "pte_is_present() not defined?"
	#endif

	#ifndef __pte_is_user_fn
	#error "pte_is_user() not defined?"
	#endif

	#ifndef __pte_is_write_fn
	#error "pte_is_write() not defined?"
	#endif

	#ifndef __pte_present_set_fn
	#error "pte_present_set() not defined?"
	#endif

	#ifndef __pte_user_set_fn
	#error "pte_user_set() not defined?"
	#endif

	#ifndef __pte_write_set_fn
	#error "pte_write_set() not defined?"
	#endif

/*============================================================================*
 * TLB Interface                                                              *
 *============================================================================*/

	#ifndef __tlb_flush_fn
	#error "tlb_flush() not defined?"
	#endif

#endif /* NANVIX_HAL_TARGET_H_ */
