/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_EXCP_H_
#define NANVIX_KERNEL_EXCP_H_

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Actions on exceptions
 */
/**@{*/
#define EXCP_DEFER 0  /* Defer exception to kernel.  */
#define EXCP_HANDLE 1 /* Handle exception.           */
/**@}*/

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Size of exception information structure.
 */
#define __SIZEOF_EXCPINFO 12

/**
 * @brief Exception information.
 */
struct excpinfo {
    int num;      /** Exception number. */
    vaddr_t addr; /** Faulting address. */
    vaddr_t pc;   /** Program counter. */
};

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Initialize the exceptions management subsystem.
 *
 * @return Upon successful completion zero is returned. Upon failure a negative
 * error code is returned instead.
 */
extern int excp_init(void);

/**
 * @brief Controls which action to take when an exception happens.
 *
 * @param excpnum Exception number
 * @param action Action to take.
 *
 * @return Upon successful completion zero is returned. Upon failure a negative
 * error code is returned instead.
 */
extern int excp_control(int excpnum, int action);

/**
 * @brief Resumes the execution of a faulting process.
 *
 * @param excpnum Exception number
 *
 * @return Upon successful completion zero is returned. Upon failure a negative
 * error code is returned instead.
 */
extern int excp_resume(int excpnum);

/**
 * @brief Waits for an exception to be raised.
 *
 * @param info Exception information.
 *
 * @return Upon successful completion zero is returned. Upon failure a negative
 * error code is returned instead.
 */
extern int excp_wait(struct excpinfo *info);

/*============================================================================*/

#endif /* NANVIX_KERNEL_EXCP_H_ */
