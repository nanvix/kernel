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
#include <arch/i386/context.h>

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
.globl _do_hwint0
.globl _do_hwint1
.globl _do_hwint2
.globl _do_hwint3
.globl _do_hwint4
.globl _do_hwint5
.globl _do_hwint6
.globl _do_hwint7
.globl _do_hwint8
.globl _do_hwint9
.globl _do_hwint10
.globl _do_hwint11
.globl _do_hwint12
.globl _do_hwint13
.globl _do_hwint14
.globl _do_hwint15

/*============================================================================*
 * i386_context_save()                                                        *
 *============================================================================*/

/*
 * The i386_context_save() macro saves the content of general purpose
 * and segment registers in the stack of the caller. A pointer to the
 * saved execution context is saved into ret.
 *
 * Note that the extended instruction pointer register (eip), code
 * segment register (cs), extended flags register (eflags), extended
 * stack pointer register (esp) and stack segment register (ss) are
 * not saved by this macro, because they are automatically saved by
 * the hardware upon an exception/interrupt.
 */
.macro i386_context_save, ret

	subl $I386_CONTEXT_SW_SIZE, %esp

	/* Save general purpose registers. */
	movl %eax, I386_CONTEXT_EAX(%esp)
	movl %ebx, I386_CONTEXT_EBX(%esp)
	movl %ecx, I386_CONTEXT_ECX(%esp)
	movl %edx, I386_CONTEXT_EDX(%esp)
	movl %ebp, I386_CONTEXT_EBP(%esp)
	movl %esi, I386_CONTEXT_ESI(%esp)
	movl %edi, I386_CONTEXT_EDI(%esp)

	/* Save segment registers. */
	movw %ds, I386_CONTEXT_DS(%esp)
	movw %es, I386_CONTEXT_ES(%esp)
	movw %fs, I386_CONTEXT_FS(%esp)
	movw %gs, I386_CONTEXT_GS(%esp)

	movl %esp, \ret

.endm

/*============================================================================*
 * i386_context_restore()                                                     *
 *============================================================================*/

/**
 * The i386_context_restore() function restores the content of general
 * purpose and segment registers from the stack of the caller.
 *
 * @note It is up to the caller to switch the current stack to the
 * target one.
 */
.macro i386_context_restore

	/* Restore general purpose registers. */
	movl I386_CONTEXT_EAX(%esp), %eax
	movl I386_CONTEXT_EBX(%esp), %ebx
	movl I386_CONTEXT_ECX(%esp), %ecx
	movl I386_CONTEXT_EDX(%esp), %edx
	movl I386_CONTEXT_EBP(%esp), %ebp
	movl I386_CONTEXT_ESI(%esp), %esi
	movl I386_CONTEXT_EDI(%esp), %edi

	/* Restore segment registers. */
	movw I386_CONTEXT_DS(%esp), %ds
	movw I386_CONTEXT_ES(%esp), %es
	movw I386_CONTEXT_FS(%esp), %fs
	movw I386_CONTEXT_GS(%esp), %gs

	addl $I386_CONTEXT_SW_SIZE, %esp

	/* Restore special registers. */
	iret

.endm

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

    i386_context_restore

/*============================================================================*
 * syscall()                                                                  *
 *============================================================================*/

/*
 * System call hook.
 */
syscall:
	iret

/*============================================================================*
 * i386_pic_master_ack()                                                      *
 *============================================================================*/

/*
 * Acknowledges an interrupt to the master PIC.
 */
.macro i386_pic_master_ack
	movb $0x20, %al
	outb %al, $PIC_CTRL_MASTER
.endm

/*============================================================================*
 * i386_pic_slave_ack()                                                       *
 *============================================================================*/

/*
 * Acknowledges an interrupt to the slave PIC.
 */
.macro i386_pic_slave_ack
	movb $0x20, %al
	outb %al, $PIC_CTRL_MASTER
	outb %al, $PIC_CTRL_SLAVE
.endm

/*============================================================================*
 * _do_hwint()                                                                *
 *============================================================================*/

/*
 * Low-level hardware interrupt dispatcher.
 */
.macro _do_hwint_master, num
	_do_hwint\()\num:
		i386_context_save %ebx
		i386_pic_master_ack
		pushl %ebx
		pushl $(\num)
		call i386_do_hwint
		addl $8, %esp
		i386_context_restore
.endm

/*
 * Low-level hardware interrupt dispatcher.
 */
.macro _do_hwint_slave, num
	_do_hwint\()\num:
		i386_context_save %ebx
		i386_pic_slave_ack
		pushl %ebx
		pushl $(\num)
		call i386_do_hwint
		addl $8, %esp
		i386_context_restore
.endm

/* Hardware interrupt hooks. */
_do_hwint_master  0
_do_hwint_master  1
_do_hwint_master  2
_do_hwint_master  3
_do_hwint_master  4
_do_hwint_master  5
_do_hwint_master  6
_do_hwint_master  7
_do_hwint_slave   8
_do_hwint_slave   9
_do_hwint_slave  10
_do_hwint_slave  11
_do_hwint_slave  12
_do_hwint_slave  13
_do_hwint_slave  14
_do_hwint_slave  15
