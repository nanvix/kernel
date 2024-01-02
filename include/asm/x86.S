/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ASM_X86_S_
#define ASM_X86_S_

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * clear_gprs()                                                               *
 *============================================================================*/

/*
 * Clears all GPR registers.
 */
.macro clear_gprs

    xorl %eax, %eax
    xorl %ebx, %ebx
    xorl %ecx, %ecx
    xorl %edx, %edx
    xorl %esi, %esi
    xorl %edi, %edi

.endm

/*============================================================================*
 * reset_stack()                                                              *
 *============================================================================*/

/*
 * Resets the stack.
 */
.macro reset_stack
    /* TODO: Make this macro generic on the kstack size. */

    movl $kstack, %esp
    addl $4092, %esp   /* Frame pointer. */
    movl %esp, %ebp    /* Frame pointer. */

.endm

/*============================================================================*
 * halt()                                                                     *
 *============================================================================*/

/*
 * Halts execution
 */
.macro halt

    1: hlt
       jmp 1b

.endm


/*============================================================================*
 * context_save()                                                             *
 *============================================================================*/

/*
 * The context_save() macro saves the content of general purpose
 * and segment registers in the stack of the caller. A pointer to the
 * saved execution context is saved into ret.
 *
 * Note that the extended instruction pointer register (eip), code
 * segment register (cs), extended flags register (eflags), extended
 * stack pointer register (esp) and stack segment register (ss) are
 * not saved by this macro, because they are automatically saved by
 * the hardware upon an exception/interrupt.
 */
.macro context_save, ret

    subl $CONTEXT_SW_SIZE, %esp

    /* Save general purpose registers. */
    movl %eax, CONTEXT_EAX(%esp)
    movl %ebx, CONTEXT_EBX(%esp)
    movl %ecx, CONTEXT_ECX(%esp)
    movl %edx, CONTEXT_EDX(%esp)
    movl %ebp, CONTEXT_EBP(%esp)
    movl %esi, CONTEXT_ESI(%esp)
    movl %edi, CONTEXT_EDI(%esp)

    /* Save segment registers. */
    movw %ds, CONTEXT_DS(%esp)
    movw %es, CONTEXT_ES(%esp)
    movw %fs, CONTEXT_FS(%esp)
    movw %gs, CONTEXT_GS(%esp)

    movl %esp, \ret

.endm

/*============================================================================*
 * context_restore()                                                          *
 *============================================================================*/

/*
 * The context_restore() macro restores the content of general
 * purpose and segment registers from the stack of the caller.
 *
 * It is up to the caller to switch the current stack to the target one.
 */
.macro context_restore

    /* Restore general purpose registers. */
    movl CONTEXT_EAX(%esp), %eax
    movl CONTEXT_EBX(%esp), %ebx
    movl CONTEXT_ECX(%esp), %ecx
    movl CONTEXT_EDX(%esp), %edx
    movl CONTEXT_EBP(%esp), %ebp
    movl CONTEXT_ESI(%esp), %esi
    movl CONTEXT_EDI(%esp), %edi

    /* Restore segment registers. */
    movw CONTEXT_DS(%esp), %ds
    movw CONTEXT_ES(%esp), %es
    movw CONTEXT_FS(%esp), %fs
    movw CONTEXT_GS(%esp), %gs

    addl $CONTEXT_SW_SIZE, %esp

    /*
     * Special registers are
     * restored by a further call to iret.
     */

.endm


/*============================================================================*
 * _do_excp()                                                                 *
 *============================================================================*/

/*
 * Offset to exception structure.
 */
#define EXCEPTION_SKIP -CONTEXT_SW_SIZE - EXCEPTION_SIZE + EXCEPTION_ERR

/*
 * Exception hook.
 */
.macro _do_excp_noerr_code, number
	_do_excp\()\number:
        push $0
		xchg %eax, (%esp)
		xchg %eax, EXCEPTION_SKIP(%esp)
		xchg %eax, (%esp)
		context_save %eax
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
		xchg %eax, EXCEPTION_SKIP(%esp)
		xchg %eax, (%esp)
		context_save %eax
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
		xchg %eax, EXCEPTION_SKIP(%esp)
		xchg %eax, (%esp)
		context_save %eax
		movl $(\number), %ebx
		movl %cr2, %ecx
		jmp _do_excp
.endm

/*============================================================================*
 * _do_hwint()                                                                *
 *============================================================================*/

/*
 * Low-level hardware interrupt dispatcher.
 */
.macro _do_hwint num
	_do_hwint\()\num:
		context_save %eax
		pushl $(\num)
		call do_interrupt
		addl $WORD_SIZE, %esp
		context_restore
		iret
.endm


#endif /* ASM_X86_S_ */
