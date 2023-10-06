/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_CTX_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_CTX_H_

#include <arch/x86.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Hardware-saved execution context size (in bytes).
 */
#define CONTEXT_HW_SIZE 24

/**
 * @brief Software-saved execution context size (in bytes).
 */
#define CONTEXT_SW_SIZE 48

/**
 * @brief Execution context size (in bytes).
 */
#define CONTEXT_SIZE (CONTEXT_HW_SIZE + CONTEXT_SW_SIZE)

/**
 * @name Offsets to the Context Structure
 */
/**@{*/
#define CONTEXT_CR3 0     /** Page Directory Register               */
#define CONTEXT_GS 4      /** Extra Data Segment #3 Register        */
#define CONTEXT_FS 8      /** Extra Data Segment #2 Register        */
#define CONTEXT_ES 12     /** Extra Data Segment #1 Register        */
#define CONTEXT_DS 16     /** Data Segment Register                 */
#define CONTEXT_EDI 20    /** Extended Destination Index Register   */
#define CONTEXT_ESI 24    /** Extended Source Index Register        */
#define CONTEXT_EBP 28    /** Extended Stack base Pointer Register  */
#define CONTEXT_EDX 32    /** Extended Accumulator #2 Register      */
#define CONTEXT_ECX 36    /** Extended Counter Register             */
#define CONTEXT_EBX 40    /** Extended Base Index Register          */
#define CONTEXT_EAX 44    /** Extended Accumulator #1 Register      */
#define CONTEXT_ERR 48    /* Error code                             */
#define CONTEXT_EIP 52    /** Extended Instruction Pointer Register */
#define CONTEXT_CS 56     /** Code Segment Register                 */
#define CONTEXT_EFLAGS 60 /** Extended Flags Register               */
#define CONTEXT_ESP 64    /** Extended Stack Pointer Register       */
#define CONTEXT_SS 68     /** Stack Segment Register                */
/**@}*/

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Saved execution context upon interrupts and exceptions.
 */
struct context {
    word_t cr3;                               /** Page Directory     */
    word_t gs, fs, es, ds;                    /** Segment Registers  */
    word_t edi, esi, ebp, edx, ecx, ebx, eax; /** GPRs               */
    word_t err, eip, cs, eflags, esp, ss;     /** Special Registers  */
} __attribute__((packed)) __attribute__((aligned(WORD_SIZE)));

#endif /* _ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_CTX_H_ */
