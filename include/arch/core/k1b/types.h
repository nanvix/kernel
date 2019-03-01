/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis <davidsondfgl@gmail.com>
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

#ifndef ARCH_CORE_OR1K_TYPES_H_
#define ARCH_CORE_OR1K_TYPES_H_

/**
 * @cond mor1kx
 */

#ifndef _ASM_FILE_
	#include <stdint.h>
#endif

	/**
	 * @name Bit-Length of Machine Types
	 */
	/**@{*/
	#define BYTE_BIT    8 /**< Byte.        */
	#define WORD_BIT   16 /**< Word.        */
	#define DWORD_BIT  32 /**< Double word. */
	#define QWORD_BIT  64 /**< Quad word.   */
	/**@}*/

	/**
	 * @name Byte-Length of Machine Types
	 */
	/**@{*/
	#define BYTE_SIZE  1 /**< Byte.        */
	#define WORD_SIZE  2 /**< Word.        */
	#define DWORD_SIZE 4 /**< Double word. */
	#define QWORD_SIZE 8 /**< Quad word.   */
	/**@}*/

#ifndef _ASM_FILE_

	/**
	 * @brief Virtual address.
	 */
	typedef uint32_t vaddr_t;

	/**
	 * @brief Physical address.
	 */
	typedef uint32_t paddr_t;

	/**
	 * @name Machine Types
	 */
	/**@{*/
	typedef uint8_t byte_t;   /**< Byte.        */
	typedef uint16_t word_t;  /**< Word.        */
	typedef uint32_t dword_t; /**< Double word. */
	/**@}*/

#endif

/**@}*/

#endif /* ARCH_CORE_OR1K_TYPES_H_ */
