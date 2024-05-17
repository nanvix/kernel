/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_INTERRUPT_H_
#define NANVIX_KERNEL_HAL_INTERRUPT_H_

/**
 * @addtogroup kernel-hal-interrupt Interrupts
 * @ingroup kernel-hal
 *
 * @brief Interrupts Module
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal/arch.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Threshold for spurious interrupts.
 *
 * INTERRUPT_SPURIOUS_THRESHOLD states the number of spurious
 * interrupts that we are willing to get, before enter in verbose
 * mode.
 */
#define INTERRUPT_SPURIOUS_THRESHOLD 100

/*============================================================================*
 * Types                                                                      *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Hardware interrupt handler.
 */
typedef void (*interrupt_handler_t)(void);

#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief High-level hardware interrupt dispatcher.
 *
 * @param intnum Number of the interrupt.
 */
extern void do_interrupt(int intnum);

/**
 * @brief Registers an interrupt handler.
 *
 * @param num     Number of the interrupt.
 * @param handler Interrupt handler.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int interrupt_register(int num, interrupt_handler_t handler);

/**
 * @brief Unregisters an interrupt handler.
 *
 * @param num Number of the interrupt.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int interrupt_unregister(int num);

/**
 * @brief Setups hardware interrupts.
 */
extern void interrupts_init(void);

/**
 * @brief Forges an interrupt stack.
 *
 * @param user_stack   User stack.
 * @param kernel_stack Kernel stack.
 * @param user_func    User function.
 * @param kernel_func  Kernel function.
 *
 * @returns Upon successful completion, a pointer to the forged stack is
 * returned. Upon failure, a null pointer is returned instead.
 */
extern void *interrupt_forge_stack(void *user_stack, void *kernel_stack,
                                   void (*user_func)(void),
                                   void (*kernel_func)(void));

/**
 * @brief interrupt handlers.
 */
extern interrupt_handler_t interrupt_handlers[INTERRUPTS_NUM];

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_INTERRUPT_H_ */
