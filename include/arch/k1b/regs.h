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

#ifndef ARCH_K1B_REGS_H_
#define ARCH_K1B_REGS_H_

	#ifndef _ASM_FILE_
	#error "do not include this in C source files"
	#endif

	/**
	 * @name Aliases for Registers
	 */
	/**@{*/
	#define sp r12 /**< Stack Pointer Pointer              */
	#define bp r13 /**< Stack Base Pointer Register        */
	#define pi pcr /**< Processing Identification Register */
	/**@}*/

/*============================================================================*
 * Red Zone                                                                   *
 *============================================================================*/

	/**
	 * @brief Size of red zone (in bytes).
	 */
	#define REDZONE_SIZE 16

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

/*============================================================================*
 * Prologue                                                                   *
 *============================================================================*/

	/**
	 * @brief Size of a stack frame (in bytes).
	 */
	#define STACK_FRAME_SIZE 16

	.macro _do_prologue

		/* Allocate a new stack frame. */
		add $sp, $sp, -STACK_FRAME_SIZE
		;;

		/* Save r0 and r1 registers. */
		sd 8[$sp] = $p0
		;;

		/* Save ra and bp registers. */
		get $r0 = $ra
		;;
		copy $r1 = $bp
		;;
		sd 0[$sp] = $p0
		;;

		/* Update stack base pointer. */
		copy $bp = $sp
		;;

	.endm

	.macro _do_epilogue

		/* Restore bp and ra registers. */
		ld $p0 = 0[$sp]
		;;
		set $ra = $r0
		;;
		copy $bp = $r1
		;;

		/* Restore r0 and r1 registers. */
		ld $p0 = 8[$sp]
		;;

		/* Wipe out frame. */
		add $sp, $sp, STACK_FRAME_SIZE
		;;

	.endm

#endif /* ARCH_K1B_REGS_H_ */
