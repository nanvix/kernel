/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_MEMORY_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_MEMORY_H_

/**
 * @name Physical Memory Layout
 */
/**@{*/
#define MEMORY_BASE 0x00000000     /** DRAM Base */
#define MEMORY_END_PHYS 0x08000000 /** DRAM End  */
/**@}*/

/**
 * @brief DRAM brief (in bytes).
 */
#define MEMORY_SIZE (MEMORY_END_PHYS - MEMORY_BASE)

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_MEMORY_H_ */
