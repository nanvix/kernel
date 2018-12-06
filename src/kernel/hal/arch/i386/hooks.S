/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Must come first. */
#define _ASM_FILE_

#include <arch/i386/8259.h>

/* Exported symbols. */
.globl swint0
.globl swint1
.globl swint2
.globl swint3
.globl swint4
.globl swint5
.globl swint6
.globl swint7
.globl swint8
.globl swint9
.globl swint10
.globl swint11
.globl swint12
.globl swint13
.globl swint14
.globl swint15
.globl swint16
.globl swint17
.globl syscall
.globl hwint0
.globl hwint1
.globl hwint2
.globl hwint3
.globl hwint4
.globl hwint5
.globl hwint6
.globl hwint7
.globl hwint8
.globl hwint9
.globl hwint10
.globl hwint11
.globl hwint12
.globl hwint13
.globl hwint14
.globl hwint15

/*============================================================================*
 * context_save()                                                             *
 *============================================================================*/

/**
 * The context_save() function saves the content of general purpose
 * and segment registers in the stack of the caller.
 *
 * @note It is up to the caller to save the content of stack, flags
 * and program counter registers, as well as switching the current stack
 * to elsewhere.
 */
.macro context_save

	/* General purpose registers. */
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi
	
	/* Segment registers. */
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
.endm

/*============================================================================*
 * context_restore()                                                          *
 *============================================================================*/

/**
 * The context_restore() function restores the content of general
 * purpose and segment registers from the stack of the caller.
 *
 * @note It is up to the caller to switch the current stack to the
 * target one.
 */
context_restore:

	/* Segment registers. */
	popl %gs
	popl %fs
	popl %es
	popl %ds

	/* General purpose registers. */
	popl %edi
	popl %esi
	popl %ebp
	popl %ebx
	popl %edx
	popl %ecx
	popl %eax

	iret

/*----------------------------------------------------------------------------*
 *                                  swint()                                   *
 *----------------------------------------------------------------------------*/

/*
 * Software interrupt without error code.
 */
.macro swint_noerr_code, number, handler
	swint\()\number:
		iret
.endm

/*
 * Software interrupt with error code.
 */
.macro swint_err_code, number, handler
	swint\()\number:
		iret
.endm

/* Software interrupt hooks. */
swint_noerr_code  0 do_divide
swint_noerr_code  1 do_debug
swint_noerr_code  2 do_nmi
swint_noerr_code  3 do_breakpoint
swint_noerr_code  4 do_overflow
swint_noerr_code  5 do_bounds
swint_noerr_code  6 do_invalid_opcode
swint_noerr_code  7 do_coprocessor_not_available
swint_err_code    8 do_double_fault
swint_noerr_code  9 do_coprocessor_segment_overrun
swint_err_code   10 do_invalid_tss
swint_err_code   11 do_segment_not_present
swint_err_code   12 do_stack_exception
swint_err_code   13 do_general_protection
swint_err_code   14 _do_page_fault
swint_noerr_code 15 do_reserved
swint_noerr_code 16 do_coprocessor_error
swint_noerr_code 17 do_reserved

/*----------------------------------------------------------------------------*
 *                                 syscall()                                  *
 *----------------------------------------------------------------------------*/

/*
 * System call hook.
 */
syscall:
	iret

/*----------------------------------------------------------------------------*
 *                                   hwint()                                  *
 *----------------------------------------------------------------------------*/

/*
 * Master hardware interrupt.
 */
.macro hwint_master, num
	hwint\()\num:
		context_save
		movb $0x20, %al
		outb %al, $PIC_CTRL_MASTER /* Send reset signal to master. */
		pushl $(\num)
		call do_hwint
		addl $4, %esp
		jmp context_restore
.endm

/*
 * Slave hardware interrupt.
 */
.macro hwint_slave, num
	hwint\()\num:
		context_save
		movb $0x20, %al
		outb %al, $PIC_CTRL_MASTER /* Send reset signal to master. */
		outb %al, $PIC_CTRL_SLAVE  /* Send reset signal to slave.  */
		pushl $(\num)
		call do_hwint
		addl $4, %esp
		jmp context_restore
.endm

/* Hardware interrupt hooks. */
hwint_master 0
hwint_master 1
hwint_master 2
hwint_master 3
hwint_master 4
hwint_master 5
hwint_master 6
hwint_master 7
hwint_slave  8
hwint_slave  9
hwint_slave 10
hwint_slave 11
hwint_slave 12
hwint_slave 13
hwint_slave 14
hwint_slave 15
