/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_EXCP_H_
#define ARCH_X86_CPU_EXCP_H_

/**
 * @addtogroup x86-cpu-exception x86 Software Exceptions
 * @ingroup x86-cpu
 *
 * @brief Software Exceptions Module
 */
/**@{*/

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of exceptions in the i486 core.
 */
#define EXCEPTIONS_NUM 21

/**
 * @name Exception Codes
 */
/**@{*/
#define EXCEPTION_DIVIDE 0                 /** Division-by-Zero Error      */
#define EXCEPTION_DEBUG 1                  /** Debug Exception             */
#define EXCEPTION_NMI 2                    /** Non-Maskable Interrupt      */
#define EXCEPTION_BREAKPOINT 3             /** Breakpoint Exception        */
#define EXCEPTION_OVERFLOW 4               /** Overflow Exception          */
#define EXCEPTION_BOUNDS 5                 /** Bounds Check Exception      */
#define EXCEPTION_INVALID_OPCODE 6         /** Invalid Opcode Exception    */
#define EXCEPTION_COPROC_NOT_AVAILABLE 7   /** Coprocessor Not Available   */
#define EXCEPTION_DOUBLE_FAULT 8           /** Double Fault                */
#define EXCEPTION_COPROC_SEGMENT_OVERRUN 9 /** Coprocessor Segment Overrun */
#define EXCEPTION_INVALID_TSS 10           /** Invalid TSS                 */
#define EXCEPTION_SEGMENT_NOT_PRESENT 11   /** Segment Not Present         */
#define EXCEPTION_STACK_SEGMENT_FAULT 12   /** Stack Segment Fault         */
#define EXCEPTION_GENERAL_PROTECTION 13    /** General Protection Fault    */
#define EXCEPTION_PAGE_FAULT 14            /** Page Fault                  */
#define EXCEPTION_FPU_ERROR 16             /** Floating Point Exception    */
#define EXCEPTION_ALIGNMENT_CHECK 17       /** Alignment Check Exception   */
#define EXCEPTION_MACHINE_CHECK 18         /** Machine Check Exception     */
#define EXCEPTION_SIMD_ERROR 19            /** SMID Unit Exception         */
#define EXCEPTION_VIRTUAL_EXCEPTION 20     /** Virtual Exception           */
#define EXCEPTION_SECURITY_EXCEPTION 30    /** Security Exception.         */
/**@}*/

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_EXCP_H_ */
