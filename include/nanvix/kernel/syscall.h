/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

#ifdef __NANVIX_MICROKERNEL
	#include <nanvix/kernel/thread.h>
	#include <nanvix/kernel/sync.h>
	#include <nanvix/kernel/mailbox.h>
	#include <nanvix/kernel/portal.h>
	#include <nanvix/kernel/signal.h>
	#include <nanvix/const.h>
#endif /* __NANVIX_MICROKERNEL */

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
	#define NR_SYSCALLS NR_last_kcall

	/**
	 * @name System Call Numbers
	 */
	/**@{*/
	#define NR__exit           1 /**< kernel_exit()           */
	#define NR_write           2 /**< kernel_write()          */
	#define NR_thread_get_id   3 /**< kernel_thread_get_id()  */
	#define NR_thread_create   4 /**< kernel_thread_create()  */
	#define NR_thread_exit     5 /**< kernel_thread_exit()    */
	#define NR_thread_join     6 /**< kernel_thread_join()    */
	#define NR_sleep           7 /**< kernel_sleep()          */
	#define NR_wakeup          8 /**< kernel_wakeup()         */
	#define NR_shutdown        9 /**< kernel_shutdown()       */
	#define NR_sigctl         10 /**< kernel_perf_read()      */
	#define NR_alarm          11 /**< kernel_perf_read()      */
	#define NR_sigsend        12 /**< kernel_perf_read()      */
	#define NR_sigwait        13 /**< kernel_perf_read()      */
	#define NR_sigreturn      14 /**< kernel_perf_read()      */
	#define NR_node_get_num   15 /**< kernel_node_get_num()   */
	#define NR_node_set_num   16 /**< kernel_node_set_num()   */
	#define NR_sync_create    17 /**< kernel_sync_create()    */
	#define NR_sync_open      18 /**< kernel_sync_open()      */
	#define NR_sync_wait      19 /**< kernel_sync_wait()      */
	#define NR_sync_signal    20 /**< kernel_sync_signal()    */
	#define NR_sync_close     21 /**< kernel_sync_close()     */
	#define NR_sync_unlink    22 /**< kernel_sync_unlink()    */
	#define NR_mailbox_create 23 /**< kernel_mailbox_create() */
	#define NR_mailbox_open   24 /**< kernel_mailbox_open()   */
	#define NR_mailbox_unlink 25 /**< kernel_mailbox_unlink() */
	#define NR_mailbox_close  26 /**< kernel_mailbox_close()  */
	#define NR_mailbox_awrite 27 /**< kernel_mailbox_awrite() */
	#define NR_mailbox_aread  28 /**< kernel_mailbox_aread()  */
	#define NR_mailbox_wait   29 /**< kernel_mailbox_wait()   */
	#define NR_mailbox_ioctl  30 /**< kernel_mailbox_ioctl()  */
	#define NR_portal_create  31 /**< kernel_portal_create()  */
	#define NR_portal_allow   32 /**< kernel_portal_allow()   */
	#define NR_portal_open    33 /**< kernel_portal_open()    */
	#define NR_portal_unlink  34 /**< kernel_portal_unlink()  */
	#define NR_portal_close   35 /**< kernel_portal_close()   */
	#define NR_portal_awrite  36 /**< kernel_portal_awrite()  */
	#define NR_portal_aread   37 /**< kernel_portal_aread()   */
	#define NR_portal_wait    38 /**< kernel_portal_wait()    */
	#define NR_portal_ioctl   39 /**< kernel_portal_ioctl()   */
	#define NR_clock          40 /**< kernel_clock()          */
	#define NR_stats          42 /**< kernel_stats()          */

	#define NR_last_kcall     43 /**< NR_SYSCALLS definer     */
	/**@}*/

/*============================================================================*
 * Thread Kernel Calls                                                        *
 *============================================================================*/

#ifdef __NANVIX_MICROKERNEL

	EXTERN void kernel_exit(int);
	EXTERN ssize_t kernel_write(int, const char *, size_t);
	EXTERN int kernel_thread_get_id(void);
	EXTERN int kernel_thread_create(int *, void*(*)(void*), void *);
	EXTERN void kernel_thread_exit(void *);
	EXTERN int kernel_thread_join(int, void **);
	EXTERN int kernel_sleep(void);
	EXTERN int kernel_wakeup(int);

	/**
	 * @brief Shutdowns the kernel.
	 *
	 * @returns Upon successful completion, this function does not
	 * return.Upon failure, a negative error code is returned instead.
	 */
	EXTERN int kernel_shutdown(void);

#endif /* __NANVIX_MICROKERNEL */

/*============================================================================*
 * Thread Kernel Calls                                                        *
 *============================================================================*/

#ifdef __NANVIX_MICROKERNEL

	/**
	 * @brief Controls the behavior of a signal.
	 *
	 * @param signum Signal ID.
	 * @param sigact Behavior descriptor.
	 *
	 * @returns Zero if successfully changes the behavior, non zero otherwise.
	 */
	EXTERN int kernel_sigctl(int signum, struct ksigaction *sigact);

	/**
	 * @brief Schedules an alarm signal.
	 *
	 * @param seconds Time in seconds.
	 *
	 * @returns Zero if successfully register the alarm, non zero otherwise.
	 */
	EXTERN int kernel_alarm(int seconds);

	/**
	 * @brief Sends a signal.
	 *
	 * @param signum Signal ID.
	 * @param tid    Thread ID.
	 *
	 * @returns Zero if successfully sends the signal, non zero otherwise.
	 */
	EXTERN int kernel_sigsend(int signum, int tid);

	/**
	 * @brief Waits for the receipt of a signal.
	 *
	 * @param signum Signal ID.
	 *
	 * @returns Zero if successfully receives the signal, non zero otherwise.
	 */
	EXTERN int kernel_sigwait(int signum);

	/**
	 * @brief Returns from a signal handler.
	 */
	EXTERN void kernel_sigreturn(void);

#endif /* __NANVIX_MICROKERNEL */

/*============================================================================*
 * NoC Kernel Calls                                                           *
 *============================================================================*/

	EXTERN int kernel_node_get_num(int);
	EXTERN int kernel_node_set_num(int, int);

/*============================================================================*
 * Sync Kernel Calls                                                          *
 *============================================================================*/

#ifdef __NANVIX_MICROKERNEL

	EXTERN int kernel_sync_create(const int *, int, int);
	EXTERN int kernel_sync_open(const int *, int, int);
	EXTERN int kernel_sync_unlink(int);
	EXTERN int kernel_sync_close(int);
	EXTERN int kernel_sync_wait(int);
	EXTERN int kernel_sync_signal(int);

#endif /* __NANVIX_MICROKERNEL */

/*============================================================================*
 * Mailbox Kernel Calls                                                       *
 *============================================================================*/

#ifdef __NANVIX_MICROKERNEL

	EXTERN int kernel_mailbox_create(int);
	EXTERN int kernel_mailbox_open(int);
	EXTERN int kernel_mailbox_unlink(int);
	EXTERN int kernel_mailbox_close(int);
	EXTERN int kernel_mailbox_aread(int, void *, size_t);
	EXTERN int kernel_mailbox_awrite(int, const void *, size_t);
	EXTERN int kernel_mailbox_wait(int);
	EXTERN int kernel_mailbox_ioctl(int, unsigned, va_list *);

#endif /* __NANVIX_MICROKERNEL */

/*============================================================================*
 * Portal Kernel Calls                                                        *
 *============================================================================*/

#ifdef __NANVIX_MICROKERNEL

	EXTERN int kernel_portal_create(int);
	EXTERN int kernel_portal_allow(int, int);
	EXTERN int kernel_portal_open(int, int);
	EXTERN int kernel_portal_unlink(int);
	EXTERN int kernel_portal_close(int);
	EXTERN int kernel_portal_aread(int, void *, size_t);
	EXTERN int kernel_portal_awrite(int, const void *, size_t);
	EXTERN int kernel_portal_wait(int);
	EXTERN int kernel_portal_ioctl(int, unsigned, va_list *);

#endif /* __NANVIX_MICROKERNEL */

/*============================================================================*
 * Miscellaneous Kernel Calls                                                 *
 *============================================================================*/

	/**
	 * @brief Gets the kernel wall clock.
	 *
	 * @param buffer Store location for wall clock.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int kernel_clock(uint64_t *buffer);

	/**
	 * @brief Gets performance statistics of the kernel.
	 *
	 * @param buffer Store location for the statistics.
	 * @param perf   Performance counter to watch.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int kernel_stats(uint64_t *buffer, int perf);

/**@}*/

#endif /* NANVIX_SYSCALL_H_ */

