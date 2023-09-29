/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_PM_THREAD_H_
#define NANVIX_KERNEL_PM_THREAD_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @name Thread States
 */
/**@{*/
#define THREAD_NOT_STARTED 0 /** Not Started */
#define THREAD_READY 1       /** Started     */
#define THREAD_RUNNING 2     /** Running     */
#define THREAD_TERMINATED 5  /** Terminated  */
/**@}*/

/*============================================================================*
 * Types                                                                      *
 *============================================================================*/

/**
 * @brief Thread ID.
 */
typedef int tid_t;

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Thread.
 */
struct thread {
    /**
     * @name Control variables.
     */
    /**@{*/
    tid_t tid;    /** Thread ID. */
    unsigned age; /** Age.       */
    short state;  /** State.     */
    /**@}*/

    /**
     * @name Memory Information
     */
    /**@{*/
    const void *pgdir; /** Page directory. */
    /**@}*/

    /**
     * @name Arguments and functions.
     */
    /**@{*/
    void *arg;              /** Argument.         */
    void *(*start)(void *); /** Starting routine. */
    void *retval;           /** Return value.     */
    /**@}*/

    /**
     * @name Scheduling variables.
     */
    /**@{*/
    unsigned quantum;    /** Quantum.                */
    struct context ctx;  /** Execution context.      */
    byte_t *stack;       /** Stack.                  */
    struct thread *next; /** Next thread in a queue. */
    /**@}*/
};

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Gets the currently running thread.
 *
 * The thread_get() function returns a pointer to the thread
 * that is running in the underlying core.
 *
 * @returns A pointer to the thread that is running in the
 * underlying core.
 */
extern struct thread *thread_get_curr(void);

/**
 * @brief Creates a new thread.
 *
 * @param start Start routine.
 * @param arg   Argument passed to the start routine.
 *
 * @returns Upon successful completion, the ID of the newly created thread is
 * returned. Upon failure, a negative number is returned instead.
 */
extern tid_t thread_create(void *(*start)(void *), void *arg);

/**
 * @brief Yields the calling thread.
 */
extern void thread_yield(void);

/**
 * @brief Terminates the calling thread.
 */
extern noreturn void thread_exit(void);

/**
 * @details Initializes the thread system.
 *
 * @param root_pgdir Root page directory.
 */
extern void thread_init(const void *root_pgdir);

/**
 * @brief Atomically puts the calling thread to sleep.
 *
 * @param lock  Spinlock to release and acquire.
 */
extern void thread_sleep(spinlock_t *lock);

/**
 * @brief Wakes up a thread.
 *
 * @param t Target thread.
 */
extern void thread_wakeup(struct thread *t);

#endif /* NANVIX_KERNEL_PM_THREAD_H_ */
