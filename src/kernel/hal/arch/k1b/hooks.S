/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#include <mOS_common_types_s_c.h>

/**
 * @brief Size of red zone (in bytes).
 */
#define REDZONE_SIZE 16

/**
 * @name Aliases for Registers
 */
/**@{*/
#define bp r10 /**< Base pointer. */
#define sp r12 /**< Stack pointer */
/**@}*/

.global _syscall
.global _do_excp

.section .text

/*
 * Allocates the red zone in the current stack frame.
 */
.macro redzone_alloc
	add $sp = $sp, -REDZONE_SIZE
.endm

/*
 * Frees the red zone in the current stack frame.
 */
.macro redzone_free
	add $sp = $sp, REDZONE_SIZE
.endm

/*===========================================================================*
 * _syscall()                                                                *
 *===========================================================================*/

/*
 * System call hook.
 */
.align 8
_syscall:
	/* Save system call context. */
	add $sp = $sp, -8
	;;
	get $r8 = $ra
	;;
	copy $bp = $r7
	;;
	sw 0 [$sp] = $bp
	;;
	sw 4 [$sp] = $r8
	;;

	/*
	 * Invoke system call dispatcher.
	 */
	redzone_alloc
	;;
	call do_syscall
	;;
	redzone_free
	;;

	/* Restore system call context. */
	lw $r8 = 4 [$sp]
	;;
	lw $bp = 0 [$sp]
	;;
	add $sp = $sp, 8
	;;
	set $ra = $r8
	;;
	scall MOS_VC_RFE
	;;
	ret
	;;

/*===========================================================================*
 * _do_excp()                                                                *
 *===========================================================================*/

/*
 * System call hook.
 */
.align 8
_do_excp:
	/* Save exception context. */
	add $sp = $sp, -8
	;;
	get $r8 = $ra
	;;
	copy $bp = $r7
	;;
	sw 0 [$sp] = $bp
	;;
	sw 4 [$sp] = $r8
	;;

	/*
	 * Get exception code.
	 */
	get $r1 = $ps
	;;
	srl $r0 = $r1, 28
	;;

	/*
	 * Invoke exception dispatcher.
	 */
	redzone_alloc
	;;
	call do_excp
	;;
	redzone_free
	;;

	/* Restore exception context. */
	lw $r8 = 4 [$sp]
	;;
	lw $bp = 0 [$sp]
	;;
	add $sp = $sp, 8
	;;
	set $ra = $r8
	;;
	scall MOS_VC_RFE
	;;
	ret
	;;
