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

#include <arch/k1b/regs.h>
#include <target/kalray/mppa256.h>

.global _k1b_core_reset

.section .text

/*===========================================================================*
 * k1b_core_reset()                                                          *
 *===========================================================================*/

/*
 * Resets the underlying core.
 */
.align 8
_k1b_core_reset:
	/* Get core id. */
	get   $r2, $pi
	;;
	extfz $r1, $r2, 15, 11
	;;

	/* Setup kernel stack. */
	mulwdl $r0 = $r1, K1B_KSTACK_SIZE
	;;
	make   $r1 = MPPA256_KSTACK_BASE_VIRT - 8
	;;
	sbf    $sp = $r0, $r1
	;;

	/* Restart core. */
	redzone_alloc
	;;
	call k1b_slave_setup
	;;

	/* Never gets here. */
	k1b_core_reset.halt:
		goto k1b_core_reset.halt
		;;
