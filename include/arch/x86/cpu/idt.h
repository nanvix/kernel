/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_IDT_H_
#define ARCH_X86_CPU_IDT_H_

/**
 * @addtogroup x86-cpu-idt x86 IDT
 * @ingroup x86
 *
 * @brief x86 IDT
 */
/**@{*/

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Size of and IDT entry (in bytes).
 */
#define IDTE_SIZE 8

/**
 * @brief Size of IDTPTR (in bytes).
 */
#define IDTPTR_SIZE 6

/**
 * @brief Number of entries in the IDT.
 */
#define IDT_LENGTH 256

/**
 * @name Types of IDT Entries
 */
/**@{*/
#define IDT_TASK32 0x5 /** 32-bit task gate.      */
#define IDT_INT32 0xe  /** 32-bit interrupt gate. */
#define IDT_TRAP32 0xf /** 32-bit trap gate.      */
/**@}*/

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Interrupt descriptor table entry (IDTE).
 */
struct idte {
    unsigned handler_low : 16;  /** Handler low.  */
    unsigned selector : 16;     /** GDT selector. */
    unsigned : 8;               /** Always zero.  */
    unsigned type : 4;          /** Gate type.    */
    unsigned flags : 4;         /** Flags.        */
    unsigned handler_high : 16; /** handler high. */
} __attribute__((packed));

/**
 * @brief Interrupt descriptor table pointer (IDTPTR).
 */
struct idtptr {
    unsigned size : 16; /** IDT size.            */
    unsigned ptr : 32;  /** IDT virtual address. */
} __attribute__((packed));

#endif /* _ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_IDT_H_ */
