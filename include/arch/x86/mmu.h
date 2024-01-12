/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_MMU_H_
#define ARCH_X86_MMU_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#ifndef _ASM_FILE_
#include <stdint.h>
#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @name Bit-Length of Memory Types
 */
/**@{*/
#define PADDR_BIT 32 /** Physical Address */
#define VADDR_BIT 32 /** Virtual Address  */
/**@}*/

/**
 * @name Byte-Length of Memory Types
 */
/**@{*/
#define PADDR_BYTE 4 /** Physical Address */
#define VADDR_BYTE 4 /** Virtual Address  */
/**@}*/

/**
 * @name Page Shifts and Masks
 */
/**@{*/
#define PAGE_SHIFT 12                  /** Page Shift       */
#define PGTAB_SHIFT 22                 /** Page Table Shift */
#define PAGE_MASK (~(PAGE_SIZE - 1))   /** Page Mask        */
#define PGTAB_MASK (~(PGTAB_SIZE - 1)) /** Page Table Mask  */
/**@}*/

/**
 * @name Size of Pages and Page Tables
 */
/**@{*/
#define PAGE_SIZE (1 << PAGE_SHIFT)   /** Page Size                 */
#define PGTAB_SIZE (1 << PGTAB_SHIFT) /** Page Table Size           */
#define PTE_SIZE 4                    /** Page Table Entry Size     */
#define PDE_SIZE 4                    /** Page Directory Entry Size */
/**@}*/

/**
 * @brief Length of virtual addresses.
 *
 * Number of bits in a virtual address.
 */
#define VADDR_LENGTH 32

/**
 * @brief Page Directory length.
 *
 * Number of Page Directory Entries (PDEs) per Page Directory.
 */
#define PGDIR_LENGTH (1 << (VADDR_LENGTH - PGTAB_SHIFT))

/**
 * @brief Page Table length.
 *
 * Number of Page Table Entries (PTEs) per Page Table.
 */
#define PGTAB_LENGTH (1 << (PGTAB_SHIFT - PAGE_SHIFT))

/*============================================================================*
 * Types                                                                      *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Frame number.
 */
typedef uint32_t frame_t;

/**
 * @brief Virtual address.
 */
typedef uint32_t vaddr_t;

/**
 * @brief Physical address.
 */
typedef uint32_t paddr_t;

#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Page table entry.
 */
struct pte {
    unsigned present : 1;  /** Present in memory? */
    unsigned writable : 1; /** Writable page?     */
    unsigned user : 1;     /** User page?         */
    unsigned : 2;          /** Reserved.          */
    unsigned accessed : 1; /** Accessed?          */
    unsigned dirty : 1;    /** Dirty?             */
    unsigned : 2;          /** Reserved.          */
    unsigned : 3;          /** Unused.            */
    unsigned frame : 20;   /** Frame number.      */
} __attribute__((packed));

/**
 * @brief Page directory entry.
 */
struct pde {
    unsigned present : 1;  /** Present in memory? */
    unsigned writable : 1; /** Writable page?     */
    unsigned user : 1;     /** User page?         */
    unsigned : 2;          /** Reserved.          */
    unsigned accessed : 1; /** Accessed?          */
    unsigned dirty : 1;    /** Dirty?             */
    unsigned : 2;          /** Reserved.          */
    unsigned : 3;          /** Unused.            */
    unsigned frame : 20;   /** Frame number.      */
} __attribute__((packed));

#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Loads changes in the TLB.
 *
 * @returns This function always returns zero.
 */
static inline int tlb_load(paddr_t pgdir)
{
    // TODO: We can improve this by using the invlpg instruction.
    __asm__ volatile("movl %0, %%eax\n"
                     "movl %%eax, %%cr3\n"
                     "movl %%cr0, %%eax\n"
                     "orl $0x80000000, %%eax\n"
                     "movl %%eax, %%cr0\n"
                     :
                     : "r"(pgdir)
                     : "eax");

    return (0);
}

/**
 * @brief Flushes changes in the TLB.
 */
static inline int tlb_flush(void)
{
    __asm__ volatile("movl %%cr3, %%eax\n"
                     "movl %%eax, %%cr3\n"
                     "movl %%cr0, %%eax\n"
                     "orl $0x80000000, %%eax\n"
                     "movl %%eax, %%cr0\n"
                     :
                     :
                     : "eax");

    return (0);
}

#endif /* !_ASM_FILE_ */

#endif /* ARCH_X86_MMU_H_ */
