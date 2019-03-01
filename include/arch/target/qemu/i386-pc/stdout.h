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

#ifndef TARGET_QEMU_I386_PC_STDOUT_H_
#define TARGET_QEMU_I386_PC_STDOUT_H_

	/* Target Interface Implementation */
	#include <arch/target/qemu/i386-pc/_i386-pc.h>

/**
 * @defgroup target-i386-pc-stdout Standard Output
 * @ingroup target-i386-pc
 */
/**@{*/

	#include <driver/console.h>
	#include <sys/types.h>

	/**
	 * @see console_init()
	 */
	static inline void qemu_i386pc_stdout_init(void)
	{
		console_init();
	}

	/**
	 * @see console_write()
	 */
	static inline void qemu_i386pc_stdout_write(const char *buf, size_t n)
	{
		console_write(buf, n);
	}

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond i386
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_stdout_init_fn  /**< hal_stdout_init()  */
	#define __hal_stdout_write_fn /**< hal_stdout_write() */
	/**@}*/

	/**
	 * @see qemu_i386pc_stdout_init().
	 */
	static inline void hal_stdout_init(void)
	{
		qemu_i386pc_stdout_init();
	}

	/**
	 * @see qemu_i386pc_stdout_write().
	 */
	static inline void hal_stdout_write(const char *buf, size_t n)
	{
		qemu_i386pc_stdout_write(buf, n);
	}

/**@endcond*/

#endif /* TARGET_QEMU_I386_PC_STDOUT_H_ */
