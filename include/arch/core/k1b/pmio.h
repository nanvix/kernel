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

#ifndef CORE_K1B_PMIO_H_
#define CORE_K1B_PMIO_H_

	/* Cluster Interface Implementation */
	#include <arch/cluster/k1b/_k1b.h>

/**
 * @addtogroup k1b-core-io Port-Mapped I/O
 * @ingroup k1b-core
 *
 * @brief Port-Mapped I/O
 */
/**@{*/

	#include <stdint.h>

	/**
	 * @brief Writes 8 bits to an I/O port.
	 *
	 * @param port Number of the target port.
	 * @param bits Bits to write.
	 */
	static inline void k1b_output8(uint16_t port, uint8_t byte)
	{
		__k1_club_syscall2(port, (unsigned) &byte, 1);
	}

	/**
	 * @brief Writes a 8-bit string to an I/O port.
	 *
	 * @param port Number of the target port.
	 * @param str  8-bit string to write.
	 * @param len  Length of the string.
	 */
	static inline void k1b_output8s(uint16_t port, const uint8_t *str, size_t len)
	{
		/* Nothing to do. */
		if (len <= 0)
			return;

		__k1_club_syscall2(port, (unsigned) str, len);
	}

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond k1b
 */

	/**
	 * @name Provided Functions
	 */
	/**@{*/
	#define __output8_fn  /**< k1b_output8()  */
	#define __output8s_fn /**< k1b_output8s() */
	#define __iowait_fn   /**< iowait()       */
	/**@}*/

	/**
	 * @see k1b_output8().
	 */
	static inline void output8(uint16_t port, uint8_t bits)
	{
		k1b_output8(port, bits);
	}

	/**
	 * @see k1b_output8s().
	 */
	static inline void output8s(uint16_t port, const uint8_t *str, size_t len)
	{
		k1b_output8s(port, str, len);
	}

	/**
	 * @see k1b_iowait().
	 */
	static inline void iowait(uint16_t port)
	{
		((void) port);
	}

/**@endcond*/

#endif /* CORE_K1B_PMIO_H_ */
