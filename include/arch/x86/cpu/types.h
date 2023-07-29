/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_TYPES_H_
#define ARCH_X86_CPU_TYPES_H_

#ifndef _ASM_FILE_
#include <stdint.h>
#endif /* _ASM_FILE_ */

/**
 * @name Bit-Length of CPU Types
 */
/**@{*/
#define BYTE_BIT 8   /** Byte        */
#define HWORD_BIT 16 /** Half Word   */
#define WORD_BIT 32  /** Word        */
#define DWORD_BIT 64 /** Double Word */
/**@}*/

/**
 * @name Byte-Length of CPU Types
 */
/**@{*/
#define BYTE_SIZE 1  /** Byte        */
#define HWORD_SIZE 2 /** Half Word   */
#define WORD_SIZE 4  /** Word        */
#define DWORD_SIZE 8 /** Double Word */
/**@}*/

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

#endif /* ARCH_X86_CPU_TYPES_H_ */
