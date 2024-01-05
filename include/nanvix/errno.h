/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_ERRNO_H_
#define NANVIX_ERRNO_H_

/**
 * @name Error Codes
 */
/**@{*/
#define E2BIG 1            /** Arg list too long.                   */
#define EACCES 2           /** Permission denied.                   */
#define EADDRINUSE 3       /** Address already in use.              */
#define EADDRNOTAVAIL 4    /** Address not available.               */
#define EAFNOSUPPORT 5     /** Address family not supported.        */
#define EAGAIN 6           /** No more processes.                   */
#define EALREADY 7         /** Socket already connected.            */
#define EBADF 8            /** Bad file number.                     */
#define EBADMSG 9          /** Bad message.                         */
#define EBUSY 10           /** Device or resource busy.             */
#define ECANCELED 11       /** Operation canceled.                  */
#define ECHILD 12          /** No children.                         */
#define ECONNABORTED 13    /** Software caused connection abort.    */
#define ECONNREFUSED 14    /** Connection refused.                  */
#define ECONNRESET 15      /** Connection reset by peer.            */
#define EDEADLK 16         /** Deadlock.                            */
#define EDESTADDRREQ 17    /** Destination address required.        */
#define EDOM 18            /** Math argument out of domain of func. */
#define EDQUOT 19          /** Quota exceeded.                      */
#define EEXIST 20          /** File exists.                         */
#define EFAULT 21          /** Bad address.                         */
#define EFBIG 22           /** File too large.                      */
#define EHOSTUNREACH 23    /** Host is unreachable.                 */
#define EIDRM 24           /** Identifier removed.                  */
#define EILSEQ 25          /** Illegal byte sequence.               */
#define EINPROGRESS 26     /** Connection already in progress.      */
#define EINTR 27           /** Interrupted system call.             */
#define EINVAL 28          /** Invalid argument.                    */
#define EIO 29             /** I/O error.                           */
#define EISCONN 30         /** Socket is already connected.         */
#define EISDIR 31          /** Is a directory.                      */
#define ELOOP 32           /** Too many symbolic links.             */
#define EMFILE 33          /** File descriptor value too large.     */
#define EMLINK 34          /** Too many links.                      */
#define EMSGSIZE 35        /** Message too long.                    */
#define EMULTIHOP 36       /** Multihop attempted.                  */
#define ENAMETOOLONG 37    /** File or path name too long.          */
#define ENETDOWN 38        /** Network interface is not configured. */
#define ENETRESET 39       /** Connection aborted by network.       */
#define ENETUNREACH 40     /** Network is unreachable.              */
#define ENFILE 41          /** Too many open files in system.       */
#define ENOBUFS 42         /** No buffer space available.           */
#define ENODEV 43          /** No such device.                      */
#define ENOENT 44          /** No such file or directory.           */
#define ENOEXEC 45         /** Exec format error.                   */
#define ENOLCK 46          /** No lock.                             */
#define ENOLINK 47         /** Virtual circuit is gone.             */
#define ENOMEM 48          /** Not enough space.                    */
#define ENOMSG 49          /** No message of desired type.          */
#define ENOPROTOOPT 50     /** Protocol not available.              */
#define ENOSPC 51          /** No space left on device.             */
#define ENOSYS 52          /** Function not implemented.            */
#define ENOTCONN 53        /** Socket is not connected.             */
#define ENOTDIR 54         /** Not a directory.                     */
#define ENOTEMPTY 55       /** Directory not empty.                 */
#define ENOTRECOVERABLE 56 /** State not recoverable.               */
#define ENOTSOCK 57        /** Socket operation on non-socket.      */
#define ENOTSUP 58         /** Not supported.                       */
#define ENOTTY 59          /** Not a character device.              */
#define ENXIO 60           /** No such device or address.           */
#define EOPNOTSUPP 61      /** Operation not supported on socket.   */
#define EOVERFLOW 62       /** Value too large for data type.       */
#define EOWNERDEAD 63      /** Previous owner died.                 */
#define EPERM 64           /** Not owner.                           */
#define EPIPE 65           /** Broken pipe.                         */
#define EPROTO 66          /** Protocol error.                      */
#define EPROTONOSUPPORT 67 /** Unknown protocol.                    */
#define EPROTOTYPE 68      /** Protocol wrong type for socket.      */
#define ERANGE 69          /** Result too large.                    */
#define EROFS 70           /** Read-only file system.               */
#define ESPIPE 71          /** Illegal seek.                        */
#define ESRCH 72           /** No such process.                     */
#define ESTALE 73          /** Stale NFS file handle.               */
#define ETIMEDOUT 74       /** Connection timed out.                */
/**@}*/

#endif /* NANVIX_ERRNO_H_ */
