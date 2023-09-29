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
/**
 * @name Physical Memory Layout
 */
/**@{*/
#define KERNEL_BASE_PHYS MEMORY_BASE                    /** Kernel          */
#define KERNEL_END_PHYS (KERNEL_BASE_PHYS + PGTAB_SIZE) /** Kernel End      */
#define KPOOL_BASE_PHYS (KERNEL_END_PHYS)               /** Kernel  Pool    */
#define KPOOL_END_PHYS (KPOOL_BASE_PHYS + PGTAB_SIZE)   /** Kernel Pool End */
#define USER_BASE_PHYS KPOOL_END_PHYS                   /** User Base       */
#define USER_END_PHYS MEMORY_END_PHYS                   /** User End        */
/**@}*/

/**
 * @name Virtual Memory Layout
 */
/**@{*/
#define KERNEL_BASE_VIRT KERNEL_BASE_PHYS /** Kernel Code and Data */
#define KERNEL_END_VIRT KERNEL_END_PHYS   /** Kernel End           */
#define KPOOL_BASE_VIRT KPOOL_BASE_PHYS   /** Kernel Page Pool     */
#define KPOOL_END_VIRT KPOOL_END_PHYS     /** Kernel Pool End      */
#define USER_BASE_VIRT USER_BASE_PHYS     /** User Base            */
#define USER_END_VIRT USER_END_PHYS       /** User End             */
/**@}*/

/**
 * @brief Kernel memory size (in bytes).
 */
#define KMEM_SIZE (KERNEL_END_PHYS - KERNEL_BASE_PHYS)

/**
 * @brief Kernel page pool size (in bytes).
 */
#define KPOOL_SIZE (KPOOL_END_PHYS - KPOOL_BASE_PHYS)

/**
 * @brief User memory size (in bytes).
 */
#define UMEM_SIZE (USER_END_PHYS - USER_BASE_PHYS)

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Initializes the memory interface.
 *
 * @returns The root page directory.
 */
extern const void *memory_init(void);

/*============================================================================*/

#endif /* NANVIX_KERNEL_MM_MEMORY_H_ */
