/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_TYPES_H_
#define NANVIX_TYPES_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <stdint.h>

/*============================================================================*
 * Macros                                                                     *
 *============================================================================*/

/**
 * @brief Access mode bits.
 */
/**@{*/
#define S_IRWXU 0700  /** Read, write, execute/search by owner.     */
#define S_IRUSR 0400  /** Read permission, owner.                   */
#define S_IWUSR 0200  /** Write permission, owner.                  */
#define S_IXUSR 0100  /** Execute/search permission, owner.         */
#define S_IRWXG 070   /** Read, write, execute/search by group.     */
#define S_IRGRP 040   /** Read permission, group.                   */
#define S_IWGRP 020   /** Write permission, group.                  */
#define S_IXGRP 010   /** Execute/search permission, group.         */
#define S_IRWXO 07    /** Read, write, execute/search by others.    */
#define S_IROTH 04    /** Read permission, others.                  */
#define S_IWOTH 02    /** Write permission, others.                 */
#define S_IXOTH 01    /** Execute/search permission, others.        */
#define S_ISUID 04000 /** Set-user-ID on execution.                 */
#define S_ISGID 02000 /** Set-group-ID on execution.                */
#define S_ISVTX 01000 /** On directories, restricted deletion flag. */
/**@}*/

/*============================================================================*
 * Types                                                                      *
 *============================================================================*/

/**
 * @brief Access mode permissions.
 */
typedef uint32_t mode_t;

/**
 * @brief User group ID.
 */
typedef int gid_t;

/**
 * @brief Process ID.
 */
typedef int pid_t;

/**
 * @brief Thread ID.
 */
typedef int tid_t;

/**
 * @brief User ID.
 */
typedef int uid_t;

#endif /* !NANVIX_TYPES_H_ */
