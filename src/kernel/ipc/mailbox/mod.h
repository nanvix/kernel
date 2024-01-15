/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef __MAILBOX_H_
#define __MAILBOX_H_

/*============================================================================*
 * Imports                                                                   *
 *============================================================================*/

#include <nanvix/kernel/pm.h>
#include <stdbool.h>
#include <stddef.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum number of mailboxes.
 */
#define MAILBOX_MAX 256

/**
 * @brief Number of messages that can be stored in a mailbox.
 */
#define MAILBOX_SIZE 16

/**
 * @brief Maximum size of a message.
 */
#define MAILBOX_MESSAGE_SIZE 64

/*============================================================================*
 * Module-Private Functions                                                   *
 *============================================================================*/

/**
 * @brief Gets the owner of a mailbox.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @returns Upon successful completion, the PID of the owner of the target
 * mailbox is returned. Upon failure, a negative error code is returned
 * instead.
 */
extern pid_t mailbox_owner(const int mbxid);

/**
 * @brief Gets the tag of a mailbox.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @returns Upon successful completion, the tag of the target mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
extern int mailbox_tag(const int mbxid);

#endif /* __MAILBOX_H_ */
