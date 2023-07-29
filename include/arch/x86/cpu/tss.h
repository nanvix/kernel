/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_TSS_H_
#define ARCH_X86_CPU_TSS_H_

/**
 * @addtogroup x86-cpu-tss x86 TSS
 * @ingroup x86
 *
 * @brief x86 TSS
 */
/**@{*/

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Size of a TSS (in bytes).
 */
#define TSS_SIZE 104

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Task state segment (TSS).
 */
struct tss {
    unsigned link;   /** Previous TSS in the list. */
    unsigned esp0;   /** Ring 0 stack pointer.     */
    unsigned ss0;    /** Ring 0 stack segment.     */
    unsigned esp1;   /** Ring 1 stack pointer.     */
    unsigned ss1;    /** Ring 1 stack segment.     */
    unsigned esp2;   /** Ring 2 stack pointer.     */
    unsigned ss2;    /** Ring 2 stack segment.     */
    unsigned cr3;    /** cr3.                      */
    unsigned eip;    /** eip.                      */
    unsigned eflags; /** eflags.                   */
    unsigned eax;    /** eax.                      */
    unsigned ecx;    /** ecx.                      */
    unsigned edx;    /** edx.                      */
    unsigned ebx;    /** ebx.                      */
    unsigned esp;    /** esp.                      */
    unsigned ebp;    /** ebp.                      */
    unsigned esi;    /** esi.                      */
    unsigned edi;    /** edi.                      */
    unsigned es;     /** es.                       */
    unsigned cs;     /** cs.                       */
    unsigned ss;     /** ss.                       */
    unsigned ds;     /** ds.                       */
    unsigned fs;     /** fs.                       */
    unsigned gs;     /** gs.                       */
    unsigned ldtr;   /** LDT selector.             */
    unsigned iomap;  /** IO map.                   */
} __attribute__((packed));

#endif /* _ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_TSS_H_ */
