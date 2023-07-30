/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_H_
#define NANVIX_KERNEL_HAL_H_

/**
 * @addtogroup kernel-hal Kernel HAL
 * @ingroup kernel
 *
 * @brief Hardware Abstraction Layer
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal/arch.h>
#include <nanvix/kernel/hal/cpu.h>
#include <nanvix/kernel/hal/exception.h>
#include <nanvix/kernel/hal/interrupt.h>
#include <nanvix/kernel/hal/memory.h>
#include <nanvix/kernel/hal/mmu.h>
#include <nanvix/kernel/hal/stdout.h>
#include <nanvix/kernel/hal/timer.h>
#if defined(__TARGET_HAS_TLB_HW) && (__TARGET_HAS_TLB_HW == 0)
#include <nanvix/kernel/hal/tlb.h>
#endif /* __TARGET_HAS_TLB_HW == 0 */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes the Hardware Abstraction Layer (HAL).
 */
extern void hal_init(void);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_H_ */
