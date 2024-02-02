/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_PM_THREAD_H_
#define NANVIX_KERNEL_PM_THREAD_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/types.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum number of threads.
 */
#define THREADS_MAX 16

/**
 * @name Thread States
 */
/**@{*/
#define THREAD_AVAILABLE 0  /** Available     */
#define THREAD_STARTED 1    /** Started     */
#define THREAD_READY 2      /** Ready       */
#define THREAD_RUNNING 3    /** Running     */
#define THREAD_TERMINATED 4 /** Terminated  */
#define THREAD_WAITING 5    /** Waiting     */
/**@}*/

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Thread.
 */
struct thread {
    tid_t tid;          /** Thread ID.              */
    pid_t pid;          /** Process ID.             */
    unsigned age;       /** Age.                    */
    short state;        /** State.                  */
    unsigned quantum;   /** Quantum.                */
    struct context ctx; /** Execution context.      */
    byte_t *stack;      /** Stack.                  */
};

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Initializes the thread system.
 */
extern void thread_init(void);

/**
 * @brief Creates a new thread.
 *
 * @param pid  ID of the target process.
 * @param root Root thread?
 *
 * @returns Upon successful completion, the ID of the created thread is
 * returned. Upon failure, a negative error code is returned instead.
 */
extern tid_t thread_create(pid_t pid, bool root);

/**
 * @brief Releases the target thread entry.
 *
 * @param tid ID of the target thread.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative number is returned instead.
 */
extern int thread_free(tid_t tid);

/**
 * @brief Releases all threads from a target process.
 *
 * @param pid ID of the target process.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative number is returned instead.
 */
extern int thread_free_all(pid_t pid);

/**
 * @brief Gets the execution context of a thread.
 *
 * @param tid ID of the target thread.
 *
 * @returns Upon successful completion, a pointer to the execution context
 * of the target thread is returned. Upon failure, NULL is returned instead.
 */
extern struct context *thread_get_ctx(tid_t tid);

/**
 * @brief Gets the currently running thread.
 *
 * @returns Upon successful completion, the ID of the currently running
 * thread is returned. Upon failure, a negative error code is returned
 * instead.
 */
extern tid_t thread_get_curr(void);

/**
 * @brief Gets the ID of the target thread's owner process.
 *
 * @param tid ID of the target thread.
 *
 * @returns Upon successful completion, the ID of the target thread's owner
 * process is returned. Upon failure, a negative number is returned instead.
 */
extern pid_t thread_get_pid(tid_t tid);

/**
 * @brief Gets the stack of the target thread.
 *
 * @param tid ID of the target thread.
 *
 * @returns Upon successful completion, a pointer to the stack of the target
 * thread is returned. Upon failure, NULL is returned instead.
 */
extern byte_t *thread_get_stack(tid_t tid);

/**
 * @brief Yields the processor to another thread.
 */
extern void thread_yield(void);

/**
 * @brief Puts the calling thread to sleep.
 */
extern void thread_sleep(void);

/**
 * @brief Wakes up a thread.
 *
 * @param tid ID of the target thread.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative number is returned instead.
 */
extern int thread_wakeup(tid_t tid);

/**
 * @brief Puts all threads from the calling process to sleep.
 */
extern void thread_sleep_all(void);

/**
 * @brief Wakes up all threads from a process.
 *
 * @param pid ID of the target process.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative number is returned instead.
 */
extern int thread_wakeup_all(pid_t pid);

/**
 * @brief Exits the calling thread.
 */
extern void thread_exit(void);

#endif /* NANVIX_KERNEL_PM_THREAD_H_ */
