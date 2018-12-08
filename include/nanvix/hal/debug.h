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

#ifndef NANVIX_HAL_DEBUG_H_
#define NANVIX_HAL_DEBUG_H_

/**
 * @addtogroup kernel-hal-debug Debug
 * @ingroup kernel-hal
 */
/**@{*/

	#include <nanvix/const.h>
	#include <nanvix/hal/target.h>

	#ifndef __hal_stdout_init
	#error "hal_stdout_init() not defined?"
	#endif

	#ifndef __hal_stdout_write
	#error "hal_stdout_write() not defined?"
	#endif

	/**
	 * @brief Initializes the standard output device.
	 */
	EXTERN void hal_stdout_init(void);

	/*
	 * @brief Writes to the standard output device.
	 *
	 * @param buf Target buffer.
	 * @param n   Number of bytes to write.
	 */
	EXTERN void hal_stdout_write(const char *buf, size_t n);

/**@}*/

#endif /* NANVIX_HAL_DEBUG_H_ */
