/*
 * MIT License
 *
 * Copyright(c) 2018-2018 Davidson Francis <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <arch/or1k/ompic.h>
#include <nanvix/const.h>
#include <stdint.h>

/*
 * @brief Reds from a specified OMPIC register.
 *
 * @param reg Target register.
 * @return Register value.
 */
PRIVATE inline uint32_t or1k_ompic_readreg(uint32_t reg)
{
	return ( *((volatile uint32_t *) (reg)) );
}

/*
 * @brief Writes into a specified OMPIC register.
 *
 * @param reg Target register.
 * @param data Data to be written.
 */
PRIVATE inline void or1k_ompic_writereg(uint32_t reg, uint32_t data)
{
	*((volatile uint32_t *) (reg)) = data;
}

/*
 * @brief Sends an Inter-processor Interrupt.
 *
 * @param dstcore Destination core to be sent the message.
 * @param data Data to be sent.
 */
PUBLIC void or1k_ompic_send_ipi(uint32_t dstcore, uint16_t data)
{
	int coreid; /* Core ID. */
	coreid = or1k_core_get_id();

	/* Send IPI. */
	or1k_ompic_writereg(OR1K_OMPIC_CTRL(coreid), OR1K_OMPIC_CTRL_IRQ_GEN |
		OR1K_OMPIC_CTRL_DST(dstcore)| OR1K_OMPIC_DATA(data));
}

/*
 * @brief Handles to Inter-processor Interrupt here.
 *
 * @param num Dummy argument.
 */
PRIVATE void or1k_ompic_handle_ipi(int num)
{
	int coreid; /* Core ID. */

	UNUSED(num);

	/* Current core. */
	coreid = or1k_core_get_id();

	/* ACK IPI. */
	or1k_ompic_writereg(OR1K_OMPIC_CTRL(coreid), OR1K_OMPIC_CTRL_IRQ_ACK);
}

/*
 * @brief Setup the OMPIC.
 */
PUBLIC void or1k_ompic_init(void)
{
	/* IPI handler. */
	interrupt_register(OR1K_PC_INT_OMPIC, or1k_ompic_handle_ipi);
}
