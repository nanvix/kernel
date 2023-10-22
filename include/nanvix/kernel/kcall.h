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
#define NR_void0 0          /** kernel_void0()            */
#define NR_void1 1          /** kernel_void1()            */
#define NR_void2 2          /** kernel_void2()            */
#define NR_void3 3          /** kernel_void3()            */
#define NR_void4 4          /** kernel_void4()            */
#define NR_void5 5          /** kernel_void5()            */
#define NR_shutdown 6       /** kernel_shutdown()         */
#define NR_write 7          /** kernel_write()            */
#define NR__exit 8          /** kernel_exit()             */
#define NR_thread_get_id 9  /** kernel_thread_get_id()    */
#define NR_thread_create 10 /** kernel_thread_create()    */
#define NR_thread_exit 11   /** kernel_thread_exit()      */
#define NR_thread_join 12   /** kernel_thread_join()      */
#define NR_thread_yield 13  /** kernel_thread_yield()     */
#define NR_sleep 14         /** kernel_sleep()            */
#define NR_wakeup 15        /** kernel_wakeup()           */
#define NR_sigctl 16        /** kernel_sigctl()           */
#define NR_alarm 17         /** kernel_alarm()            */
#define NR_sigsend 18       /** kernel_sigsend()          */
#define NR_sigwait 19       /** kernel_sigwait()          */
#define NR_sigreturn 20     /** kernel_sigreturn()        */
#define NR_clock 21         /** kernel_clock()            */
#define NR_stats 22         /** kernel_stats()            */
#define NR_frame_alloc 23   /** kernel_frame_alloc()      */
#define NR_frame_free 24    /** kernel_frame_free()       */
#define NR_upage_alloc 25   /** kernel_upage_alloc()      */
#define NR_upage_free 26    /** kernel_upage_free()       */
#define NR_upage_map 27     /** kernel_upage_map()        */
#define NR_upage_link 28    /** kernel_upage_link()       */
#define NR_upage_unlink 29  /** kernel_upage_unlink()     */
#define NR_upage_unmap 30   /** kernel_upage_unmap()      */
#define NR_excp_ctrl 31     /** kernel_excp_ctrl()        */
#define NR_excp_pause 32    /** kernel_excp_pause()       */
#define NR_excp_resume 33   /** kernel_excp_resume()      */
#define NR_last_kcall 34    /** NR_SYSCALLS definer      */
/**@}*/

#endif /* NANVIX_KERNEL_KCALL_H_ */
