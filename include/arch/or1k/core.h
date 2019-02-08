/*
 * MIT License
 *
 * Copyright(c) 2018-2018 Davidson Francis <davidsondfgl@gmail.com>
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

#ifndef ARCH_OR1K_CORE_H_
#define ARCH_OR1K_CORE_H_

	#include <arch/or1k/regs.h>

/**
 * @addtogroup or1k-core Core
 * @ingroup or1k
 *
 * @brief or1k Core
 */
/**@{*/

	/**
	 * @name Size of Machine Types
	 */
	/**@{*/
	#define BYTE_SIZE  1 /**< Byte.        */
	#define WORD_SIZE  2 /**< Word.        */
	#define DWORD_SIZE 4 /**< Double word. */
	#define QWORD_SIZE 8 /**< Quad word.   */
	/**@}*/

	/**
	 * @name Bit-Length of Machine Types
	 */
	/**@{*/
	#define BYTE_BIT    8 /**< Byte.        */
	#define WORD_BIT   16 /**< Word.        */
	#define DWORD_BIT  32 /**< Double word. */
	#define QWORD_BIT  64 /**< Quad word.   */
	/**@}*/

#ifndef _ASM_FILE_

	#include <nanvix/const.h>
	#include <stdint.h>

	/**
	 * @name Machine Types
	 */
	/**@{*/
	typedef uint8_t byte_t;   /**< Byte.        */
	typedef uint16_t word_t;  /**< Word.        */
	typedef uint32_t dword_t; /**< Double word. */
	/**@}*/

	/**
	 * @brief Gets the ID of the core.
	 *
	 * The or1k_core_get_id() returns the ID of the underlying core.
	 *
	 * @returns The ID of the underlying core.
	 */
	static inline int or1k_core_get_id(void)
	{
		return (0);
	}

	/**
	 * @brief Halts the processor.
	 *
	 * The or1k_hlt() function stops instruction execution in the the
	 * underlying core and places it in a halt state. An enabled
	 * hardware interrupt, NMI, or a reset resumes execution.
	 */
	static inline void or1k_hlt(void)
	{
	}

	/**
	 * @brief Reads from an specified Special-Purpose register.
	 *
	 * @param reg Register to be read.
	 *
	 * @returns Returns the value of the specified SPR.
	 */
	static inline unsigned or1k_mfspr(unsigned reg)
	{
		register unsigned ret
			__asm__("r3") = (unsigned) 0;
		register unsigned r5
			__asm__("r5") = (unsigned) reg;

		__asm__ __volatile__
		(
			"l.mfspr r3, r5, 0"
			: "=r" (ret)
			: "r" (r5)
		);
		return (ret);
	}

	/**
	 * @brief Writes to an specified Special-Purpose register.
	 *
	 * @param reg Register to be written.
	 * @param value Value to be written.
	 */
	static inline void or1k_mtspr(unsigned reg, unsigned value)
	{
		register unsigned r3
			__asm__("r3") = (unsigned) reg;
		register unsigned r5
			__asm__("r5") = (unsigned) value;

		__asm__ __volatile__
		(
			"l.mtspr r3, r5, 0\n"
			:
			: "r" (r3), "r" (r5)
		);
	}

/**@}*/

/*============================================================================*
 *                              Exported Interface                            *
 *============================================================================*/

/**
 * @cond or1k
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __core_get_id   /**< core_get_id()   */
	#define __core_halt     /**< core_halt()     */
	#define __core_shutdown /**< core_shutdown() */
	/**@}*/

/**
 * @addtogroup kernel-hal-core Core
 * @ingroup kernel-hal-cpu
 *
 * @brief Core Interface
 */
/**@{*/

	/**
	 * @see or1k_core_get_id().
	 */
	static inline int core_get_id(void)
	{
		return (or1k_core_get_id());
	}

	/**
	 * @see or1k_hlt().
	 */
	static inline void core_halt(void)
	{
		or1k_hlt();
	}

/**@}*/

/**@endcond*/

#endif /* _ASM_FILE_ */

#endif	/* ARCH_OR1K_CORE_H_ */
