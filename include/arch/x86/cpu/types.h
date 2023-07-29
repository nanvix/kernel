/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_TYPES_H_
#define ARCH_X86_CPU_TYPES_H_

/**
 * @addtogroup x86-cpu-types x86 CPU Types
 * @ingroup x86-cpu
 *
 * @brief x86 CPU Types
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#ifndef _ASM_FILE_
#include <stdint.h>
#endif /* _ASM_FILE_ */

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @name Bit-Length of CPU Types
 */
/**@{*/
#define BYTE_BIT 8   /** Number of bits in a byte.        */
#define HWORD_BIT 16 /** Number of bits in a half-word.   */
#define WORD_BIT 32  /** Number of bits in a word.        */
#define DWORD_BIT 64 /** Number of bits in a double-word. */
/**@}*/

/**
 * @name Byte-Length of CPU Types
 */
/**@{*/
#define BYTE_SIZE 1  /** Number of bytes in a byte.        */
#define HWORD_SIZE 2 /** Number of bytes in a half-word.   */
#define WORD_SIZE 4  /** Number of bytes in a word.        */
#define DWORD_SIZE 8 /** Number of bytes in a double-word. */
/**@}*/

/*============================================================================*
 * Types                                                                      *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @name Types
 */
/**@{*/
typedef uint8_t byte_t;   /** Byte        */
typedef uint16_t hword_t; /** Half Word   */
typedef uint32_t word_t;  /** Word        */
typedef uint64_t dword_t; /** Double Word */
/**@}*/

#endif /* _ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_TYPES_H_ */
