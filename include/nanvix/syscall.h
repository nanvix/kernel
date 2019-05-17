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
	#include <nanvix/signal.h>

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
	#define NR_SYSCALLS 19

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
	#define NR_shutdown      9 /**< sys_shutdown()      */
	#define NR_perf_query   10 /**< sys_perf_query()    */
	#define NR_perf_start   11 /**< sys_perf_start()    */
	#define NR_perf_stop    12 /**< sys_perf_stop()     */
	#define NR_perf_read    13 /**< sys_perf_read()     */
	#define NR_sigclt       14 /**< sys_perf_read()     */
	#define NR_alarm        15 /**< sys_perf_read()     */
	#define NR_sigsend      16 /**< sys_perf_read()     */
	#define NR_sigwait      17 /**< sys_perf_read()     */
	#define NR_sigreturn    18 /**< sys_perf_read()     */
	/**@}*/

/*============================================================================*
 *                            Thread system syscalls                          *
 *============================================================================*/

	EXTERN void sys_exit(int);
	EXTERN ssize_t sys_write(int, const char *, size_t);
	EXTERN int sys_thread_get_id(void);
	EXTERN int sys_thread_create(int *, void*(*)(void*), void *);
	EXTERN void sys_thread_exit(void *);
	EXTERN int sys_thread_join(int, void **);
	EXTERN int sys_sleep(void);
	EXTERN int sys_wakeup(int);

	/**
	 * @brief Shutdowns the kernel.
	 *
	 * @returns Upon successful completion, this function does not
	 * return.Upon failure, a negative error code is returned instead.
	 */
	EXTERN int sys_shutdown(void);

/*============================================================================*
 *                              Perf System syscalls                          *
 *============================================================================*/

	/**
	 * @brief Queries a performance event.
	 *
	 * @param event Target event.
	 *
	 * @returns Upon successful completion, either zero or non zero is
	 * returned, indicating whether or not the queried performance event
	 * may be monitored in the underlying core. Upon failure, a negative
	 * error code is returned instead.
	 */
	EXTERN int sys_perf_query(int event);

	/**
	 * @brief Starts a performance monitor.
	 *
	 * @param perf  Target performance monitor.
	 * @param event Target event to watch.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int sys_perf_start(int perf, int event);

	/**
	 * @brief Stops a performance monitor.
	 *
	 * @param perf Target performance monitor.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int sys_perf_stop(int perf);

	/**
	 * @brief Reads a PM register.
	 *
	 * @param perf Target performance monitor.
	 *
	 * @returns Upon successful completion, the value of the target
	 * performance monitor. Upon failure, a negative error code,
	 * converted to uint64_t is returned instead.
	 */
	EXTERN uint64_t sys_perf_read(int perf);

/*============================================================================*
 *                           Signal system syscalls                           *
 *============================================================================*/

	/**
	 * @brief Controls the behavior of a signal.
	 *
	 * The sigclt() function modifies the treatment of a signal.
	 *
	 * @param signum Signal ID.
	 * @param sigact Behavior descriptor.
	 *
	 * @returns Zero if successfully changes the behavior, non zero otherwise.
	 */
	EXTERN int sys_sigclt(int signum, struct sigaction * sigact);

	/**
	 * @brief Schedules an alarm signal.
	 *
	 * The alarm() function schedule an alarm signal to trigger when
	 * the @seconds seconds pass.
	 *
	 * @param seconds Time in seconds.
	 *
	 * @returns Zero if successfully register the alarm, non zero otherwise.
	 */
	EXTERN int sys_alarm(int seconds);

	/**
	 * @brief Sends a signal.
	 *
	 * The sigsend() function sends a signal @signum to another thread @tid.
	 *
	 * @param signum Signal ID.
	 * @param tid    Thread ID.
	 *
	 * @returns Zero if successfully sends the signal, non zero otherwise.
	 */
	EXTERN int sys_sigsend(int signum, int tid);

	/**
	 * @brief Waits for the receipt of a signal.
	 *
	 * The sigwait() function waits for the receipt of a @signum signal.
	 *
	 * @param signum Signal ID.
	 *
	 * @returns Zero if successfully receives the signal, non zero otherwise.
	 */
	EXTERN int sys_sigwait(int signum);

	/**
	 * @brief Returns from a signal handler.
	 *
	 * The sigreturn() function returns from a signal handler, restoring the
	 * execution stream.
	 */
	EXTERN void sys_sigreturn(void);

/**@}*/

#endif /* NANVIX_SYSCALL_H_ */

