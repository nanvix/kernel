/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_KCALL_H_
#define NANVIX_KERNEL_KCALL_H_

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
#define NR__exit 1         /** kernel_exit()             */
#define NR_write 2         /** kernel_write()            */
#define NR_thread_get_id 3 /** kernel_thread_get_id()    */
#define NR_thread_create 4 /** kernel_thread_create()    */
#define NR_thread_exit 5   /** kernel_thread_exit()      */
#define NR_thread_join 6   /** kernel_thread_join()      */
#define NR_thread_yield 7  /** kernel_thread_yield()     */
#define NR_sleep 8         /** kernel_sleep()            */
#define NR_wakeup 9        /** kernel_wakeup()           */
#define NR_shutdown 10     /** kernel_shutdown()         */
#define NR_sigctl 11       /** kernel_sigctl()           */
#define NR_alarm 12        /** kernel_alarm()            */
#define NR_sigsend 13      /** kernel_sigsend()          */
#define NR_sigwait 14      /** kernel_sigwait()          */
#define NR_sigreturn 15    /** kernel_sigreturn()        */
#define NR_clock 16        /** kernel_clock()            */
#define NR_stats 17        /** kernel_stats()            */
#define NR_frame_alloc 18  /** kernel_frame_alloc()      */
#define NR_frame_free 19   /** kernel_frame_free()       */
#define NR_upage_alloc 20  /** kernel_upage_alloc()      */
#define NR_upage_free 21   /** kernel_upage_free()       */
#define NR_upage_map 22    /** kernel_upage_map()        */
#define NR_upage_link 23   /** kernel_upage_link()       */
#define NR_upage_unlink 24 /** kernel_upage_unlink()     */
#define NR_upage_unmap 25  /** kernel_upage_unmap()      */
#define NR_excp_ctrl 26    /** kernel_excp_ctrl()        */
#define NR_excp_pause 27   /** kernel_excp_pause()       */
#define NR_excp_resume 28  /** kernel_excp_resume()      */
#define NR_last_kcall 29   /** NR_SYSCALLS definer      */
/**@}*/

#endif /* NANVIX_KERNEL_KCALL_H_ */
