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

#include <arch/core/i386/8259.h>
#include <arch/core/i386/context.h>
#include <arch/core/i386/core.h>
#include <arch/core/i386/excp.h>
#include <arch/core/i386/gdt.h>

/* Exported symbols. */
.globl _do_excp0
.globl _do_excp1
.globl _do_excp2
.globl _do_excp3
.globl _do_excp4
.globl _do_excp5
.globl _do_excp6
.globl _do_excp7
.globl _do_excp8
.globl _do_excp9
.globl _do_excp10
.globl _do_excp11
.globl _do_excp12
.globl _do_excp13
.globl _do_excp14
.globl _do_excp15
.globl _do_excp16
.globl _do_excp17
.globl _do_excp18
.globl _do_excp19
.globl _do_excp20
.globl _do_excp30
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
 * _do_excp()                                                                 *
 *============================================================================*/

/*
 * Offset to exception structure.
 */
#define I386_EXCEPTION_SKIP -I386_CONTEXT_SW_SIZE - I386_EXCEPTION_SIZE + I386_EXCEPTION_ERR

/*
 * Exception without error code.
 */
.macro _do_excp_noerr_code, number
	_do_excp\()\number:
		i386_context_save %eax
		movl $(\number), %ebx
		movl $0, %ecx
		jmp _do_excp
.endm

/*
 * Exception with error code.
 */
.macro _do_excp_err_code, number
	_do_excp\()\number:
		xchg %eax, (%esp)
		xchg %eax, I386_EXCEPTION_SKIP(%esp)
		xchg %eax, (%esp)
		i386_context_save %eax
		movl $(\number), %ebx
		movl $0, %ecx
		jmp _do_excp
.endm

/*
 * Exception with error code.
 */
.macro _do_excp_err2_code, number
	_do_excp\()\number:
		xchg %eax, (%esp)
		xchg %eax, I386_EXCEPTION_SKIP(%esp)
		xchg %eax, (%esp)
		i386_context_save %eax
		movl $(\number), %ebx
		movl %cr2, %ecx
		jmp _do_excp
.endm

/* Exception hooks. */
_do_excp_noerr_code  0 /* Division-by-Zero Error      */
_do_excp_noerr_code  1 /* Debug Exception             */
_do_excp_noerr_code  2 /* Non-Maskable Interrupt      */
_do_excp_noerr_code  3 /* Breakpoint Exception        */
_do_excp_noerr_code  4 /* Overflow Exception          */
_do_excp_noerr_code  5 /* Bounds Check Exception      */
_do_excp_noerr_code  6 /* Invalid Opcode Exception    */
_do_excp_noerr_code  7 /* Coprocessor Not Available   */
_do_excp_err_code    8 /* Double Fault                */
_do_excp_noerr_code  9 /* Coprocessor Segment Overrun */
_do_excp_err_code   10 /* Invalid TSS                 */
_do_excp_err_code   11 /* Segment Not Present         */
_do_excp_err_code   12 /* Stack Segment Fault         */
_do_excp_err_code   13 /* General Protection Fault    */
_do_excp_err2_code  14 /* Page Fault                  */
_do_excp_noerr_code 15 /* Reserved                    */
_do_excp_noerr_code 16 /* Floating Point Exception    */
_do_excp_err_code   17 /* Alignment Check Exception   */
_do_excp_noerr_code 18 /* Machine Check Exception     */
_do_excp_noerr_code 19 /* SMID Unit Exception         */
_do_excp_noerr_code 20 /* Virtual Exception           */
_do_excp_err_code   30 /* Security Exception.         */

/*
 * Low-level exception handler dispatcher.
 */
_do_excp:
	/* Save exception information. */
	movl I386_CONTEXT_EIP(%eax), %edx
	subl $I386_EXCEPTION_SIZE, %esp
	movl %ebx, I386_EXCEPTION_NUM(%esp)
	movl %ecx, I386_EXCEPTION_DATA(%esp)
	movl %edx, I386_EXCEPTION_CODE(%esp)
	movl %esp, %ebx

	/* Call high-level exception dispatcher. */
	pushl %eax /* Execution context. */
	pushl %ebx /* Exception context. */
	call  do_excp
	addl  $(2*DWORD_SIZE), %esp

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
		addl $(2*DWORD_SIZE), %esp
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
		addl $(2*DWORD_SIZE), %esp
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
