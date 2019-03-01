/*
 * Copyright(C) 2011-2018 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis <davidsondfgl@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * OpenRISC head.S
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * Modifications for the OpenRISC architecture:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

/* Must come first. */
#define _ASM_FILE_

#include <arch/or1k/context.h>
#include <arch/or1k/core.h>
#include <arch/or1k/mmu.h>
#include <arch/or1k/regs.h>
#include <arch/or1k/excp.h>
#include <target/or1k/pc.h>

/**
 * @brief Size of red zone (in bytes).
 */
#define REDZONE_SIZE 128

/**
 * @name Aliases for Registers
 */
/**@{*/
#define sp r1 /**< Stack Pointer Pointer        */
#define bp r2 /**< Stack Base Pointer Register  */
/**@}*/

/*============================================================================*
 * or1k_redzone_alloc()                                                       *
 *============================================================================*/

/*
 * Allocates the red zone in the current stack frame.
 */
.macro redzone_alloc
	l.addi sp, sp, -REDZONE_SIZE
.endm

/*============================================================================*
 * or1k_redzone_free()                                                        *
 *============================================================================*/

/*
 * Frees the red zone in the current stack frame.
 */
.macro redzone_free
	l.addi sp, sp, REDZONE_SIZE
.endm

/*============================================================================*
 * or1k_context_save()                                                        *
 *============================================================================*/

/*
 * Saves the current execution context.
 */
.macro or1k_context_save dest
	
	/* Save GPRs, except SP, BP and scratch registers r3 ... r6. */
	l.sw OR1K_CONTEXT_R0(\dest),   r0
	l.sw OR1K_CONTEXT_R7(\dest),   r7
	l.sw OR1K_CONTEXT_R8(\dest),   r8
	l.sw OR1K_CONTEXT_R9(\dest),   r9
	l.sw OR1K_CONTEXT_R10(\dest), r10
	l.sw OR1K_CONTEXT_R11(\dest), r11
	l.sw OR1K_CONTEXT_R12(\dest), r12
	l.sw OR1K_CONTEXT_R13(\dest), r13
	l.sw OR1K_CONTEXT_R14(\dest), r14
	l.sw OR1K_CONTEXT_R15(\dest), r15
	l.sw OR1K_CONTEXT_R16(\dest), r16
	l.sw OR1K_CONTEXT_R17(\dest), r17
	l.sw OR1K_CONTEXT_R18(\dest), r18
	l.sw OR1K_CONTEXT_R19(\dest), r19
	l.sw OR1K_CONTEXT_R20(\dest), r20
	l.sw OR1K_CONTEXT_R21(\dest), r21
	l.sw OR1K_CONTEXT_R22(\dest), r22
	l.sw OR1K_CONTEXT_R23(\dest), r23
	l.sw OR1K_CONTEXT_R24(\dest), r24
	l.sw OR1K_CONTEXT_R25(\dest), r25
	l.sw OR1K_CONTEXT_R26(\dest), r26
	l.sw OR1K_CONTEXT_R27(\dest), r27
	l.sw OR1K_CONTEXT_R28(\dest), r28
	l.sw OR1K_CONTEXT_R29(\dest), r29
	l.sw OR1K_CONTEXT_R30(\dest), r30
	l.sw OR1K_CONTEXT_R31(\dest), r31

	/* Stack Pointer, r1. */
	OR1K_EXCEPTION_LOAD_SP(r3)
	l.sw OR1K_CONTEXT_R1(\dest),   r3

	/* Frame pointer. */
	OR1K_EXCEPTION_LOAD_GPR2(r3)
	l.sw OR1K_CONTEXT_R2(\dest),   r3

	/* EPCR. */
	l.mfspr r3, r0, OR1K_SPR_EPCR_BASE
	l.sw OR1K_CONTEXT_EPCR(\dest), r3

	/* EEAR. */
	l.mfspr r3, r0, OR1K_SPR_EEAR_BASE
	l.sw OR1K_CONTEXT_EEAR(\dest), r3

	/* ESR. */
	l.mfspr r3, r0, OR1K_SPR_ESR_BASE
	l.sw OR1K_CONTEXT_ESR(\dest),  r3

	/* Scratch registers: r3 ... r6. */
	OR1K_EXCEPTION_LOAD_GPR3(r3)
	l.sw OR1K_CONTEXT_R3(\dest), r3
	
	OR1K_EXCEPTION_LOAD_GPR4(r3)
	l.sw OR1K_CONTEXT_R4(\dest), r3

	OR1K_EXCEPTION_LOAD_GPR5(r3)
	l.sw OR1K_CONTEXT_R5(\dest), r3

	OR1K_EXCEPTION_LOAD_GPR6(r3)
	l.sw OR1K_CONTEXT_R6(\dest), r3

.endm

/*============================================================================*
 * or1k_context_restore()                                                     *
 *============================================================================*/

/*
 * Restores an execution context.
 */
.macro or1k_context_restore src

	/* Load GPRs. */
	l.lwz bp,  OR1K_CONTEXT_R2(\src)
	l.lwz r4,  OR1K_CONTEXT_R4(\src)
	l.lwz r5,  OR1K_CONTEXT_R5(\src)
	l.lwz r6,  OR1K_CONTEXT_R6(\src)
	l.lwz r7,  OR1K_CONTEXT_R7(\src)
	l.lwz r8,  OR1K_CONTEXT_R8(\src)
	l.lwz r9,  OR1K_CONTEXT_R9(\src)
	l.lwz r10, OR1K_CONTEXT_R10(\src)
	l.lwz r11, OR1K_CONTEXT_R11(\src)
	l.lwz r12, OR1K_CONTEXT_R12(\src)
	l.lwz r13, OR1K_CONTEXT_R13(\src)
	l.lwz r14, OR1K_CONTEXT_R14(\src)
	l.lwz r15, OR1K_CONTEXT_R15(\src)
	l.lwz r16, OR1K_CONTEXT_R16(\src)
	l.lwz r17, OR1K_CONTEXT_R17(\src)
	l.lwz r18, OR1K_CONTEXT_R18(\src)
	l.lwz r19, OR1K_CONTEXT_R19(\src)
	l.lwz r20, OR1K_CONTEXT_R20(\src)
	l.lwz r21, OR1K_CONTEXT_R21(\src)
	l.lwz r22, OR1K_CONTEXT_R22(\src)
	l.lwz r23, OR1K_CONTEXT_R23(\src)
	l.lwz r24, OR1K_CONTEXT_R24(\src)
	l.lwz r25, OR1K_CONTEXT_R25(\src)
	l.lwz r26, OR1K_CONTEXT_R26(\src)
	l.lwz r27, OR1K_CONTEXT_R27(\src)
	l.lwz r28, OR1K_CONTEXT_R28(\src)
	l.lwz r29, OR1K_CONTEXT_R29(\src)
	l.lwz r30, OR1K_CONTEXT_R30(\src)
	l.lwz r31, OR1K_CONTEXT_R31(\src)

	/* Special Purpose Registers: EPCR, EEAR and ESR. */
	l.lwz   r3, OR1K_CONTEXT_EPCR(\src)
	l.mtspr r0, r3, OR1K_SPR_EPCR_BASE

	l.lwz   r3, OR1K_CONTEXT_EEAR(\src)
	l.mtspr r0, r3, OR1K_SPR_EEAR_BASE

	l.lwz   r3, OR1K_CONTEXT_ESR(\src)
	l.mtspr r0, r3, OR1K_SPR_ESR_BASE

	/* Last scratch register, r3. */
	l.lwz r3, OR1K_CONTEXT_R3(\src)

	/* Stack Pointer, r1. */
	l.lwz sp, OR1K_CONTEXT_R1(\src)

.endm

/*===========================================================================*
 * _do_handler() macros                                                      *
 *===========================================================================*/

.macro _do_handler address handler handler_number

.org \address

	/* Backup scratch registers. */
	OR1K_EXCEPTION_STORE_GPR3
	OR1K_EXCEPTION_STORE_GPR4
	OR1K_EXCEPTION_STORE_GPR5
	OR1K_EXCEPTION_STORE_GPR6

	/* Save handler number. */
	l.ori r6, r0, \handler_number

	/* Jump into \handler. */
	l.j \handler
	l.nop

.endm

.macro _do_prologue

	/* Save stack and base pointer. */
	OR1K_EXCEPTION_STORE_SP
	OR1K_EXCEPTION_STORE_GPR2

.endm

.macro _do_epilogue

.endm

/*============================================================================*
 * hardcoded hw exceptions                                                    *
 *============================================================================*/


.section .bootstrap,"ax",@progbits

/*
 * In OpenRISC architecture, all the exceptions are hardcoded to a very
 * specific addresses.
 */

_do_handler 0x100, _do_start, OR1K_EXCP_RESET               /* Reset.                  */
_do_handler 0x200, _do_excp,  OR1K_EXCP_BUSERROR            /* Bus error.              */
_do_handler 0x300, _do_excp,  OR1K_EXCP_PAGE_FAULT          /* Data page fault.        */
_do_handler 0x400, _do_excp,  OR1K_EXCP_PAGE_FAULT          /* Instruction page fault. */
_do_handler 0x500, _do_hwint, OR1K_PC_INT_CLOCK             /* Tick timer.             */
_do_handler 0x600, _do_excp,  OR1K_EXCP_ALIGNMENT           /* Alignment.              */
_do_handler 0x700, _do_excp,  OR1K_EXCP_ILLEGAL_INSTRUCTION /* Illegal Instruction.    */
_do_handler 0x800, _do_hwint, OR1K_PC_INT_EXTERNAL          /* External Interrupt.     */
_do_handler 0x900, _do_excp,  OR1K_EXCP_DTLB_FAULT          /* DTLB Fault.             */
_do_handler 0xa00, _do_excp,  OR1K_EXCP_ITLB_FAULT          /* ITLB Fault.             */
_do_handler 0xb00, _do_excp,  OR1K_EXCP_RANGE               /* Range.                  */
_do_handler 0xc00, _syscall,  0                             /* Syscall.                */
_do_handler 0xd00, _do_excp,  OR1K_EXCP_FLOAT_POINT         /* Floating point.         */
_do_handler 0xe00, _do_excp,  OR1K_EXCP_TRAP                /* Trap.                   */

/*===========================================================================*
 * _do_excp()                                                                *
 *===========================================================================*/

/*
 * Exception hook.
 */
_do_excp:
	_do_prologue
	redzone_alloc

	/*
	 * Save execution context
	 * in the current stack.
	 */
	l.addi sp, sp, -OR1K_CONTEXT_SIZE
	or1k_context_save sp

	/*
	 * Save a reference to execution context
	 * into r4 (second parameter). The high-level
	 * exception handler dispatcher will need it.
	 *
	 * r4 should not be used as a scratch register
	 * from this point, until do_excp.
	 */
	l.ori  r4, sp, 0

	/*
	 * Save exception information
	 * in the current stack.
	 */
	l.addi sp, sp, -OR1K_EXCEPTION_SIZE
	l.sw OR1K_EXCEPTION_NUM (sp), r6

	l.mfspr r3, r0, OR1K_SPR_EEAR_BASE
	l.sw OR1K_EXCEPTION_EEAR(sp), r3
	
	l.mfspr r3, r0, OR1K_SPR_EPCR_BASE
	l.sw OR1K_EXCEPTION_EPCR(sp), r3

	/*
	 * Save a reference to exception information
	 * into r3 (first parameter). The high-level
	 * exception handler dispatcher will need it.
	 * 
	 * r3 should not be used as a scratch register
	 * from this point, until do_excp.
	 */
	l.ori  r3, sp, 0

	/*
	 * Call exception dispatcher.
	 */
	l.jal do_excp
	l.nop

	/*
	 * Wipe out exception information
	 * from current stack.
	 */
	l.addi sp, sp, OR1K_EXCEPTION_SIZE

	/*
	 * Restore saved execution context.
	 */
	or1k_context_restore sp

	_do_epilogue

	l.rfe
	l.nop

/*===========================================================================*
 * _do_hwint()                                                               *
 *===========================================================================*/

/*
 * Interrupt hook.
 */
_do_hwint:
	_do_prologue
	redzone_alloc

	/*
	 * Save execution context
	 * in the current stack.
	 */
	l.addi sp, sp, -OR1K_CONTEXT_SIZE
	or1k_context_save sp

	/*
	 * Save a reference to execution context
	 * into r4 (second parameter). The high-level
	 * interrupt handler will need it.
	 *
	 * r4 should not be used as a scratch register
	 * from this point, until do_hwint.
	 */
	l.ori r4, sp, 0

	/*
	 * Save a reference to interrupt information
	 * into r3 (first parameter). The high-level
	 * interrupt handler will need it.
	 * 
	 * r3 should not be used as a scratch register
	 * from this point, until do_hwint.
	 */
	l.or r3, r0, r6

	/*
	 * Call interrupt handler.
	 */
	l.jal or1k_do_hwint
	l.nop

	/*
	 * Restore saved execution context.
	 */
	or1k_context_restore sp

	_do_epilogue

	l.rfe
	l.nop

/*===========================================================================*
 * _syscall()                                                                *
 *===========================================================================*/

/*
 * System call hook.
 */
_syscall:
	_do_prologue
	redzone_alloc

	/*
	 * Save execution context
	 * in the current stack.
	 */
	l.addi sp, sp, -OR1K_CONTEXT_SIZE
	or1k_context_save sp

	/*
	 * Syscall parameters.
	 */
	l.lwz r3, OR1K_CONTEXT_R13(sp) /* arg6. */
	l.sw -8(sp), r3
	l.lwz r3, OR1K_CONTEXT_R11(sp) /* syscall_nr */
	l.sw -4(sp), r3

	l.lwz r8, OR1K_CONTEXT_R8(sp)  /* arg5. */
	l.lwz r7, OR1K_CONTEXT_R7(sp)  /* arg4. */
	l.lwz r6, OR1K_CONTEXT_R6(sp)  /* arg3. */
	l.lwz r5, OR1K_CONTEXT_R5(sp)  /* arg2. */
	l.lwz r4, OR1K_CONTEXT_R4(sp)  /* arg1. */
	l.lwz r3, OR1K_CONTEXT_R3(sp)  /* arg0. */

	l.addi sp, sp, -8

	/*
	 * Call syscall handler.
	 */
	l.jal do_syscall1
	l.nop
	l.addi sp, sp, 8

	/* Copy return value to the user stack. */
	l.sw OR1K_CONTEXT_R11(sp), r11

	/*
	 * Restore saved execution context.
	 */
	or1k_context_restore sp

	_do_epilogue

	l.rfe
	l.nop

/*===========================================================================*
 * _do_start()                                                               *
 *===========================================================================*/

/*
 * The very initial beggining of the OpenRISC processor is here, so it's
 * important to leave the registers in an initial state before the
 * start() is called.
 */

_do_start:
	l.ori r1, r0,  0; l.ori r2,  r0, 0; l.ori r3,  r0, 0; l.ori r4,  r0, 0;
	l.ori r5, r0,  0; l.ori r6,  r0, 0; l.ori r7,  r0, 0; l.ori r8,  r0, 0;
	l.ori r9, r0,  0; l.ori r10, r0, 0; l.ori r11, r0, 0; l.ori r12, r0, 0;
	l.ori r13, r0, 0; l.ori r14, r0, 0; l.ori r15, r0, 0; l.ori r16, r0, 0;
	l.ori r17, r0, 0; l.ori r18, r0, 0; l.ori r19, r0, 0; l.ori r20, r0, 0;
	l.ori r21, r0, 0; l.ori r22, r0, 0; l.ori r23, r0, 0; l.ori r24, r0, 0;
	l.ori r25, r0, 0; l.ori r26, r0, 0; l.ori r27, r0, 0; l.ori r28, r0, 0;
	l.ori r29, r0, 0; l.ori r30, r0, 0; l.ori r31, r0, 0;

	l.jal start
	l.nop
