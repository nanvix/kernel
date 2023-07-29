/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_MEMORY_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_MEMORY_H_

/**
 * @name Physical Memory Layout
 */
/**@{*/
#define MEMORY_BASE 0x00000000     /** DRAM Base */
#define MEMORY_END_PHYS 0x04000000 /** DRAM End  */
/**@}*/

/**
 * @brief DRAM brief (in bytes).
 */
#define MEMORY_SIZE (MEMORY_END_PHYS - MEMORY_BASE)

/**
 * @name Physical Memory Layout
 */
/**@{*/
#define KERNEL_BASE_PHYS MEMORY_BASE /** Kernel Code and Data */
#define KERNEL_END_PHYS (KERNEL_BASE_PHYS + PGTAB_SIZE) /** Kernel End */
#define KPOOL_BASE_PHYS (KERNEL_END_PHYS + PGTAB_SIZE)  /** Kernel Page Pool */
#define KPOOL_END_PHYS (KPOOL_BASE_PHYS + PGTAB_SIZE)   /** Kernel Pool End */
#define USER_BASE_PHYS KPOOL_END_PHYS /** User Base            */
#define USER_END_PHYS MEMORY_END_PHYS /** User End             */
/**@}*/

/**
 * @name Virtual Memory Layout
 */
/**@{*/
#define KERNEL_BASE_VIRT KERNEL_BASE_PHYS /** Kernel Code and Data */
#define KERNEL_END_VIRT KERNEL_END_PHYS   /** Kernel End           */
#define KPOOL_BASE_VIRT KPOOL_BASE_PHYS   /** Kernel Page Pool     */
#define KPOOL_END_VIRT KPOOL_END_PHYS     /** Kernel Pool End      */
#define USER_BASE_VIRT 0xa0000000         /** User Base            */
#define USER_END_VIRT 0xc0000000          /** User End             */
#define USTACK_BASE_VIRT 0xc0000000       /** User Stack Base      */
#define USTACK_END_VIRT 0xb0000000        /** User Stack End       */
/**@}*/

/**
 * @name Memory Regions Constants
 */
/**@{*/
#define MEM_REGIONS 2              /** Memory Regions number.            */
#define ROOT_PGTAB_NUM MEM_REGIONS /** Root page table size.             */
#define MREGION_PT_ALIGN_START 0   /** MRegion start page table aligned. */
#define MREGION_PT_ALIGN_END 2     /** MRegion end page table aligned.   */
#define MREGION_PG_ALIGN_START 0   /** MRegion start page aligned.       */
#define MREGION_PG_ALIGN_END 0     /** MRegion end page aligned.         */
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
#endif /* NANVIX_KERNEL_HAL_ARCH_X86_MEMORY_H_ */
