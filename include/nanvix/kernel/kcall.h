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
#define NR_void0 0              /** kcall_void0()             */
#define NR_void1 1              /** kcall_void1()             */
#define NR_void2 2              /** kcall_void2()             */
#define NR_void3 3              /** kcall_void3()             */
#define NR_void4 4              /** kcall_void4()             */
#define NR_void5 5              /** kcall_void5()             */
#define NR_shutdown 6           /** kcall_shutdown()          */
#define NR_write 7              /** kcall_write()             */
#define NR_fralloc 8            /** kcall_fralloc()           */
#define NR_frfree 9             /** kcall_frfree()            */
#define NR_vmcreate 10          /** kcall_vmcreate()          */
#define NR_vmremove 11          /** kcall_vmremove()          */
#define NR_vmmap 12             /** kcall_vmmap()             */
#define NR_vmunmap 13           /** kcall_vmunmap()           */
#define NR_vmctrl 14            /** kcall_vmctrl()            */
#define NR_vminfo 15            /** kcall_vminfo()            */
#define NR_kmod_get 16          /** kcall_kmod_get()          */
#define NR_spawn 17             /** kcall_spawn()             */
#define NR_semget 18            /** kcall_semget()            */
#define NR_semop 19             /** kcall_semop()             */
#define NR_semctl 20            /** kcall_semctl()            */
#define NR_thread_get_id 21     /** kcall_thread_get_id()     */
#define NR_thread_create 22     /** kcall_thread_create()     */
#define NR_thread_exit 23       /** kcall_thread_exit()       */
#define NR_thread_yield 24      /** kcall_thread_yield()      */
#define NR_thread_join 25       /** kcall_thread_join()       */
#define NR_thread_detach 26     /** kcall_thread_detach()     */
#define NR_pinfo 27             /** kcall_pinfo()             */
#define NR_getuid 28            /** kcall_getuid()            */
#define NR_geteuid 29           /** kcall_geteuid()           */
#define NR_getgid 30            /** kcall_getgid()            */
#define NR_getegid 31           /** kcall_getegid()           */
#define NR_setuid 32            /** kcall_setuid()            */
#define NR_seteuid 33           /** kcall_seteuid()           */
#define NR_setgid 34            /** kcall_setgid()            */
#define NR_setegid 35           /** kcall_setegid()           */
#define NR_excpctrl 36          /** kcall_excpctrl()          */
#define NR_excpwait 37          /** kcall_excpwait()          */
#define NR_excpresume 38        /** kcall_excpresume()        */
#define NR_do_mailbox_create 39 /** kcall_do_mailbox_create() */
#define NR_do_mailbox_open 40   /** kcall_mailbox_open()      */
#define NR_do_mailbox_unlink 41 /** kcall_mailbox_unlink()    */
#define NR_do_mailbox_close 42  /** kcall_mailbox_close()     */
#define NR_do_mailbox_write 43  /** kcall_mailbox_write()     */
#define NR_do_mailbox_read 44   /** kcall_mailbox_read()      */
#define NR_last_kcall 45        /** NR_SYSCALLS definer       */
/**@}*/

#endif /* NANVIX_KERNEL_KCALL_H_ */
