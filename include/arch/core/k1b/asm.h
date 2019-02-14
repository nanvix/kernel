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

#ifndef ARCH_CORE_K1B_ASM_H_
#define ARCH_CORE_K1B_ASM_H_

	#ifndef _ASM_FILE_
		#error "do not include this header in C files"
	#endif

	#include <arch/core/k1b/context.h>

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

/*============================================================================*
 * k1b_context_save()                                                         *
 *============================================================================*/

	/*
	 * Saves the current execution context in the current stack.
	 */
	.macro k1b_context_save dest

		/* Allocate some memory. */
		add $r12, $r12, -K1B_CONTEXT_SIZE
		;;

		/* Save GPRs. */
		sd K1B_CONTEXT_R0[$r12], $p0   /**< r0  + r1  */
		;;
		sd K1B_CONTEXT_R2[$r12], $p2   /**< r2  + r3  */
		;;
		sd K1B_CONTEXT_R4[$r12], $p4   /**< r4  + r5  */
		;;
		sd K1B_CONTEXT_R6[$r12], $p6   /**< r6  + r7  */
		;;
		sd K1B_CONTEXT_R8[$r12], $p8   /**< r8  + r9  */
		;;
		sd K1B_CONTEXT_R10[$r12], $p10 /**< r10 + r11 */
		;;
		copy $r1 = $r13
		;;
		copy $r0 = $r12
		;;
		add $r0, $r0, K1B_CONTEXT_SIZE
		;;
		sd K1B_CONTEXT_R12[$r12], $p0  /**< r12 + r13 */
		;;
		sd K1B_CONTEXT_R14[$r12], $p14 /**< r14 + r15 */
		;;
		sd K1B_CONTEXT_R16[$r12], $p16 /**< r16 + r17 */
		;;
		sd K1B_CONTEXT_R18[$r12], $p18 /**< r18 + r19 */
		;;
		sd K1B_CONTEXT_R20[$r12], $p20 /**< r20 + r21 */
		;;
		sd K1B_CONTEXT_R22[$r12], $p22 /**< r22 + r23 */
		;;
		sd K1B_CONTEXT_R24[$r12], $p24 /**< r24 + r25 */
		;;
		sd K1B_CONTEXT_R26[$r12], $p26 /**< r26 + r27 */
		;;
		sd K1B_CONTEXT_R28[$r12], $p28 /**< r28 + r29 */
		;;
		sd K1B_CONTEXT_R30[$r12], $p30 /**< r30 + r31 */
		;;
		sd K1B_CONTEXT_R32[$r12], $p32 /**< r32 + r33 */
		;;
		sd K1B_CONTEXT_R34[$r12], $p34 /**< r34 + r35 */
		;;
		sd K1B_CONTEXT_R36[$r12], $p36 /**< r36 + r37 */
		;;
		sd K1B_CONTEXT_R38[$r12], $p38 /**< r38 + r39 */
		;;
		sd K1B_CONTEXT_R40[$r12], $p40 /**< r40 + r41 */
		;;
		sd K1B_CONTEXT_R42[$r12], $p42 /**< r42 + r43 */
		;;
		sd K1B_CONTEXT_R44[$r12], $p44 /**< r44 + r45 */
		;;
		sd K1B_CONTEXT_R46[$r12], $p46 /**< r46 + r47 */
		;;
		sd K1B_CONTEXT_R48[$r12], $p48 /**< r48 + r49 */
		;;
		sd K1B_CONTEXT_R50[$r12], $p50 /**< r50 + r51 */
		;;
		sd K1B_CONTEXT_R52[$r12], $p52 /**< r52 + r53 */
		;;
		sd K1B_CONTEXT_R54[$r12], $p54 /**< r54 + r55 */
		;;
		sd K1B_CONTEXT_R56[$r12], $p56 /**< r56 + r57 */
		;;
		sd K1B_CONTEXT_R58[$r12], $p58 /**< r58 + r59 */
		;;
		sd K1B_CONTEXT_R60[$r12], $p60 /**< r60 + r61 */
		;;
		sd K1B_CONTEXT_R62[$r12], $p62 /**< r62 + r63 */
		;;

		/* Get SFRs*/
		get   $r4 = $ra
		;;
		get   $r5 = $cs
		;;
		get   $r8 = $lc
		;;
		get   $r9 = $ls
		;;
		get   $r10 = $le
		;;

		/*
		 * Save SFRs. Note that $spc,
		 * $ps and $sps are retrieved from mOS.
		 */
		sw K1B_CONTEXT_RA[$r12],  $r4
		;;
		sw K1B_CONTEXT_CS[$r12],  $r5
		;;
		sw K1B_CONTEXT_LC[$r12],  $r8
		;;
		sw K1B_CONTEXT_LS[$r12],  $r9
		;;
		sw K1B_CONTEXT_LE[$r12],  $r10
	.endm

/*============================================================================*
 * k1b_context_restore()                                                      *
 *============================================================================*/

	/*
	 * Restores an execution context from the current stack.
	 */
	.macro k1b_context_restore src

		/*
		 * Restore SFRs. Note that $spc,
		 * $ps and $sps are restored by mOS.
		 */
		lw $r0  = K1B_CONTEXT_RA[$r12]
		;;
		lw $r1  = K1B_CONTEXT_CS[$r12]
		;;
		lw $r2  = K1B_CONTEXT_LC[$r12]
		;;
		lw $r3  = K1B_CONTEXT_LS[$r12]
		;;
		lw $r4 = K1B_CONTEXT_LE[$r12]
		;;

		/* Set SFRs. */
		set   $ra = $r0
		;;
		set   $cs = $r1
		;;
		set   $lc = $r2
		;;
		set   $ls = $r3
		;;
		set   $le = $r4
		;;

		/* Restore GPRs. */
		ld $p0  = K1B_CONTEXT_R0[$r12]  /**< r0  + r1  */
		;;
		ld $p2  = K1B_CONTEXT_R2[$r12]  /**< r2  + r3  */
		;;
		ld $p4  = K1B_CONTEXT_R4[$r12]  /**< r4  + r5  */
		;;
		ld $p6  = K1B_CONTEXT_R6[$r12]  /**< r6  + r7  */
		;;
		ld $p8  = K1B_CONTEXT_R8[$r12]  /**< r8  + r9  */
		;;
		ld $p10 = K1B_CONTEXT_R10[$r12] /**< r10 + r11 */
		;;
		lw $r13 = K1B_CONTEXT_R13[$r12] /**< r13       */
		;;
		ld $p14 = K1B_CONTEXT_R14[$r12] /**< r14 + r15 */
		;;
		ld $p16 = K1B_CONTEXT_R16[$r12] /**< r16 + r17 */
		;;
		ld $p18 = K1B_CONTEXT_R18[$r12] /**< r18 + r19 */
		;;
		ld $p20 = K1B_CONTEXT_R20[$r12] /**< r20 + r21 */
		;;
		ld $p22 = K1B_CONTEXT_R22[$r12] /**< r22 + r23 */
		;;
		ld $p24 = K1B_CONTEXT_R24[$r12] /**< r24 + r25 */
		;;
		ld $p26 = K1B_CONTEXT_R26[$r12] /**< r26 + r27 */
		;;
		ld $p28 = K1B_CONTEXT_R28[$r12] /**< r28 + r29 */
		;;
		ld $p30 = K1B_CONTEXT_R30[$r12] /**< r30 + r31 */
		;;
		ld $p32 = K1B_CONTEXT_R32[$r12] /**< r32 + r33 */
		;;
		ld $p34 = K1B_CONTEXT_R34[$r12] /**< r34 + r35 */
		;;
		ld $p36 = K1B_CONTEXT_R36[$r12] /**< r36 + r37 */
		;;
		ld $p38 = K1B_CONTEXT_R38[$r12] /**< r38 + r39 */
		;;
		ld $p40 = K1B_CONTEXT_R40[$r12] /**< r40 + r41 */
		;;
		ld $p42 = K1B_CONTEXT_R42[$r12] /**< r42 + r43 */
		;;
		ld $p44 = K1B_CONTEXT_R44[$r12] /**< r44 + r45 */
		;;
		ld $p46 = K1B_CONTEXT_R46[$r12] /**< r46 + r47 */
		;;
		ld $p48 = K1B_CONTEXT_R48[$r12] /**< r48 + r49 */
		;;
		ld $p50 = K1B_CONTEXT_R50[$r12] /**< r50 + r51 */
		;;
		ld $p52 = K1B_CONTEXT_R52[$r12] /**< r52 + r53 */
		;;
		ld $p54 = K1B_CONTEXT_R54[$r12] /**< r54 + r55 */
		;;
		ld $p56 = K1B_CONTEXT_R56[$r12] /**< r56 + r57 */
		;;
		ld $p58 = K1B_CONTEXT_R58[$r12] /**< r58 + r59 */
		;;
		ld $p60 = K1B_CONTEXT_R60[$r12] /**< r60 + r61 */
		;;
		ld $p62 = K1B_CONTEXT_R62[$r12] /**< r62 + r63 */
		;;

		/* Wipe out allocated memory. */
		add $r12, $r12, K1B_CONTEXT_SIZE
		;;
	.endm

#endif /* ARCH_CORE_K1B_ASM_H_ */
