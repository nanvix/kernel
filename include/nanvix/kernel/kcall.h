/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
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
 * @brief Requests for vmctrl()
 */
/**@{*/
#define VMEM_CHMOD 0 /** Change permissions. */
/**@}*/

/**
 * @brief Access permissions on a virtual memory space.
 */
#define VMEM_READ (1 << 0)  /** Read permission.    */
#define VMEM_WRITE (1 << 1) /** Write permission.   */
#define VMEM_EXEC (1 << 2)  /** Execute permission. */

/**
 * @brief Special PID for a process to refer to itself.
 */
#define PID_SELF (-1)

/**
 * @name System Call Numbers
 */
/**@{*/
#define NR_void0 0          /** kcall_void0()            */
#define NR_void1 1          /** kcall_void1()            */
#define NR_void2 2          /** kcall_void2()            */
#define NR_void3 3          /** kcall_void3()            */
#define NR_void4 4          /** kcall_void4()            */
#define NR_void5 5          /** kcall_void5()            */
#define NR_shutdown 6       /** kcall_shutdown()         */
#define NR_write 7          /** kcall_write()            */
#define NR_fralloc 8        /** kcall_fralloc()          */
#define NR_frfree 9         /** kcall_frfree()           */
#define NR_vmcreate 10      /** kcall_vmcreate()         */
#define NR_vmremove 11      /** kcall_vmremove()         */
#define NR_vmmap 12         /** kcall_vmmap()            */
#define NR_vmunmap 13       /** kcall_vmunmap()          */
#define NR_vmctrl 14        /** kcall_vmctrl()           */
#define NR_vminfo 15        /** kcall_vminfo()           */
#define NR_kmod_get 16      /** kcall_kmod_get()         */
#define NR_spawn 17         /** kcall_spawn()            */
#define NR_semget 18        /** kcall_semget()           */
#define NR_semop 19         /** kcall_semop()            */
#define NR_semctl 20        /** kcall_semctl()           */
#define NR_thread_get_id 21 /** kcall_thread_get_id()    */
#define NR_thread_create 22 /** kcall_thread_create()    */
#define NR_thread_exit 23   /** kcall_thread_exit()      */
#define NR_thread_yield 24  /** kcall_thread_yield()     */
#define NR_mailbox_tag 25   /** kcall_mailbox_tag        */
#define NR_thread_join 26   /** kcall_thread_join()      */
#define NR_thread_detach 27 /** kcall_thread_detach()    */
#define NR_pinfo 28         /** kcall_pinfo()            */
#define NR_getuid 29        /** kcall_getuid()           */
#define NR_geteuid 30       /** kcall_geteuid()          */
#define NR_getgid 31        /** kcall_getgid()           */
#define NR_getegid 32       /** kcall_getegid()          */
#define NR_setuid 33        /** kcall_setuid()           */
#define NR_seteuid 34       /** kcall_seteuid()          */
#define NR_setgid 35        /** kcall_setgid()           */
#define NR_setegid 36       /** kcall_setegid()          */
#define NR_excpctrl 37      /** kcall_excpctrl()         */
#define NR_excpwait 38      /** kcall_excpwait()         */
#define NR_excpresume 39    /** kcall_excpresume()       */
#define NR_last_kcall 40    /** NR_SYSCALLS definer       */
#define NR__exit            /** kcall_exit()             */
#define NR_process_get_id   /** kcall_process_get_id()   */
#define NR_process_create   /** kcall_process_create()   */
#define NR_process_exit     /** kcall_process_exit()     */
#define NR_process_join     /** kcall_process_join()     */
#define NR_process_yield    /** kcall_process_yield()    */
#define NR_sleep            /** kcall_sleep()            */
#define NR_wakeup           /** kcall_wakeup()           */
#define NR_sigctl           /** kcall_sigctl()           */
#define NR_alarm            /** kcall_alarm()            */
#define NR_sigsend          /** kcall_sigsend()          */
#define NR_sigwait          /** kcall_sigwait()          */
#define NR_sigreturn        /** kcall_sigreturn()        */
#define NR_clock            /** kcall_clock()            */
#define NR_stats            /** kcall_stats()            */
#define NR_upage_alloc      /** kcall_upage_alloc()      */
#define NR_upage_free       /** kcall_upage_free()       */
#define NR_upage_map        /** kcall_upage_map()        */
#define NR_upage_link       /** kcall_upage_link()       */
#define NR_upage_unlink     /** kcall_upage_unlink()     */
#define NR_upage_unmap      /** kcall_upage_unmap()      */
#define NR_excp_ctrl        /** kcall_excp_ctrl()        */
#define NR_excp_pause       /** kcall_excp_pause()       */
#define NR_excp_resume      /** kcall_excp_resume()      */
/**@}*/

#endif /* NANVIX_KERNEL_KCALL_H_ */
