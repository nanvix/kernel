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

#ifndef TARGET_KALRAY_MPPA256_STDOUT_H_
#define TARGET_KALRAY_MPPA256_STDOUT_H_

	/* Target Interface Implementation */
	#include <arch/target/kalray/mppa256/_mppa256.h>

/**
 * @defgroup target-kalray-mppa256-stdout Standard Output
 * @ingroup target-kalray-mppa256-cluster
 */
/**@{*/

	#include <driver/jtag.h>
	#include <sys/types.h>

	/**
	 * @see jtag_init().
	 */
	static inline void mppa256_stdout_init(void)
	{
		jtag_init();
	}

	/**
	 * @see jtag_write().
	 */
	static inline void mppa256_stdout_write(const char *buf, size_t n)
	{
		jtag_write(buf, n);
	}

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond mppa256
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_stdout_init_fn  /**< hal_stdout_init()  */
	#define __hal_stdout_write_fn /**< hal_stdout_write() */
	/**@}*/

	/**
	 * @see mppa256_stdout_init().
	 */
	static inline void hal_stdout_init(void)
	{
		mppa256_stdout_init();
	}

	/**
	 * @see mppa256_stdout_write().
	 */
	static inline void hal_stdout_write(const char *buf, size_t n)
	{
		mppa256_stdout_write(buf, n);
	}

/**@endcond*/

#endif /* TARGET_KALRAY_MPPA256_STDOUT_H_ */
