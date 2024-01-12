/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <stdint.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Module name.
 *
 */
#define MODULE_NAME "[hal][timer]"

/**
 * @brief Oscillator frequency (in Hz)
 */
#define PIT_FREQUENCY 1193182

/**
 * @name Registers
 */
/**@{*/
#define PIT_CTRL 0x43 /** Control */
#define PIT_DATA 0x40 /** Data    */
/**@}*/

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function initializes the timer device. The frequency of the
 * device is set to @p freq Hz.
 */
void timer_init(unsigned freq)
{
    const uint16_t freq_divisor = PIT_FREQUENCY / freq;

    kprintf(MODULE_NAME " initializing timer...");

    // Send control byte: adjust frequency divisor.
    kprintf(MODULE_NAME " setting frequency to %d Hz", freq);
    output8(PIT_CTRL, 0x36);

    // Send data byte: divisor_low and divisor_high.
    output8(PIT_DATA, (uint8_t)(freq_divisor & 0xff));
    output8(PIT_DATA, (uint8_t)((freq_divisor >> 8)));
}
