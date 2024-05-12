/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>
#include <nanvix/libcore.h>
#include <stddef.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of log message levels.
 */
#define LOG_LEVEL_MAX 5

/**
 * @brief Log module scope.
 */
#define LOG_MODULE_SCOPE "[kernel]"

/**
 * @brief Message levels.
 */
static const char *MSG_LEVEL[LOG_LEVEL_MAX] = {
    "[TRACE]", "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]"};

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Message buffer.
 */
static char buffer[KBUFFER_SIZE + 1];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Checks if the buffer can fit `ins_size` characters,
 * if not prints the current filled buffer.
 *
 * @param buf_pos The next position to be inserted in the buffer.
 * @param ins_size The number of characters to be inserted in the buffer.
 *
 * @return The next position to be inserted in the buffer.
 *
 */
static size_t chkbuf(size_t buf_pos, size_t ins_size)
{
    if (buf_pos > KBUFFER_SIZE) {
        buffer[KBUFFER_SIZE] = '\0';
        kputs(buffer);
        buf_pos = 0;
    } else if (buf_pos + ins_size > KBUFFER_SIZE) {
        buffer[buf_pos] = '\0';
        kputs(buffer);
        buf_pos = 0;
    }

    return (buf_pos);
}

/**
 * @brief Copies a string content to the buffer (without `\0`).
 *
 * @details The fuction calls `chkbuf()`,
 * thus it shall print the buffer to the standard output if the buffer is full.
 *
 * @param s The source string to be copied.
 * @param i The position within the buffer to be inserted.
 *
 * @return The next position to be inserted in the buffer.
 *
 */
static size_t cpy2buf(const char *s, size_t i)
{
    for (size_t j = 0; s[j] != '\0'; j++) {
        i = chkbuf(i, 1);
        buffer[i++] = s[j];
    }

    return (i);
}

/**
 * @brief Inserts module name and message level in the log message.
 *
 * @param file A string defined by the macro `__FILE__`.
 * @param funcname A string defined by the macro `__func__`.
 * @param level The log message level.
 *
 * @return The next position to be inserted in the buffer.
 */
static size_t fmt_prefix(const char *file, const char *funcname, int level)
{
    size_t i = 0;

    // Inserts message level.
    i = cpy2buf(MSG_LEVEL[level], i);

    // Inserts module name.
    i = cpy2buf(LOG_MODULE_SCOPE, i);
    buffer[i++] = '[';
    for (size_t j = 0; file[j + 2] != '\0'; j++) {
        i = chkbuf(i, 1);
        if (file[j] == '/') {
            buffer[i++] = ']';
            buffer[i++] = '[';
        } else {
            buffer[i++] = file[j];
        }
    }
    buffer[i++] = ']';

    // Inserts function name.
    i = chkbuf(i, __strlen(funcname) + 6);
    buffer[i++] = ' ';
    i = cpy2buf(funcname, i);
    i = cpy2buf("(): ", i);

    return (i);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Prints a log message to the standard output.
 */
int __log(const char *file, const char *funcname, unsigned level, ...)
{
    // Validades arguments.
    if (file == NULL || funcname == NULL || level >= LOG_LEVEL_MAX) {
        return -1;
    }

    va_list args;                                 // Variable arguments list
    size_t i = fmt_prefix(file, funcname, level); // String length.

    va_start(args, level);

    // Print message into the buffer.
    const char *fmt_str = va_arg(args, char *);
    i = chkbuf(i, __strlen(fmt_str));
    i += __vsnprintf(buffer + i, KBUFFER_SIZE - (i + 1), fmt_str, args);

    // Terminate string.
    buffer[++i] = '\n';
    buffer[++i] = '\0';

    va_end(args);
    kputs(buffer);

    return (0);
}
