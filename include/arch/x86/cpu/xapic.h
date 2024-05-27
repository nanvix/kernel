/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef X86_CPU_XAPIC_H_
#define X86_CPU_XAPIC_H_

/**
 * @addtogroup x86-cpu-xapic xAPIC
 * @ingroup x86
 *
 * @brief xAPIC
 */
/**@{*/

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @details Acknowledges interrupt request.
 *
 * @param irq Number of the interrupt to acknowledge.
 */
extern void xapic_ack(int irq);

/**
 * @brief Initializes the xAPIC.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int xapic_init(void);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* X86_CPU_XAPIC_H_ */
