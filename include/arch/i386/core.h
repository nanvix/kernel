/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef ARCH_I386_CORE_H_
#define ARCH_I386_CORE_H_

/**
 * @addtogroup i386-core Core
 * @ingroup i386
 *
 * @brief Interface for dealing with cores.
 */
/**@{*/

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_iowait
	#define __hal_outputb
	#define __hal_processor_halt
	/**@}*/

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

	/**
	 * @brief Number of cores in the i386 architecture.
	 */
	#define I386_NUM_CORES 1

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
	 * @brief Halts the processor.
	 *
	 * Stops instruction execution and places the processor in a halt
	 * state. An enabled interrupt, NMI, or a reset will resume
	 * execution. If an interrupt (including NMI) is used to resume
	 * execution after HLT, the saved CS:IP (or CS:EIP) value points
	 * to the instruction following HLT.
	 */
	static inline void hlt(void)
	{
		__asm__ __volatile__("hlt");
	}

	/**
	 * @see hlt()
	 */
	static inline void hal_processor_halt(void)
	{
		hlt();
	}

	/**
	 * @brief Writes a byte to an I/O port.
	 *
	 * The outb() function writes @p byte to I/o port whose number is
	 * @p port.
	 *
	 * @param port Number of the target port.
	 * @param byte Byte to write.
	 */
	static inline void outb(uint16_t port, uint8_t byte)
	{
		__asm__ __volatile__ ("outb %0, %1" : : "a"(byte), "Nd"(port));
	}

	/**
	 * @see outb()
	 */
	static inline void hal_outputb(uint16_t port, uint8_t byte)
	{
		outb(port, byte);
	}

	/**
	 * @brief Waits an operation in an I/O port to complete.
	 *
	 * The iowait() function forces a delay, so that an on-going
	 * operation in an I/O port completes.
	 */
	static inline void iowait(void)
	{
		__asm__ __volatile__("outb %%al, $0x80" : : "a"(0));
	}

	/**
	 * @see iowait()
	 */
	static inline void hal_iowait(void)
	{
		iowait();
	}

#endif /* _ASM_FILE_ */

/**@}*/

#endif /* ARCH_I386_CORE_H_ */
