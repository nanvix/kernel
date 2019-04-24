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
	#include <nanvix/thread.h>

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
	#define NR_SYSCALLS 9

	/**
	 * @name System Call Numbers
	 */
	/**@{*/
	#define NR__exit         1 /**< _exit()             */
	#define NR_write         2 /**< sys_write()         */
	#define NR_thread_get_id 3 /**< sys_thread_get_id() */
	#define NR_thread_create 4 /**< sys_thread_create() */
	#define NR_thread_exit   5 /**< sys_thread_exit()   */
	#define NR_thread_join   6 /**< sys_thread_join()   */
	#define NR_sleep         7 /**< sys_sleep()         */
	#define NR_wakeup        8 /**< sys_wakeup()        */
	/**@}*/

	/**
	 * @name System Calls
	 */
	/**@{*/
	EXTERN void sys_exit(int);
	EXTERN ssize_t sys_write(int, const char *, size_t);
	EXTERN int sys_thread_get_id(void);
	EXTERN int sys_thread_create(int *, void*(*)(void*), void *);
	EXTERN void sys_thread_exit(void *);
	EXTERN int sys_thread_join(int, void **);
	EXTERN int sys_sleep(void);
	EXTERN int sys_wakeup(int);
	/**@}*/

/**@}*/

#endif /* NANVIX_SYSCALL_H_ */

