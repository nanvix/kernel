/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/* Must come first. */
#define _ASM_FILE_

/*============================================================================*
 * Imported Symbols                                                           *
 *============================================================================*/

#include <asm/x86.S>
#include <nanvix/kernel/hal.h>

.extern do_exception
.extern do_kcall
.extern do_interrupt
.extern do_thread_setup
.extern tss

/*============================================================================*
 * Exported Symbols                                                           *
 *============================================================================*/

/* Exception hooks. */
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

/* Kernel call hook. */
.globl _do_kcall

/* Hardware interrupt hooks. */
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

/* Other */
.globl __context_switch
.globl __do_thread_setup
.globl __leave_kernel

/*============================================================================*
 * Text Section                                                               *
 *============================================================================*/

.section .text,"ax",@progbits

/*----------------------------------------------------------------------------*
 * _do_excp()                                                                 *
 *----------------------------------------------------------------------------*/

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
	movl CONTEXT_EIP(%eax), %edx
	subl $EXCEPTION_SIZE, %esp
	movl %ebx, EXCEPTION_NR(%esp)
	movl %ecx, EXCEPTION_DATA(%esp)
	movl %edx, EXCEPTION_CODE(%esp)
	movl EXCEPTION_ERR(%esp), %ebx
	movl %ebx, CONTEXT_ERR(%eax)
	movl %esp, %ebx

	/* Call high-level exception dispatcher. */
	pushl %eax /* Execution context. */
	pushl %ebx /* Exception context. */
	call  do_exception
	addl  $(2*WORD_SIZE), %esp
	addl $EXCEPTION_SIZE, %esp

	context_restore

	/* Pop error code. */
	addl $WORD_SIZE, %esp

    jmp __leave_kernel

/*----------------------------------------------------------------------------*
 * _do_kcall()                                                                *
 *----------------------------------------------------------------------------*/

/* Kernel call hook. */
_do_kcall:

    /* Push kernel call parameters. */
    pushl %eax
    pushl %esi
    pushl %edi
    pushl %edx
    pushl %ecx
    pushl %ebx

    cli

    /* Handle kernel call. */
    call do_kcall

    sti

    /* Wipe out kernel call parameters. */
    addl $6*WORD_SIZE, %esp

    jmp __leave_kernel

/*----------------------------------------------------------------------------*
 * _do_hwint()                                                                *
 *----------------------------------------------------------------------------*/

/* Hardware interrupt hooks. */
_do_hwint  0
_do_hwint  1
_do_hwint  2
_do_hwint  3
_do_hwint  4
_do_hwint  5
_do_hwint  6
_do_hwint  7
_do_hwint  8
_do_hwint  9
_do_hwint 10
_do_hwint 11
_do_hwint 12
_do_hwint 13
_do_hwint 14
_do_hwint 15


/*----------------------------------------------------------------------------*
 * __context_switch()                                                         *
 *----------------------------------------------------------------------------*/

/*
 * Saves the execution context of the calling thread.
 */
__context_switch:
    movl 4(%esp), %eax
    movl 8(%esp), %edx

    /* Save execution context. */
    movl %ebx, CONTEXT_EBX(%eax)
    movl %esi, CONTEXT_ESI(%eax)
    movl %edi, CONTEXT_EDI(%eax)
    movl %ebp, CONTEXT_EBP(%eax)
    movl %esp, CONTEXT_ESP(%eax)
    pushf
    pop CONTEXT_EFLAGS(%eax)

    /* Save address space. */
    movl %cr3, %ebx
    movl %ebx, CONTEXT_CR3(%eax)

    /* Restore execution context. */
    movl CONTEXT_EBX(%edx), %ebx
    movl CONTEXT_ESI(%edx), %esi
    movl CONTEXT_EDI(%edx), %edi
    movl CONTEXT_EBP(%edx), %ebp
    movl CONTEXT_ESP(%edx), %esp
    push CONTEXT_EFLAGS(%edx)
    popfl

    /* Restore address space.*/
    movl CONTEXT_CR3(%edx), %eax
    movl %eax, %cr3

    /* Update ESP0 on TSS. */
    movl CONTEXT_ESP0(%edx), %eax
    movl $tss, %edx
    movl %eax, TSS_ESP0(%edx)

    __context_switch.out:
    ret

/*----------------------------------------------------------------------------*
 * __leave_kernel()                                                           *
 *----------------------------------------------------------------------------*/

/*
 * Leaves the kernel.
 */
__do_thread_setup:
    call do_thread_setup

    /*
     * Restore data segment registers.
     */
    movl 16(%esp), %eax /* eax <= esp0 */
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

__leave_kernel:
    iret
