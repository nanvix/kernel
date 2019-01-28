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

/* Must come first. */
#define _ASM_FILE_

#include <arch/k1b/context.h>
#include <arch/k1b/excp.h>
#include <mOS_common_types_s_c.h>

/**
 * @brief Size of red zone (in bytes).
 */
#define REDZONE_SIZE 16

/**
 * @name Aliases for Registers
 */
/**@{*/
#define sp r12 /**< Stack Pointer Pointer              */
#define bp r13 /**< Stack Base Pointer Register        */
#define pi pcr /**< Processing Identification Register */
/**@}*/

.global _do_syscall
.global _do_excp

.section .text

/*============================================================================*
 * k1b_redzone_alloc()                                                        *
 *============================================================================*/

/*
 * Allocates the red zone in the current stack frame.
 */
.macro redzone_alloc
	add $sp = $sp, -REDZONE_SIZE
.endm

/*============================================================================*
 * k1b_redzone_free()                                                         *
 *============================================================================*/

/*
 * Frees the red zone in the current stack frame.
 */
.macro redzone_free
	add $sp = $sp, REDZONE_SIZE
.endm


/*============================================================================*
 * k1b_context_save()                                                         *
 *============================================================================*/

/*
 * Saves the current execution context.
 */
.macro k1b_context_save dest

	/* Save GPRs. */
	sd K1B_CONTEXT_R0[\dest], $p0   /**< r0  + r1  */
	;;
	sd K1B_CONTEXT_R2[\dest], $p2   /**< r2  + r3  */
	;;
	sd K1B_CONTEXT_R4[\dest], $p4   /**< r4  + r5  */
	;;
	sd K1B_CONTEXT_R6[\dest], $p6   /**< r6  + r7  */
	;;
	sd K1B_CONTEXT_R8[\dest], $p8   /**< r8  + r9  */
	;;
	sd K1B_CONTEXT_R10[\dest], $p10 /**< r10 + r11 */
	;;
	sd K1B_CONTEXT_R12[\dest], $p12 /**< r12 + r13 */
	;;
	sd K1B_CONTEXT_R14[\dest], $p14 /**< r14 + r15 */
	;;
	sd K1B_CONTEXT_R16[\dest], $p16 /**< r16 + r17 */
	;;
	sd K1B_CONTEXT_R18[\dest], $p18 /**< r18 + r19 */
	;;
	sd K1B_CONTEXT_R20[\dest], $p20 /**< r20 + r21 */
	;;
	sd K1B_CONTEXT_R22[\dest], $p22 /**< r22 + r23 */
	;;
	sd K1B_CONTEXT_R24[\dest], $p24 /**< r24 + r25 */
	;;
	sd K1B_CONTEXT_R26[\dest], $p26 /**< r26 + r27 */
	;;
	sd K1B_CONTEXT_R28[\dest], $p28 /**< r28 + r29 */
	;;
	sd K1B_CONTEXT_R30[\dest], $p30 /**< r30 + r31 */
	;;
	sd K1B_CONTEXT_R32[\dest], $p32 /**< r32 + r33 */
	;;
	sd K1B_CONTEXT_R34[\dest], $p34 /**< r34 + r35 */
	;;
	sd K1B_CONTEXT_R36[\dest], $p36 /**< r36 + r37 */
	;;
	sd K1B_CONTEXT_R38[\dest], $p38 /**< r38 + r39 */
	;;
	sd K1B_CONTEXT_R40[\dest], $p40 /**< r40 + r41 */
	;;
	sd K1B_CONTEXT_R42[\dest], $p42 /**< r42 + r43 */
	;;
	sd K1B_CONTEXT_R44[\dest], $p44 /**< r44 + r45 */
	;;
	sd K1B_CONTEXT_R46[\dest], $p46 /**< r46 + r47 */
	;;
	sd K1B_CONTEXT_R48[\dest], $p48 /**< r48 + r49 */
	;;
	sd K1B_CONTEXT_R50[\dest], $p50 /**< r50 + r51 */
	;;
	sd K1B_CONTEXT_R52[\dest], $p52 /**< r52 + r53 */
	;;
	sd K1B_CONTEXT_R54[\dest], $p54 /**< r54 + r55 */
	;;
	sd K1B_CONTEXT_R56[\dest], $p56 /**< r56 + r57 */
	;;
	sd K1B_CONTEXT_R58[\dest], $p58 /**< r58 + r59 */
	;;
	sd K1B_CONTEXT_R60[\dest], $p60 /**< r60 + r61 */
	;;
	sd K1B_CONTEXT_R62[\dest], $p62 /**< r62 + r63 */
	;;

	/* Get SFRs*/
	get   $r2 = $spc
	;;
	get   $r3 = $sps
	;;
	get   $r4 = $ra
	;;
	get   $r5 = $cs
	;;
	getd  $p6 = $ps
	;;
	get   $r8 = $lc
	;;
	get   $r9 = $ls
	;;
	get   $r10 = $le
	;;

	/* Save SFRs */
	sw K1B_CONTEXT_SPC[\dest], $r2
	;;
	sw K1B_CONTEXT_SPS[\dest], $r3
	;;
	sw K1B_CONTEXT_RA[\dest],  $r4
	;;
	sw K1B_CONTEXT_CS[\dest],  $r5
	;;
	sd K1B_CONTEXT_PS[\dest],  $p6
	;;
	sw K1B_CONTEXT_LC[\dest],  $r8
	;;
	sw K1B_CONTEXT_LS[\dest],  $r9
	;;
	sw K1B_CONTEXT_LE[\dest],  $r10
.endm

/*============================================================================*
 * k1b_context_restore()                                                      *
 *============================================================================*/

/*
 * Restores an execution context.
 */
.macro k1b_context_restore src

	/*
	 * Restore SFRs. Note that $pc, $spc,
	 * $ps and $spc are restored by mOS.
	 */
	lw $r0  = K1B_CONTEXT_RA[\src]
	;;
	lw $r1  = K1B_CONTEXT_CS[\src]
	;;
	lw $r2  = K1B_CONTEXT_LC[\src]
	;;
	lw $r3  = K1B_CONTEXT_LS[\src]
	;;
	lw $r4 = K1B_CONTEXT_LE[\src]
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

	/* Save GPRs. */
	ld $p0  = K1B_CONTEXT_R0[\src]  /**< r0  + r1  */
	;;
	ld $p2  = K1B_CONTEXT_R2[\src]  /**< r2  + r3  */
	;;
	ld $p4  = K1B_CONTEXT_R4[\src]  /**< r4  + r5  */
	;;
	ld $p6  = K1B_CONTEXT_R6[\src]  /**< r6  + r7  */
	;;
	ld $p8  = K1B_CONTEXT_R8[\src]  /**< r8  + r9  */
	;;
	ld $p10 = K1B_CONTEXT_R10[\src] /**< r10 + r11 */
	;;
	ld $p12 = K1B_CONTEXT_R12[\src] /**< r12 + r13 */
	;;
	ld $p14 = K1B_CONTEXT_R14[\src] /**< r14 + r15 */
	;;
	ld $p16 = K1B_CONTEXT_R16[\src] /**< r16 + r17 */
	;;
	ld $p18 = K1B_CONTEXT_R18[\src] /**< r18 + r19 */
	;;
	ld $p20 = K1B_CONTEXT_R20[\src] /**< r20 + r21 */
	;;
	ld $p22 = K1B_CONTEXT_R22[\src] /**< r22 + r23 */
	;;
	ld $p24 = K1B_CONTEXT_R24[\src] /**< r24 + r25 */
	;;
	ld $p26 = K1B_CONTEXT_R26[\src] /**< r26 + r27 */
	;;
	ld $p28 = K1B_CONTEXT_R28[\src] /**< r28 + r29 */
	;;
	ld $p30 = K1B_CONTEXT_R30[\src] /**< r30 + r31 */
	;;
	ld $p32 = K1B_CONTEXT_R32[\src] /**< r32 + r33 */
	;;
	ld $p34 = K1B_CONTEXT_R34[\src] /**< r34 + r35 */
	;;
	ld $p36 = K1B_CONTEXT_R36[\src] /**< r36 + r37 */
	;;
	ld $p38 = K1B_CONTEXT_R38[\src] /**< r38 + r39 */
	;;
	ld $p40 = K1B_CONTEXT_R40[\src] /**< r40 + r41 */
	;;
	ld $p42 = K1B_CONTEXT_R42[\src] /**< r42 + r43 */
	;;
	ld $p44 = K1B_CONTEXT_R44[\src] /**< r44 + r45 */
	;;
	ld $p46 = K1B_CONTEXT_R46[\src] /**< r46 + r47 */
	;;
	ld $p48 = K1B_CONTEXT_R48[\src] /**< r48 + r49 */
	;;
	ld $p50 = K1B_CONTEXT_R50[\src] /**< r50 + r51 */
	;;
	ld $p52 = K1B_CONTEXT_R52[\src] /**< r52 + r53 */
	;;
	ld $p54 = K1B_CONTEXT_R54[\src] /**< r54 + r55 */
	;;
	ld $p56 = K1B_CONTEXT_R56[\src] /**< r56 + r57 */
	;;
	ld $p58 = K1B_CONTEXT_R58[\src] /**< r58 + r59 */
	;;
	ld $p60 = K1B_CONTEXT_R60[\src] /**< r60 + r61 */
	;;
	ld $p62 = K1B_CONTEXT_R62[\src] /**< r62 + r63 */
.endm


/*===========================================================================*
 * _do_syscall()                                                             *
 *===========================================================================*/

/*
 * System call hook.
 */
.align 8
_do_syscall:

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
	call do_syscall1
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

.macro _do_prologue

	/* Allocate a new stack frame. */
	add $sp, $sp, -16
	;;

	/* Save scratch registers (r0 and r1). */
	sd 8[$sp] = $p0
	;;

	/* Save return address and stack base pointer. */
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

	/* Save scratch registers (r0 and r1). */
	sd 8[$sp] = $p0
	;;

	/* Save return address and stack base pointer. */
	get $r0 = $ra
	;;
	copy $r1 = $bp
	;;
	sd 0[$sp] = $p0
	;;

	/* Update stack base pointer. */
	copy $bp = $sp
	;;


	/* Free stack frame. */
	add $sp, $sp, 16
	;;
.endm

/*
 * Gets the scoreboard of the underlying core.
 */
.macro _scoreboard_get s0 s1
	/* Get core ID. */
	get   \s1, $pi
	;;
	extfz \s1, \s1, 15, 11
	;;

	/* Get pointer to scoreboard. */
	make \s0 = _scoreboard_start
	;;
	sll  \s1, \s1, MOS_SCOREBOARD_PER_CPU_ELEMENT_SHIFT
	;;
	add  \s0, \s0, \s1
	;;
.endm

/*
 * Exception hook.
 */
.align 8
_do_excp:

	_do_prologue

	/*
	 * Save execution context
	 * in the current stack.
	 */
	add $sp, $sp, -K1B_CONTEXT_SIZE
	;;
	k1b_context_save $sp
	;;
	ld  $p2 = 8[$bp]            /* r0 + r1 */
	;;
	sd  K1B_CONTEXT_R0[$sp], $p0
	;;
	ld  $p2 = 0[$bp]            /* ra + bp */
	;;
	sw  K1B_CONTEXT_RA[$sp], $r2
	;;
	sw  K1B_CONTEXT_R13[$sp], $r3
	;;

	/*
	 * Save a reference to execution context
	 * into r1. The high-level exception handler
	 * dispatcher will need it.
	 */
	copy $r1 = $sp
	;;

	_scoreboard_get $r2 $r3

	/*
	 * Save exception information
	 * in the current stack.
	 */
	add  $sp, $sp, -K1B_EXCEPTION_SIZE
	;;
	lw  $r5 = MOS_VC_REG_PS[$r2]
	;;
	srl $r4 = $r5, 28                 /* Exception code. */
	;;
	sw  K1B_EXCEPTION_NUM[$sp], $r4
	;;
	lw  $r4 = MOS_VC_REG_EA[$r2]      /* Exception address. */
	;;
	sw K1B_EXCEPTION_EA[$sp], $r4
	;;
	lw  $r4 = MOS_VC_REG_SPC[$r2]     /* Instruction address. */
	;;
	sw K1B_EXCEPTION_SPC[$sp], $r4
	;;

	/*
	 * Save a reference to exception information
	 * into r0. The high-level exception handler
	 * dispatcher will need it.
	 */
	copy $r0 = $sp
	;;

	/*
	 * Call exception dispatcher.
	 */
	redzone_alloc
	;;
	call do_excp
	;;
	redzone_free
	;;

	/*
	 * Wipe out exception information
	 * from current stack.
	 */
	add $sp, $sp, K1B_EXCEPTION_SIZE
	;;

	/*
	 * Restore saved execution context.
	 */
	k1b_context_restore $sp
	;;
	add $sp, $sp, K1B_CONTEXT_SIZE
	;;

	/* Wipe out prologue. */
	add $sp, $sp, 16
	;;

	/* Restore exception context. */
	scall MOS_VC_RFE
	;;
	ret
	;;
