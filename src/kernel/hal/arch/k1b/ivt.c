/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#include <arch/cluster/k1b/cpu.h>
#include <nanvix/const.h>

/**
 * @brief Kernel stack.
 */
PRIVATE uint64_t kstack[K1B_NUM_CORES][K1B_PAGE_SIZE/sizeof(uint64_t)] ALIGN(K1B_CACHE_LINE_SIZE);

/**
 * Lookup table that maps hardware interrupt IDs into numbers.
 */
PUBLIC const k1b_hwint_id_t hwints[K1B_NUM_HWINT] = {
	BSP_IT_TIMER_0, /* Clock 0              */
	BSP_IT_TIMER_1, /* Clock 1              */
	BSP_IT_WDOG,    /* Watchdog Timer       */
	BSP_IT_CN,      /* Control NoC          */
	BSP_IT_RX,      /* Data NoC             */
	BSP_IT_UC,      /* DMA                  */
	BSP_IT_NE,      /* NoC Error            */
	BSP_IT_WDOG_U,  /* Watchdog Timer Error */
	BSP_IT_PE_0,    /* Remote Core 0        */
	BSP_IT_PE_1,    /* Remote Core 1        */
	BSP_IT_PE_2,    /* Remote Core 2        */
	BSP_IT_PE_3,    /* Remote Core 3        */
	BSP_IT_PE_4,    /* Remote Core 4        */
	BSP_IT_PE_5,    /* Remote Core 5        */
	BSP_IT_PE_6,    /* Remote Core 6        */
	BSP_IT_PE_7,    /* Remote Core 7        */
	BSP_IT_PE_8,    /* Remote Core 8        */
	BSP_IT_PE_9,    /* Remote Core 9        */
	BSP_IT_PE_10,   /* Remote Core 10       */
	BSP_IT_PE_11,   /* Remote Core 11       */
	BSP_IT_PE_12,   /* Remote Core 12       */
	BSP_IT_PE_13,   /* Remote Core 14       */
	BSP_IT_PE_14,   /* Remote Core 14       */
	BSP_IT_PE_15,   /* Remote Core 15       */
};

/**
 * The k1b_ivt_setup() function initializes the interrupt vector table
 * in the k1b architecture. It traverses all entries of this table and
 * properly registers @p do_hwintm @p do_swint and do_excp as default
 * handlers for hardware interrupts, software interrupts and
 * exceptions, respectively.
 */
PUBLIC void k1b_ivt_setup(
	k1b_hwint_handler_fn hwint_handler,
	k1b_swint_handler_fn swint_handler,
	k1b_excp_handler_fn excp_handler)
{
	for (int i = 0; i < K1B_NUM_HWINT; i++)
		bsp_register_it(hwint_handler, hwints[i]);
	mOS_register_scall_handler(swint_handler); 
	mOS_register_trap_handler(excp_handler);

	mOS_register_stack_handler(kstack[k1b_core_get_id()]);
	mOS_trap_enable_shadow_stack();
	kprintf("[hal] exception stack at %x", kstack[k1b_core_get_id()]);

	k1b_pic_setup();
}
