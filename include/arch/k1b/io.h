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

#ifndef ARCH_K1B_JTAG_H_
#define ARCH_K1B_JTAG_H_

/**
 * @addtogroup k1b-io I/O
 * @ingroup k1b
 *
 * @brief Input/Output
 */
/**@{*/

	#include <nanvix/const.h>
	#include <nanvix/klib.h>
	#include <stdint.h>

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_outputb
	#define __hal_iowait
	/**@}*/

	/**
	 * @brief Size of JTAG buffer (in bytes).
	 */
	#define JTAG_BUFSIZE 32

	/**
	 * The hal_outputb() function is a dummy function. In the k1b
	 * architecture, there are not I/O ports.
	 */
	static inline void hal_outputb(uint16_t port, uint8_t byte)
	{
		UNUSED(port);
		UNUSED(byte);
	}

	/**
	 * The hal_iowait() function is a dummy function. In the k1b
	 * architecture, there are not I/O ports.
	 */
	static inline void hal_iowait(void)
	{
		noop();
	}

	/* Forward definitions. */
	EXTERN void hal_jtag_write(const uint8_t *, size_t);

/**@}*/

#endif /* ARCH_K1B_JTAG_H_ */
