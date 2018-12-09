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
#include <arch/i386/gdt.h>

/* Exported symbols. */
.globl excp0
.globl excp1
.globl excp2
.globl excp3
.globl excp4
.globl excp5
.globl excp6
.globl excp7
.globl excp8
.globl excp9
.globl excp10
.globl excp11
.globl excp12
.globl excp13
.globl excp14
.globl excp15
.globl excp16
.globl excp17
.globl excp18
.globl excp19
.globl excp20
.globl excp30
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
	pushl %ebx
	pushl %ecx
	pushl %edx
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
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax

	iret

/*============================================================================*
 * excp()                                                                     *
 *============================================================================*/

/*
 * Exception without error code.
 */
.macro excp_noerr_code, number
	excp\()\number:
		pushl %eax
		movl $0, %eax
		pushl %ebx
		xchg %ebx, 8(%esp)
		pushl %ecx
		movl $(\number), %ecx
		jmp _excp
.endm

/*
 * Exception with error code.
 */
.macro excp_err_code, number
	excp\()\number:
		xchg %eax, (%esp)
		pushl %ebx
		xchg %ebx, 8(%esp)
		pushl %ecx
		movl $(\number), %ecx
		jmp _excp
.endm

/*
 * Exception with error code.
 */
.macro excp_err2_code, number
	excp\()\number:
		xchg %eax, (%esp)
		pushl %ebx
		movl %cr2, %ebx
		pushl %ecx
		movl $(\number), %ecx
		jmp _excp
.endm

/* Exception hooks. */
excp_noerr_code  0 /* Division-by-Zero Error      */
excp_noerr_code  1 /* Debug Exception             */
excp_noerr_code  2 /* Non-Maskable Interrupt      */
excp_noerr_code  3 /* Breakpoint Exception        */
excp_noerr_code  4 /* Overflow Exception          */
excp_noerr_code  5 /* Bounds Check Exception      */
excp_noerr_code  6 /* Invalid Opcode Exception    */
excp_noerr_code  7 /* Coprocessor Not Available   */
excp_err_code    8 /* Double Fault                */
excp_noerr_code  9 /* Coprocessor Segment Overrun */
excp_err_code   10 /* Invalid TSS                 */
excp_err_code   11 /* Segment Not Present         */
excp_err_code   12 /* Stack Segment Fault         */
excp_err_code   13 /* General Protection Fault    */
excp_err2_code  14 /* Page Fault                  */
excp_noerr_code 15 /* Reserved                    */
excp_noerr_code 16 /* Floating Point Exception    */
excp_err_code   17 /* Alignment Check Exception   */
excp_noerr_code 18 /* Machine Check Exception     */
excp_noerr_code 19 /* SMID Unit Exception         */
excp_noerr_code 20 /* Virtual Exception           */
excp_err_code   30 /* Security Exception.         */

/*
 * Low-level exception handler dispatcher.
 */
_excp:
	/* Save registers. */
	pushl %edx
	pushl %ebp
	pushl %esi
	pushl %edi
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs

	movw $KERNEL_DS, %bx
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %fs
	movw %bx, %gs

	/* Retrieve stack pointer. */
	movl %esp, %edx
	addl 64, %edx

	/* Call high-level exception handler. */
	pushl %eax /* Error code.       */
	pushl %ebx /* Address.          */
	pushl %ecx /* Exception number. */
	pushl %edx /* Context.          */
	call do_excp
	add $16, %esp

    jmp context_restore

/*============================================================================*
 * syscall()                                                                  *
 *============================================================================*/

/*
 * System call hook.
 */
syscall:
	iret

/*============================================================================*
 * hwint()                                                                    *
 *============================================================================*/

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
