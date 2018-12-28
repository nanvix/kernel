/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_OR1K_EXCEPTION_H_
#define ARCH_OR1K_EXCEPTION_H_
/**
 * @addtogroup or1k-exception Exception
 * @ingroup or1k
 *
 * @brief Exceptions
 */
/**@{*/

	/**
	 * @brief Shadow registers support, not all architectures have,
	 * so must ensure that the underlying architecture have, to
	 * properly save the registers.
	 */
#define OR1K_HAVE_SHADOW_GPRS 

	/**
	 * @brief Temporary registers used while inside exception/interrupt
	 * handler.
	 */
#ifdef OR1K_HAVE_SHADOW_GPRS
	#define OR1K_EXCEPTION_STORE_GPR2  l.mtspr r0, r2, OR1K_SPR_SHADOW_GPR(2)
	#define OR1K_EXCEPTION_LOAD_GPR2   l.mfspr r2, r0, OR1K_SPR_SHADOW_GPR(2)
	#define OR1K_EXCEPTION_STORE_GPR3  l.mtspr r0, r3, OR1K_SPR_SHADOW_GPR(3)
	#define OR1K_EXCEPTION_LOAD_GPR3   l.mfspr r3, r0, OR1K_SPR_SHADOW_GPR(3)
	#define OR1K_EXCEPTION_STORE_GPR4  l.mtspr r0, r4, OR1K_SPR_SHADOW_GPR(4)
	#define OR1K_EXCEPTION_LOAD_GPR4   l.mfspr r4, r0, OR1K_SPR_SHADOW_GPR(4)
	#define OR1K_EXCEPTION_STORE_GPR5  l.mtspr r0, r5, OR1K_SPR_SHADOW_GPR(5)
	#define OR1K_EXCEPTION_LOAD_GPR5   l.mfspr r5, r0, OR1K_SPR_SHADOW_GPR(5)
	#define OR1K_EXCEPTION_STORE_GPR6  l.mtspr r0, r6, OR1K_SPR_SHADOW_GPR(6)
	#define OR1K_EXCEPTION_LOAD_GPR6   l.mfspr r6, r0, OR1K_SPR_SHADOW_GPR(6)
#else
	#define OR1K_EXCEPTION_STORE_GPR2  l.sw 0x64(r0), r2
	#define OR1K_EXCEPTION_LOAD_GPR2   l.lwz r2, 0x64(r0)
	#define OR1K_EXCEPTION_STORE_GPR3  l.sw  0x68(r0), r3
	#define OR1K_EXCEPTION_LOAD_GPR3   l.lwz r3, 0x68(r0)
	#define OR1K_EXCEPTION_STORE_GPR4  l.sw  0x6c(r0), r4
	#define OR1K_EXCEPTION_LOAD_GPR4   l.lwz r4, 0x6c(r0)
	#define OR1K_EXCEPTION_STORE_GPR5  l.sw  0x70(r0), r5
	#define OR1K_EXCEPTION_LOAD_GPR5   l.lwz r5, 0x70(r0)
	#define OR1K_EXCEPTION_STORE_GPR6  l.sw  0x74(r0), r6
	#define OR1K_EXCEPTION_LOAD_GPR6   l.lwz r6, 0x74(r0)
#endif

	/**
	 * @name Provided Interface
	 *
	 * @cond or1k
	 */
	/**@{*/
	#define __exception_struct          /**< @ref exception              */
	#define __hal_exception_set_handler /**< hal_exception_set_handler() */
	/**@}*/
	/**@endcond*/

/**@endcond*/

#ifndef _ASM_FILE_
#endif /* _ASM_FILE_ */

/**@}*/

#endif /* ARCH_OR1K_EXCEPTION_H_ */
