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

#ifndef ARCH_I386_IO_H_
#define ARCH_I386_IO_H_

/**
 * @addtogroup i386-io I/O
 * @ingroup i386
 *
 * @brief Input/Output
 */
/**@{*/

	#include <stdint.h>

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_outputb
	#define __hal_iowait
	/**@}*/

	/**
	 * @brief Writes a byte to an I/O port.
	 *
	 * The i386_outb() function writes @p byte to I/o port whose number is
	 * @p port.
	 *
	 * @param port Number of the target port.
	 * @param byte Byte to write.
	 */
	static inline void i386_outb(uint16_t port, uint8_t byte)
	{
		__asm__ __volatile__ ("outb %0, %1" : : "a"(byte), "Nd"(port));
	}

	/**
	 * @see i386_outb()
	 *
	 * @cond i386
	 */
	static inline void hal_outputb(uint16_t port, uint8_t byte)
	{
		i386_outb(port, byte);
	}
	/**@endcond*/

	/**
	 * @brief Waits an operation in an I/O port to complete.
	 *
	 * The i386_iowait() function forces a delay, so that an on-going
	 * operation in an I/O port completes.
	 */
	static inline void i386_iowait(void)
	{
		__asm__ __volatile__("outb %%al, $0x80" : : "a"(0));
	}

	/**
	 * @see i386_iowait()
	 *
	 * @cond i386
	 */
	static inline void hal_iowait(void)
	{
		i386_iowait();
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_I386_IO_H_ */

