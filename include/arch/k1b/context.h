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

#ifndef ARCH_K1B_CONTEXT_H_
#define ARCH_K1B_CONTEXT_H_

/**
 * @addtogroup k1b-context Context
 * @ingroup k1b
 *
 * @brief Execution Context
 */
/**@{*/

/**
 * @cond k1b
 */

	/**
	 * @name Procided Interface
	 */
	/**@{*/
	#define __context_struct /**< Exection Context Structure */
	/**@}*/

/**@endcond*/

	/**
	 * @brief Execution context size (in bytes).
	 */
	#define K1B_CONTEXT_SIZE 308

	/**
	 * @name Offsets to the Context Structure
	 */
	/**@{*/
	#define K1B_CONTEXT_R0      0 /**< General Purpose Register  0       */
	#define K1B_CONTEXT_R1      4 /**< General Purpose Register  1       */
	#define K1B_CONTEXT_R2      8 /**< General Purpose Register  2       */
	#define K1B_CONTEXT_R3     12 /**< General Purpose Register  3       */
	#define K1B_CONTEXT_R4     16 /**< General Purpose Register  4       */
	#define K1B_CONTEXT_R5     20 /**< General Purpose Register  5       */
	#define K1B_CONTEXT_R6     24 /**< General Purpose Register  6       */
	#define K1B_CONTEXT_R7     28 /**< General Purpose Register  7       */
	#define K1B_CONTEXT_R8     32 /**< General Purpose Register  8       */
	#define K1B_CONTEXT_R9     36 /**< General Purpose Register  9       */
	#define K1B_CONTEXT_R10    40 /**< General Purpose Register 10       */
	#define K1B_CONTEXT_R11    44 /**< General Purpose Register 11       */
	#define K1B_CONTEXT_R12    48 /**< General Purpose Register 12       */
	#define K1B_CONTEXT_R13    52 /**< General Purpose Register 13       */
	#define K1B_CONTEXT_R14    56 /**< General Purpose Register 14       */
	#define K1B_CONTEXT_R15    60 /**< General Purpose Register 15       */
	#define K1B_CONTEXT_R16    64 /**< General Purpose Register 16       */
	#define K1B_CONTEXT_R17    68 /**< General Purpose Register 17       */
	#define K1B_CONTEXT_R18    72 /**< General Purpose Register 18       */
	#define K1B_CONTEXT_R19    76 /**< General Purpose Register 19       */
	#define K1B_CONTEXT_R20    80 /**< General Purpose Register 20       */
	#define K1B_CONTEXT_R21    84 /**< General Purpose Register 21       */
	#define K1B_CONTEXT_R22    88 /**< General Purpose Register 22       */
	#define K1B_CONTEXT_R23    92 /**< General Purpose Register 23       */
	#define K1B_CONTEXT_R24    96 /**< General Purpose Register 24       */
	#define K1B_CONTEXT_R25   100 /**< General Purpose Register 25       */
	#define K1B_CONTEXT_R26   104 /**< General Purpose Register 26       */
	#define K1B_CONTEXT_R27   108 /**< General Purpose Register 27       */
	#define K1B_CONTEXT_R28   112 /**< General Purpose Register 28       */
	#define K1B_CONTEXT_R29   116 /**< General Purpose Register 29       */
	#define K1B_CONTEXT_R30   120 /**< General Purpose Register 30       */
	#define K1B_CONTEXT_R31   124 /**< General Purpose Register 31       */
	#define K1B_CONTEXT_R32   128 /**< General Purpose Register 32       */
	#define K1B_CONTEXT_R33   132 /**< General Purpose Register 33       */
	#define K1B_CONTEXT_R34   136 /**< General Purpose Register 34       */
	#define K1B_CONTEXT_R35   140 /**< General Purpose Register 35       */
	#define K1B_CONTEXT_R36   144 /**< General Purpose Register 36       */
	#define K1B_CONTEXT_R37   148 /**< General Purpose Register 37       */
	#define K1B_CONTEXT_R38   152 /**< General Purpose Register 38       */
	#define K1B_CONTEXT_R39   156 /**< General Purpose Register 39       */
	#define K1B_CONTEXT_R40   160 /**< General Purpose Register 40       */
	#define K1B_CONTEXT_R41   164 /**< General Purpose Register 41       */
	#define K1B_CONTEXT_R42   168 /**< General Purpose Register 42       */
	#define K1B_CONTEXT_R43   172 /**< General Purpose Register 43       */
	#define K1B_CONTEXT_R44   176 /**< General Purpose Register 44       */
	#define K1B_CONTEXT_R45   180 /**< General Purpose Register 45       */
	#define K1B_CONTEXT_R46   184 /**< General Purpose Register 46       */
	#define K1B_CONTEXT_R47   188 /**< General Purpose Register 47       */
	#define K1B_CONTEXT_R48   192 /**< General Purpose Register 48       */
	#define K1B_CONTEXT_R49   196 /**< General Purpose Register 49       */
	#define K1B_CONTEXT_R50   200 /**< General Purpose Register 50       */
	#define K1B_CONTEXT_R51   204 /**< General Purpose Register 51       */
	#define K1B_CONTEXT_R52   208 /**< General Purpose Register 52       */
	#define K1B_CONTEXT_R53   212 /**< General Purpose Register 53       */
	#define K1B_CONTEXT_R54   216 /**< General Purpose Register 54       */
	#define K1B_CONTEXT_R55   220 /**< General Purpose Register 55       */
	#define K1B_CONTEXT_R56   224 /**< General Purpose Register 56       */
	#define K1B_CONTEXT_R57   228 /**< General Purpose Register 57       */
	#define K1B_CONTEXT_R58   232 /**< General Purpose Register 58       */
	#define K1B_CONTEXT_R59   236 /**< General Purpose Register 59       */
	#define K1B_CONTEXT_R60   240 /**< General Purpose Register 60       */
	#define K1B_CONTEXT_R61   244 /**< General Purpose Register 61       */
	#define K1B_CONTEXT_R62   248 /**< General Purpose Register 62       */
	#define K1B_CONTEXT_R63   252 /**< General Purpose Register 63       */
	#define K1B_CONTEXT_SPC   256 /**< Shadow Program Counter Register   */
	#define K1B_CONTEXT_RA    260 /**< Return Address Register           */
	#define K1B_CONTEXT_CS    264 /**< Compute Status Register           */
	#define K1B_CONTEXT_SSP   268 /**< Compute Status Register           */
	#define K1B_CONTEXT_SSSP  272 /**< Compute Status Register           */
	#define K1B_CONTEXT_SSSSP 276 /**< Compute Status Register           */
	#define K1B_CONTEXT_LC    280 /**< Loop Count Register               */
	#define K1B_CONTEXT_LS    284 /**< Loop Start Register               */
	#define K1B_CONTEXT_LE    288 /**< Loop Exit Register                */
	#define K1B_CONTEXT_PS    292 /**< Processing Status Register        */
	#define K1B_CONTEXT_SPS   300 /**< Shadow Processing Status Register */
	/**@}*/

#ifndef _ASM_FILE_

	#include <stdint.h>

/**
 * @cond k1b
 */

	/**
	 * Saved execution context upon interrupts and exceptions.
	 */
	struct context
	{
		uint32_t  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7; /**< General Purpose Registers  0 to  7 */
		uint32_t  r8,  r9, r10, r11, r12, r13, r14, r15; /**< General Purpose Registers  8 to 15 */
		uint32_t r16, r17, r18, r19, r20, r21, r22, r23; /**< General Purpose Registers 16 to 23 */
		uint32_t r24, r25, r26, r27, r28, r29, r30, r31; /**< General Purpose Registers 24 to 31 */
		uint32_t r32, r33, r34, r35, r36, r37, r38, r39; /**< General Purpose Registers 32 to 39 */
		uint32_t r40, r41, r42, r43, r44, r45, r46, r47; /**< General Purpose Registers 40 to 47 */
		uint32_t r48, r49, r50, r51, r52, r53, r54, r55; /**< General Purpose Registers 48 to 55 */
		uint32_t r56, r57, r58, r59, r60, r61, r62, r63; /**< General Purpose Registers 56 to 63 */
		uint32_t spc;                                    /**< Shadow Program Counter Register    */
		uint32_t ra;                                     /**< Return Address Register            */
		uint32_t cs;                                     /**< Compute Status Register            */
		uint32_t ssp, sssp, ssssp;                       /**< Stack Pointer Registers            */
		uint32_t lc;                                     /**< Loop Count Register                */
		uint32_t ls;                                     /**< Loop Start Register                */
		uint32_t le;                                     /**< Loop Exit Register                 */
		uint64_t ps;                                     /**< Processing Status Register         */
		uint64_t sps;                                    /**< Shadow Processing Status Register  */
	} __attribute__((packed));

/**@endcond*/

/**@}*/

#else

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

		/* Wipeout allocated memory. */
		add $r12, $r12, K1B_CONTEXT_SIZE
		;;
	.endm

#endif /* _ASM_FILE_ */

#endif /* ARCH_K1B_CONTEXT_H_ */
