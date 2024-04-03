/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_IPC_MAILBOX_H_
#define NANVIX_KERNEL_IPC_MAILBOX_H_
/**
 * @addtogroup kernel-ipc-mailbox Mailbox
 * @ingroup kernel-ipc
 *
 * @brief Mailbox IPC
 */
/**@{*/

/*============================================================================*
 * Kernel-Public Functions                                                    *
 *============================================================================*/

/**
 * @brief Initialize the mailbox module.
 */
extern void mailbox_init(void);

/**
 * @brief Creates a mailbox.
 */
extern int do_mailbox_create(const int, const int);

/**
 * @brief Opens an existing mailbox.
 */
extern int do_mailbox_open(const int, const int);

/**
 * @brief Removes a mailbox.
 */
extern int do_mailbox_unlink(const int);

/**
 * @brief Closes a mailbox.
 */
extern int do_mailbox_close(const int);

/**
 * @brief Writes a message to a mailbox.
 */
extern int do_mailbox_write(const int, const void *, const int);

/**
 * @brief Reads a message from a mailbox.
 */
extern int do_mailbox_read(const int, void *, const int);
/*============================================================================*/

/**@}*/

#endif /* !NANVIX_KERNEL_IPC_MAILBOX_H_ */
