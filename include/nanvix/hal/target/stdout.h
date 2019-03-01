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

#ifndef HAL_TARGET_STDOUT_H_
#define HAL_TARGET_STDOUT_H_

	/* Target Interface Implementation */
	#include <nanvix/hal/target/_target.h>

	#include <nanvix/const.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Feature Checking */
	#ifndef TARGET_HAS_STDOUT
	#error "does this target feature a standard output device?"
	#endif

	/* Has Stadard Output Device */
	#if (TARGET_HAS_STDOUT)

		/* Functions */
		#ifndef __hal_stdout_init_fn
		#error "hal_stdout_init() not defined?"
		#endif
		#ifndef __hal_stdout_write_fn
		#error "hal_stdout_write() not defined?"
		#endif

	#endif

/*============================================================================*
 * Provided Interface                                                         *
 *============================================================================*/

/**
 * @defgroup kernel-hal-target-stdout Standard Output
 * @ingroup kernel-hal-target
 *
 * @brief Target Stadard Output HAL Interface
 */
/**@{*/

#if (TARGET_HAS_STDOUT)
	EXTERN void hal_stdout_init(void);
#else
	static inline void hal_stdout_init(void)
	{
	}
#endif

#if (TARGET_HAS_STDOUT)
	EXTERN void hal_stdout_write(const char *buf, size_t n);
#else
	static inline void hal_stdout_write(const char *buf, size_t n)
	{
		((void) buf);
		((void) n);
	}
#endif

/**@}*/

#endif /* HAL_TARGET_STDOUT_H_ */

