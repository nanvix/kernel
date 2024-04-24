/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_PM_SEMAPHORE_H_
#define NANVIX_KERNEL_PM_SEMAPHORE_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/pm/cond.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum Number of semaphores in the system.
 */
#define SEMAPHORE_MAX 128

/**
 * @brief Active semaphore?
 */
#define SEMAPHORE_ACTIVE 1

/**
 * @brief Inative semaphore?
 */
#define SEMAPHORE_INACTIVE 0

/**
 * @brief Semaphore operation UP.
 */
#define SEMAPHORE_UP 0

/**
 * @brief Semaphore operation DOWN.
 */
#define SEMAPHORE_DOWN 1

/**
 * @brief Semaphore operation DOWN without lock.
 */
#define SEMAPHORE_TRYLOCK 2

/**
 * @brief Command Semaphore Get Value
 */
#define SEMAPHORE_GETVALUE 0

/**
 * @brief Command Semaphore Set Value
 */
#define SEMAPHORE_SETVALUE 1

/**
 * @brief Command Semaphore Delete
 */
#define SEMAPHORE_DELETE 2

/*============================================================================*
 * Public Variables                                                           *
 *============================================================================*/

/**
 * @brief Kernel's semaphore.
 *
 * @details Control acesse in Syscall dispatcher.
 */
extern int kernel_semaphore;

/**
 * @brief Kernel's semaphore.
 *
 * @details Control acesse in Syscall dispatcher.
 */
extern int user_semaphore;

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Semaphore.
 */
struct semaphore {
    int count;                    /** Semaphore counter.  */
    struct condvar cond;          /** Condition variable. */
    int state;                    /** Semaphore state     */
    pid_t proc_owner;             /** Owner process.      */
    pid_t proc_user[PROCESS_MAX]; /** Users process.      */
    unsigned key;                 /** Semphore key.       */
};

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Set process as semaphore user.
 *
 * @param semid Semaphore id.
 *
 * @return Upon successful completion, (semid) is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int semaphore_get(int semid);

/**
 * @brief Initialize semaphore structure.
 *
 * @param key Semaphore key.
 *
 * @return Upon successful completion, (semid) is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int semaphore_create(unsigned key);

/**
 * @brief Delete or drop semaphore.
 *
 * @param semid Semaphore id.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int semaphore_delete(int semid);

/*
 * @brief Initialize semaphores table.
 */
extern void semtable_init(void);

/*
 * @brief Initialize control variables.
 *
 * @param semid Semaphore id.
 *
 * @param count Semaphore counter.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int semaphore_set(int semid, int count);

/*
 * @brief Return semaphore id.
 *
 * @p key Key associated with the semaphore.
 *
 * @return Upon successful completion, (semid) is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int semaphore_getid(int key);

/*
 * @brief Return value of semaphore count.
 *
 * @p semid Semaphore id.
 *
 * @return Upon successful completion, Semaphore Count is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int semaphore_getcount(int semid);

/**
 * @brief Performs a down operation in a semaphore.
 *
 * @p semid Semaphore id.
 */
extern int semaphore_down(int semid);

/**
 * @brief Performs an up operation in a semaphore.
 *
 * @p semid Semaphore id.
 */
extern int semaphore_up(int semid);

/**
 * @brief Performs a down operation in a semaphore, but
 * no lock.
 *
 * @p semid Semaphore id.
 */
extern int semaphore_trylock(int semid);

#endif /* NANVIX_KERNEL_PM_SEMAPHORE_H_ */
