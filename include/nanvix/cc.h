/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_CC_H_
#define NANVIX_CC_H_

/*============================================================================*
 * Aliases                                                                    *
 *============================================================================*/

/**
 * @name Aliases for C Extensions
 */
/**@{*/
#define asm __asm__
/**@}*/

/*============================================================================*
 * Builtin Functions                                                          *
 *============================================================================*/

/**
 * @brief Makes code unreachable.
 */
#define UNREACHABLE()                                                          \
    {                                                                          \
        while (1)                                                              \
            ;                                                                  \
        __builtin_unreachable();                                               \
    }

/**
 * @brief Declares something as unused.
 *
 * @param x Thing.
 */
#define UNUSED(x) ((void)(x))

/**
 * @brief Hints a likely condition.
 *
 * @param expr Target expression.
 */
#define LIKELY(expr) __builtin_expect(expr, 1)

/**
 * @brief Hints an unlikely condition.
 *
 * @param expr Target expression.
 */
#define UNLIKELY(expr) __builtin_expect(expr, 0)

/**
 * @brief No operation.
 */
#define noop() __asm__ __volatile__("" ::: "memory")

#endif /* NANVIX_CC_H_ */
