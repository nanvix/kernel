/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_CPU_H_
#define NANVIX_KERNEL_HAL_CPU_H_

#include <nanvix/kernel/hal/arch.h>
#include <nanvix/kernel/hal/cpu/context.h>

#ifndef _ASM_FILE_

/**
 * @brief Initializes the CPU.
 */
extern void cpu_init(void);

/**
 * @brief Disables interrupts.
 */
static inline void disable_interrupts(void)
{
    // TODO: https://github.com/nanvix/microkernel/issues/366
}

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_CPU_H_ */
