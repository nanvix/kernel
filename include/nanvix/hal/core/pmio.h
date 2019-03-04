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

#ifndef NANVIX_HAL_CORE_PMIO_H_
#define NANVIX_HAL_CORE_PMIO_H_

	/* Cluster Interface Implementation */
	#include <nanvix/hal/cluster/_cluster.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Feature Checking */
	#ifndef CORE_SUPPORTS_PMIO
	#error "does this core support port-mapped i/o devices?"
	#endif

	#if (CORE_SUPPORTS_PMIO)

		/* Functions */
		#ifndef __output8_fn
		#error "output8() not defined?"
		#endif
		#ifndef __output8s_fn
		#error "output8s() not defined?"
		#endif
		#ifndef __iowait_fn
		#error "iowait() not defined?"
		#endif

	#endif

/*============================================================================*
 * Input/Output Interface                                                     *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-core-pmio Port-Mapped I/O
 * @ingroup kernel-hal-core
 *
 * @brief Port-Mapped I/O HAL Interface
 */
/**@{*/

	#include <nanvix/const.h>
	#include <stdint.h>

	/**
	 * @brief Writes 8 bits to an I/O port.
	 *
	 * @param port Number of the target port.
	 * @param bits Bits to write.
	 */
#if (CORE_SUPPORTS_PMIO)
	EXTERN void output8(uint16_t port, uint8_t bits);
#else
	static inline void output8(uint16_t port, uint8_t bits)
	{
		((void) port);
		((void) bits);
	}
#endif
	/**
	 * @brief Writes a 8-bit string to an I/O port.
	 *
	 * @param port Number of the target port.
	 * @param str  8-bit string to write.
	 * @param len  Length of the string.
	 */
#if (CORE_SUPPORTS_PMIO)
	EXTERN void output8s(uint16_t port, const uint8_t *str, size_t len);
#else
	static inline void output8s(uint16_t port, const uint8_t *str, size_t len)
	{
		((void) port);
		((void) str);
		((void) len);
	}
#endif

	/**
	 * @brief Waits for an operation in an I/O port to complete.
	 *
	 * @param port Number of the target port.
	 */
#if (CORE_SUPPORTS_PMIO)
	EXTERN void iowait(uint16_t port);
#else
	static inline void iowait(uint16_t port)
	{
		((void) port);
	}
#endif

/**@}*/

#endif /* NANVIX_HAL_IO_CLUSTER_H_ */
