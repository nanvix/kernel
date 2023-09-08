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
 * @brief Number of page frames for user use.
 */
#define NUM_UFRAMES (UMEM_SIZE / PAGE_SIZE)

/**
 * @param Null frame.
 */
#define FRAME_NULL ((frame_t)-1)

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Asserts if a frame ID is valid.
 *
 * The frame_is_valid_id() function asserts whether or not the
 * frame @p ID is valid.
 *
 * @returns If @p id is valid, non zero is returned. Otherwise,
 * zero is returned instead.
 *
 * @author Pedro Henrique Penna
 */
extern int frame_is_valid_id(frame_t id);

/**
 * @brief Converts an ID of a user page frame to a page frame number.
 *
 * @param id ID of target user page frame.
 *
 * @returns Frame number of target user page frame.
 *
 * @author Pedro Henrique Penna
 */
extern frame_t frame_id_to_num(frame_t id);

/**
 * @brief Asserts if a frame number is valid.
 *
 * The frame_is_valid_num() function asserts whether or not the
 * frame number @p frame is valid.
 *
 * @returns If @p frame is valid, non zero is returned. Otherwise,
 * zero is returned instead.
 *
 * @author Pedro Henrique Penna
 */
extern int frame_is_valid_num(frame_t frame);

/**
 * @brief Converts a page frame number to an ID of a user page frame.
 *
 * @param frame Number of the target page frame.
 *
 * @returns ID of target user page frame.
 *
 * @author Pedro Henrique Penna
 */
extern int frame_num_to_id(frame_t frame);

/**
 * @brief Asserts if a page frame is allocated.
 *
 * @param frame Number of the target page frame.
 *
 * @returns One if the target page frame is allocated and zero
 * otherwise.
 */
extern int frame_is_allocated(frame_t frame);

/**
 * @brief Allocates a page frame.
 *
 * @returns Upon successful completion, the number of the
 * allocated page frame is returned. Upon failure, @p FRAME_NULL
 * is returned instead.
 */
extern frame_t frame_alloc(void);

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

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_MM_FRAME_H_ */
