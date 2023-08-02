/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_MM_MEMORY_H_
#define NANVIX_KERNEL_MM_MEMORY_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @name Memory area identification
 */
/**@{*/
#define KMEM_AREA 0 /** Kernel memory area. */
#define UMEM_AREA 1 /** User memory area.   */
/**@}*/

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Initializes the memory interface.
 */
extern void memory_init(void);

/*============================================================================*/

#endif /* NANVIX_KERNEL_MM_MEMORY_H_ */
