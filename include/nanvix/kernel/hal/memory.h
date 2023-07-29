/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_MEMORY_H_
#define NANVIX_KERNEL_HAL_MEMORY_H_

#ifndef _ASM_FILE_
#include <stdbool.h>

/**
 * @name Binary Sections
 */
/**@{*/
extern unsigned char __BOOTSTRAP_START; /** Bootstrap Start       */
extern unsigned char __BOOTSTRAP_END;   /** Bootstrap End         */
extern unsigned char __TEXT_START;      /** Text Start            */
extern unsigned char __TEXT_END;        /** Text End              */
extern unsigned char __DATA_START;      /** Data Start            */
extern unsigned char __DATA_END;        /** Data End              */
extern unsigned char __BSS_START;       /** BSS Start             */
extern unsigned char __BSS_END;         /** BSS End               */
extern unsigned char __RODATA_START;    /** Ready-Only Data Start */
extern unsigned char __RODATA_END;      /** Ready-Only Data End   */
/**@}*/

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Memory region.
 */
struct memory_region {
    paddr_t pbase;      /** Base physical address.  */
    vaddr_t vbase;      /** Base virtual address.   */
    paddr_t pend;       /** End physical address.   */
    vaddr_t vend;       /** End virtual address.    */
    size_t size;        /** Size.                   */
    bool writable;      /** Writable?               */
    bool executable;    /** Executable?             */
    int root_pgtab_num; /** Root page table number. */
    const char *desc;   /** Description.            */
};

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_MEMORY_H_ */
