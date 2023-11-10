/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_MEMORY_H_
#define NANVIX_KERNEL_HAL_MEMORY_H_

#ifndef _ASM_FILE_
#include <stdbool.h>

/**
 * @name Binary Sections
 */
/**@{*/
extern unsigned char __KERNEL_START;    /** Kernel Start          */
extern unsigned char __KERNEL_END;      /** Kernel End            */
extern unsigned char __BOOTSTRAP_START; /** Bootstrap Start       */
extern unsigned char __BOOTSTRAP_END;   /** Bootstrap End         */
extern unsigned char __TEXT_START;      /** Text Start            */
extern unsigned char __TEXT_END;        /** Text End              */
extern unsigned char __DATA_START;      /** Data Start            */
extern unsigned char __DATA_END;        /** Data End              */
extern unsigned char __BSS_START;       /** BSS Start             */
extern unsigned char __BSS_END;         /** BSS End               */
extern unsigned char __RODATA_START;    /** Ready-Only Data Start */
extern unsigned char __RODATA_END;      /** Ready-Only Data End   */
/**@}*/

/*============================================================================*
 * Variables                                                                  *
 *============================================================================*/

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_MEMORY_H_ */
