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
 *
 * @param owner PID of the processing owning the target mailbox.
 * @param tag Tag of the target mailbox.
 *
 * @returns Upon successful completion, the ID of the target open mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
extern int do_mailbox_create(const int owner, const int tag);

/**
 * @brief Opens an existing mailbox.
 *
 * @param owner PID of the processing owning the target mailbox.
 * @param tag Tag of the target mailbox.
 *
 * @returns Upon successful completion, the ID of the target mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
extern int do_mailbox_open(const int owner, const int tag);

/**
 * @brief Removes a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int do_mailbox_unlink(const int ombxid);

/**
 * @brief Closes a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int do_mailbox_close(const int ombxid);

/**
 * @brief Writes a message to a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 * @param buffer Buffer where the data should be read from.
 * @param sz Number of bytes to write.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int do_mailbox_write(const int ombxid, const void *buffer, const int sz);

/**
 * @brief Reads a message from a mailbox.
 *
 * @param ombxid ID of the target mailbox.
 * @param buffer Buffer where the data should be written to.
 * @param sz Number of bytes to read.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int do_mailbox_read(const int ombxid, void *buffer, const int sz);

/*============================================================================*/

/**@}*/

#endif /* !NANVIX_KERNEL_IPC_MAILBOX_H_ */
