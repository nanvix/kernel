/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef KERNEL_KCALL_MOD_H_
#define KERNEL_KCALL_MOD_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/excp.h>
#include <nanvix/kernel/kmod.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <nanvix/types.h>
#include <stddef.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Issues a void kernel call that takes no arguments.
 *
 * @return Always returns zero.
 */
extern int kcall_void0(void);

/**
 * @brief Issues a void kernel call that takes one argument.
 *
 * @param arg0 First argument.
 *
 * @return Always returns @p arg0.
 */
extern int kcall_void1(int arg0);

/**
 * @brief Issues a void kernel call that takes two arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 *
 * @return Always returns @p arg0 + @p arg1.
 */
extern int kcall_void2(int arg0, int arg1);

/**
 * @brief Issues a void kernel call that takes three arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 * @param arg2 Third argument.
 *
 * @return Always returns @p arg0 + @p arg1 + @p arg2.
 */
extern int kcall_void3(int arg0, int arg1, int arg2);

/**
 * @brief Issues a void kernel call that takes four arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 * @param arg2 Third argument.
 * @param arg3 Fourth argument.
 *
 * @return Always returns @p arg0 + @p arg1 + @p arg2 + @p arg3.
 */
extern int kcall_void4(int arg0, int arg1, int arg2, int arg3);

/**
 * @brief Issues a void kernel call that takes five arguments.
 *
 * @param arg0 First argument.
 * @param arg1 Second argument.
 * @param arg2 Third argument.
 * @param arg3 Fourth argument.
 * @param arg4 Fifth argument.
 *
 * @return Always returns @p arg0 + @p arg1 + @p arg2 + @p arg3 + @p arg4.
 */
extern int kcall_void5(int arg0, int arg1, int arg2, int arg3, int arg4);

/**
 * @brief Shutdowns the system
 */
extern noreturn void kcall_shutdown(void);

/**
 * @brief Writes a buffer to a file descriptor.
 *
 * @param fd  Target file descriptor.
 * @param buf Target buffer.
 * @param n   Number of bytes to write.
 *
 * @returns The number of bytes written is returned.
 */
extern size_t kcall_write(int fd, const char *buf, size_t n);

/**
 * @brief Attempts to allocate a page frame.
 *
 * @return On success, the number of the allocated page frame is returned.
 * On failure, @p FRAME_NULL is returned instead.
 */
extern frame_t kcall_fralloc(void);

/**
 * @brief Frees a page frame.
 *
 * @param frame Number of the target page frame.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int kcall_frfree(frame_t frame);

/**
 * @brief Creates a virtual memory space.
 *
 * @return Upon successful completion, a handle to the newly created virtual
 * memory space is returned. Upon failure, @p VMEM_NULL is returned instead.
 */
extern vmem_t kcall_vmcreate(void);

/**
 * @brief Removes a virtual memory space.
 *
 * @param vmem Handle to the target virtual memory space.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kcall_vmremove(vmem_t vmem);

/**
 * @brief Maps a page frame at a virtual address.
 *
 * @param vmem Handle to the target virtual memory space.
 * @param vaddr Handle to the virtual address.
 * @param frame Target page frame
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kcall_vmmap(vmem_t vmem, vaddr_t vaddr, frame_t frame);

/**
 * @brief Unmaps a page frame from a virtual address.
 *
 * @param vmem Handle to the target virtual memory space.
 * @param vaddr Handle to the target virtual address.
 *
 * @return Upon successful completion, a handle to the unmapped page frame is
 * returned. Upon failure, @p FRAME_NULL is returned instead.
 */
extern frame_t kcall_vmunmap(vmem_t vmem, vaddr_t vaddr);

/**
 * @brief Manipulates various parameters from a virtual memory space.
 *
 * @param vmem    Handle to the target virtual memory space.
 * @param request Request.
 * @param vaddr   Target virtual address.
 * @param mode    Access permissions mode.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kcall_vmctrl(vmem_t vmem, unsigned request, vaddr_t vaddr,
                        mode_t mode);

/**
 * @brief Gets information on a page.
 *
 * @param vmem Handle to the target virtual memory space.
 * @param vaddr Target virtual address.
 * @param buf Storage location for page information.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kcall_vminfo(vmem_t vmem, vaddr_t vaddr, struct pageinfo *buf);

/**
 * @brief Gets information on a kernel module.
 *
 * @param kmod Storage location for kernel module information.
 * @param index Index of target kernel module.
 *
 * @returns Upon successful completion, zero is returned. Upon failure,
 * a negative number is returned instead.
 */
extern int kcall_kmod_get(struct kmod *kmod, unsigned index);

/**
 * @brief Spawns a new process.
 *
 * @param image Image of the process.
 *
 * @returns Upon successful completion, the PID of the spawned process
 * is returned. Upon failure, a negative error code is returned
 * instead.
 */
extern pid_t kcall_spawn(void *image);

/**
 * @brief Gets various information about the calling process.
 *
 * @param buf Storage location for process information.
 *
 * @returns Upon successful completion, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
extern int kcall_pinfo(struct process_info *buf);

/**
 * @brief Gets the real user ID of the calling process.
 *
 * @return The real user ID stored in the identity pointed to by @p id is
 * returned.
 */
extern uid_t kcall_getuid(void);

/**
 * @brief Gets the effective user ID of the calling process.
 *
 * @return The effective user ID stored in the identity pointed to by @p id is
 * returned.
 */
extern uid_t kcall_geteuid(void);

/**
 * @brief Gets the user group ID of the calling process.
 *
 * @return The user group ID stored in the identity pointed to by @p id is
 * returned.
 */
extern gid_t kcall_getgid(void);

/**
 * @brief Gets the effective user group ID of the calling process.
 *
 * @return The effective user group ID stored in the identity pointed to by @p
 * id is returned.
 */
extern gid_t kcall_getegid(void);

/**
 * @brief Sets the real user ID of the calling process.
 *
 * @param uid User ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int kcall_setuid(uid_t uid);

/**
 * @brief Sets the effective user ID of the calling process.
 *
 * @param euid Effective user ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int kcall_seteuid(uid_t euid);

/**
 * @brief Sets the user group ID of the calling process.
 *
 * @param gid user group ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 */
extern int kcall_setgid(gid_t gid);

/**
 * @brief Sets the effective user group ID of the calling process.
 *
 * @param egid Effective user group ID.
 *
 * @return On successful completion, zero is returned. On error, a negative
 * error code is returned instead.
 */
extern int kcall_setegid(gid_t egid);

/**
 * @brief Get a semaphore.
 *
 * @param key Key for allow semaphore access.
 *
 * @returns Upon successful, Semaphore ID. Upon Failure, a negative
 * error code is returned instead.
 */
extern int kcall_semget(unsigned key);

/**
 * @brief Get mailbox tag.
 *
 * @param mbxid mailbox ID.
 *
 * @returns Upon successful, mailbox tag. Upon Failure, a negative
 * error code is returned instead.
 */
extern int kcall_mailbox_tag(int mbxid);

/**
 * @brief Verify if process can operate in semaphore (id), and operate (op):
 *
 * @param id Semaphore Identifier.
 *
 * @param op Operation.
 *
 * @returns Upon successful, zero code. Upon Failure, a negative
 * error code is returned instead.
 */
extern int kcall_semop(int id, int op);

/**
 * @brief Verify if process can control the semaphore (id), and exec (cmd).
 *
 * @param id Semaphore Identifier.
 *
 * @param cmd Command.
 *
 * @param val Value.
 *
 * @returns Upon successful, zero code or semaphore value. Upon Failure,
 * a negative error code is returned instead.
 */
extern int kcall_semctl(int id, int cmd, int val);

/**
 * @brief Gets the ID of the calling thread.
 *
 * @returns The ID of the calling thread is returned.
 */
extern tid_t kcall_thread_get_id(void);

/**
 * @brief Creates a new thread.
 *
 * @param start Start routine.
 * @param args Arguments.
 *@param caller Thread caller function.
 *
 * @returns Upon successful completion, the ID of the newly created
 * thread is returned. Upon failure, a negative error code is
 * returned instead.
 */
extern tid_t kcall_thread_create(void (*start)(void *), void *args,
                                 void (*caller)());

/**
 * @brief Exits the calling thread.
 *
 * @param retval Return value.
 */
extern noreturn void kcall_thread_exit(void *retval);

/**
 * @brief Yields the processor to another thread.
 *
 * @returns Upon successful completion, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
extern noreturn void kcall_thread_yield(void);

/**
 * @brief Waits for the target thread to terminate.
 *
 * @param tid ID of the target thread.
 * @param retval Location to store the return value of the target thread.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int kcall_thread_join(tid_t tid, void **retval);

/**
 * @brief Detaches the target thread.
 *
 * @param tid ID of the target thread.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int kcall_thread_detach(tid_t tid);

/**
 * @brief Controls which action to take when an exception happens.
 *
 * @param excpnum Exception number
 * @param action Action to take.
 *
 * @return Upon successful completion zero is returned. Upon failure a negative
 * error code is returned instead.
 */
extern int kcall_excpctrl(int excpnum, int action);

/**
 * @brief Resumes the execution of a faulting process.
 *
 * @param excpnum Exception number
 *
 * @return Upon successful completion zero is returned. Upon failure a negative
 * error code is returned instead.
 */
extern int kcall_excpresume(int excpnum);

/**
 * @brief Waits for an exception to happen.
 *
 * @param info Exception information.
 *
 * @return Upon successful completion zero is returned. Upon failure a negative
 * error code is returned instead.
 */
extern int kcall_excpwait(struct excpinfo *info);

/*============================================================================*/

#endif /* KERNEL_KCALL_MOD_H_ */
