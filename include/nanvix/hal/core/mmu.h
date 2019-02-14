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

#ifndef HAL_CORE_MMU_H_
#define HAL_CORE_MMU_H_

	/* Core Interface Implementation */
	#include <nanvix/hal/core/_core.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Constants */
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

	/* Types & Structures */
	#ifndef __frame_t
	#error "__frame_t not defined?"
	#endif
	#ifndef __paddr_t
	#error "paddr_t not defined?"
	#endif
	#ifndef __vaddr_t
	#error "vaddr_t not defined?"
	#endif

	/* Functions */
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
 * Memory Management Unit Interface                                           *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-core-mmu MMU
 * @ingroup kernel-hal-core
 *
 * @brief MMU HAL Interface
 */
/**@{*/

/**@}*/

#endif /* HAL_CORE_MMU_H_ */
