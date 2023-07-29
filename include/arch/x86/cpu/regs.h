/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef _ARCH_X86_REGS_H_
#define _ARCH_X86_REGS_H_

/**
 * @name Extended Flags Register
 */
/**@{*/
#define EFLAGS_CF (1 << 0)     /** Carry Flag            */
#define EFLAGS_PF (1 << 2)     /** Parity Flag           */
#define EFLAGS_AF (1 << 4)     /** Auxiliary Flag        */
#define EFLAGS_ZF (1 << 6)     /** Zero Flag             */
#define EFLAGS_SF (1 << 7)     /** Sign Flag             */
#define EFLAGS_TF (1 << 8)     /** Trap Flag             */
#define EFLAGS_IF (1 << 9)     /** Interrupt Enable Flag */
#define EFLAGS_DF (1 << 10)    /** Direction Flag        */
#define EFLAGS_OF (1 << 11)    /** Overflow Flag         */
#define EFLAGS_IOPL0 (0 << 12) /** I/O Privilege Level 0 */
#define EFLAGS_IOPL1 (1 << 12) /** I/O Privilege Level 1 */
#define EFLAGS_IOPL2 (2 << 12) /** I/O Privilege Level 2 */
#define EFLAGS_IOPL3 (3 << 12) /** I/O Privilege Level 3 */
#define EFLAGS_NT (1 << 14)    /** Nested Task           */
#define EFLAGS_RF (1 << 16)    /** Resume Flag           */
/**@}*/

#endif /* _ARCH_X86_REGS_H_ */
