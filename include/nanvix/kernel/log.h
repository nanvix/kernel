/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_LOG_H_
#define NANVIX_KERNEL_LOG_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <stdarg.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Log Message Levels
 */
/**@{*/
#define TRACE 0 /** Trace level.       */
#define DEBUG 1 /** Debug level.       */
#define INFO 2  /** Information level. */
#define WARN 3  /** Warning level.     */
#define ERROR 4 /** Error level.       */
/**@}*/

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Prints a log message to the standard output.
 *
 * @details For the log message to be fully printed it must not contain more
 * than `LOG_BUFFER_SIZE` characters.
 *
 * @param file A string defined by the macro `__FILE__`.
 * @param funcname A string defined by the macro `__func__`.
 * @param level The log message level e.g. `INFO`.
 *
 * @return Zero for a successful completion.  Upon failure, an negative number
 * will be returned instead.
 */
extern int __log(const char *file, const char *funcname, unsigned level, ...);

/**
 * @brief Prints a custom log message to the standard output.
 *
 * @param level The log message level e.g. `INFO`.
 *
 * @deprecated use `trace()`, `debug`, `info()`, `warn()` and `error()` instead.
 */
#define log(level, ...) __log(__FILE__, __func__, level, __VA_ARGS__)

/**
 * @brief Prints a debug-level message on the standard output device.
 */
#define trace(...) log(TRACE, __VA_ARGS__)

/**
 * @brief Prints a debug-level message on the standard output device.
 */
#define debug(...) log(DEBUG, __VA_ARGS__)

/**
 * @brief Prints an information-level message on the standard output device.
 */
#define info(...) log(INFO, __VA_ARGS__)

/**
 * @brief Prints a warning-level message on the standard output device.
 */
#define warn(...) log(WARN, __VA_ARGS__)

/**
 * @brief Prints an error-level message on the standard output device.
 */
#define error(...) log(ERROR, __VA_ARGS__)

#endif /* NANVIX_KERNEL_LOG_H_ */
