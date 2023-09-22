/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_MM_FRAME_H_
#define NANVIX_KERNEL_MM_FRAME_H_
/**
 * @addtogroup kernel-mm-frame Frame Allocator
 * @ingroup kernel-mm
 *
 * @brief Page Frame Allocator
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @param Null frame.
 */
#define FRAME_NULL ((frame_t)-1)

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Prints the page frame allocator bitmap.
 */
extern void frame_print(void);

/**
 * @brief Allocates a page frame.
 *
 * @returns Upon successful completion, the number of the
 * allocated page frame is returned. Upon failure, @p FRAME_NULL
 * is returned instead.
 */
extern frame_t frame_alloc_any(void);

/**
 * @brief Attempts to allocate a specific page frame.
 *
 * @param frame Number of the target page frame.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int frame_alloc(frame_t frame);

/**
 * @brief Frees a page frame.
 *
 * @param frame Number of the target page frame.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int frame_free(frame_t frame);

/**
 * @brief Initializes the frame allocator.
 */
extern void frame_init(void);

/**
 * @brief Books all page frames within a range.
 *
 * @param base Base address.
 * @param end  End address.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int frame_book_range(paddr_t base, paddr_t end);

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_MM_FRAME_H_ */
