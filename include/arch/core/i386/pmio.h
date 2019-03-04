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

#ifndef ARCH_CORE_I386_PMIO_H_
#define ARCH_CORE_I386_PMIO_H_

/**
 * @addtogroup i386-core-io Port-Mapped I/O
 * @ingroup i386-core
 *
 * @brief Port-Mapped I/O
 */
/**@{*/

	#include <sys/types.h>
	#include <stdint.h>

	/**
	 * @brief Writes 8 bits to an I/O port.
	 *
	 * @param port Number of the target port.
	 * @param bits Bits to write.
	 */
	static inline void i386_output8(uint16_t port, uint8_t bits)
	{
		__asm__ __volatile__ ("outb %0, %1" : : "a"(bits), "Nd"(port));
	}

	/**
	 * @brief Waits for an operation in an I/O port to complete.
	 */
	static inline void i386_iowait(void)
	{
		__asm__ __volatile__("outb %%al, $0x80" : : "a"(0));
	}

	/**
	 * @brief Writes a 8-bit string to an I/O port.
	 *
	 * @param port Number of the target port.
	 * @param str  8-bit string to write.
	 * @param len  Length of the string.
	 *
	 * @todo This can be platform-independent.
	 */
	static inline void i386_output8s(uint16_t port, const uint8_t *str, size_t len)
	{
		for (size_t i = 0; i < len; i++)
		{
			i386_output8(port, str[i]);
			i386_iowait();
		}
	}

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond i386
 */

	/**
	 * @name Provided Functions
	 */
	/**@{*/
	#define __output8_fn  /**< i386_output8()  */
	#define __output8s_fn /**< i386_output8s() */
	#define __iowait_fn   /**< iowait()        */
	/**@}*/

	/**
	 * @see i386_output8().
	 */
	static inline void output8(uint16_t port, uint8_t bits)
	{
		i386_output8(port, bits);
	}

	/**
	 * @see i386_output8s().
	 */
	static inline void output8s(uint16_t port, const uint8_t *str, size_t len)
	{
		i386_output8s(port, str, len);
	}

	/**
	 * @see i386_iowait().
	 */
	static inline void iowait(uint16_t port)
	{
		((void) port);

		i386_iowait();
	}

/**@endcond*/

#endif /* ARCH_CORE_I386_PMIO_H_ */

