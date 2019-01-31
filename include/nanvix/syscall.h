/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2017 Davidson Francis     <davidsondfgl@gmail.com>
 *              2016-2016 Subhra S. Sarkar     <rurtle.coder@gmail.com>
 *              2017-2017 Romane Gallier       <romanegallier@gmail.com>
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

#ifndef NANVIX_SYSCALL_H_
#define NANVIX_SYSCALL_H_

	#include <nanvix/const.h>
	#include <sys/types.h>

/**
 * @addtogroup kernel-syscalls System Calls
 * @ingroup kernel
 */
/**@{*/

	/**
	 * @brief Number of system calls.
	 *
	 * @note This should be set to the highest system call number.
	 */
	#define NR_SYSCALLS 6

	/**
	 * @name Magic Numbers for nosyscall()
	 */
	/**@{*/
	#define NOSYSCALL_MAGIC_ARG 0x001f00d  /**< Magic number for argument.     */
	#define NOSYSCALL_MAGIC_RET 0xdeadca11 /**< Magic number for return value. */
	/**@}*/

	/**
	 * @name System Call Numbers
	 */
	/**@{*/
	#define NR_nosyscall     0 /**< nosyscall()     */
	#define NR_cache_flush   1 /**< cache_flush()   */
	#define NR__exit         2 /**< _exit()         */
	#define NR_write         3 /**< write()         */
	#define NR_thread_get_id 4 /**< thread_get_id() */
	#define NR_thread_create 5 /**< thread_create() */
	/**@}*/

	/**
	 * @name System Calls
	 */
	/**@{*/
	EXTERN int sys_nosyscall(unsigned);
	EXTERN int sys_cache_flush(void);
	EXTERN void sys_exit(int);
	EXTERN ssize_t sys_write(int, const char *, size_t);
	EXTERN int sys_thread_get_id(void);
	EXTERN int sys_thread_create(int *, void*(*)(void*), void *);
	/**@}*/

/**@}*/
	
#endif /* NANVIX_SYSCALL_H_ */

