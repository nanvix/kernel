/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_EXCEPTION_H_
#define NANVIX_KERNEL_HAL_EXCEPTION_H_

/**
 * @addtogroup kernel-hal-exception Software Exceptions
 * @ingroup kernel-hal
 *
 * @brief Software Exceptions Module
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal/arch.h>

/*============================================================================*
 * Types                                                                      *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Exception handler
 */
typedef void (*exception_handler_t)(const struct exception *,
                                    const struct context *);

#endif /* _ASM_FILE_ */

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Information about exceptions.
 */
struct exception_info {
    exception_handler_t handler; /** Handler */
    const char *name;            /** Name    */
};

#endif /* _ASM_FILE_ */

/*============================================================================*
 * Variables                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @details Lookup table with information about exceptions.
 */
extern struct exception_info exceptions[EXCEPTIONS_NUM];

#endif /* _ASM_FILE_ */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Dumps information about an exception.
 *
 * @param excp Exception information.
 */
extern void exception_dump(const struct exception *excp);

/**
 * @brief Registers an exception handler.
 *
 * @param excpnum Number of the target exception.
 * @param handler Exception handler.
 *
 * @returns Upon successful completion zero is returned. If an exception handler
 * is already registered for exception @p excpnum, the older handler is not
 * overwritten and one is returned. Upon failure a negative error code is
 * returned instead.
 */
extern int exception_register(int excpnum, exception_handler_t handler);

/**
 * @brief Unregisters an exception handler.
 *
 * @param excpnum Number of the target exception.
 *
 * @returns Upon successful completion zero is returned. Upon
 * failure a negative error code is returned instead.
 */
extern int exception_unregister(int excpnum);

/**
 * @brief Gets the number of an exception.
 *
 * @param excp Target exception information structure.
 *
 * @returns The exception number stored in the exception
 * information structure pointed to by @p excp.
 */
extern int exception_get_num(const struct exception *excp);

/**
 * @brief Initializes the exception module.
 */
extern void exceptions_init(void);

/**
 * @brief High-level exception dispatcher.
 *
 * @param excp Exception information.
 * @param ctx  Interrupted context.
 *
 * @note This function is called from assembly code.
 */
extern void do_exception(const struct exception *excp,
                         const struct context *ctx);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_EXCEPTION_H_ */
