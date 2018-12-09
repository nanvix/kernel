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

#include <arch/k1b/excp.h>
#include <arch/k1b/cache.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/**
 * @brief Information about exceptions.
 *
 * Lookup table with information about exceptions.
 */
PRIVATE const struct
{
	int code;           /**< Code.          */
	const char *errmsg; /**< Error message. */
} exceptions[K1B_NUM_EXCEPTION] = {
	{ K1B_EXCP_RESET,           "reset exception"                              },
	{ K1B_EXCP_OPCODE,          "bad instruction bundle"                       },
	{ K1B_EXCP_PROTECTION,      "protection fault"                             },
	{ K1B_EXCP_ALIGNMENT,       "alignment check exception"                    },
	{ K1B_EXCP_RANGE_CODE,      "instruction out of range"                     },
	{ K1B_EXCP_RANGE_DATA,      "data out of range"                            },
	{ K1B_EXCP_OPCODE,          "double ecc fault on out of range instruction" },
	{ K1B_EXCP_DOUBLE_ECC_DATA, "double ecc fault on out of range data"        },
	{ K1B_EXCP_PARITY_CODE,     "parity error on out of range instruction"     },
	{ K1B_EXCP_PARITY_DATA,     "parity error on out of range data"            },
	{ K1B_EXCP_SINGLE_ECC_CODE, "single ecc fault on out of range instruction" },
	{ K1B_EXCP_SINGLE_ECC_DATA, "single ecc fault on out of range data"        },
	{ K1B_EXCP_PAGE_FAULT,      "page fault"                                   },
	{ K1B_EXCP_PAGE_PROTECTION, "page protection"                              },
	{ K1B_EXCP_WRITE_CLEAN ,    "write to clean exception"                     },
	{ K1B_EXCP_ATOMIC_CLEAN,    "atomic to clean exception"                    }
};

/**
 * @brief Exception handlers.
 *
 * Lookup table with registered exception handlers.
 */
PRIVATE void (*k1b_excp_handlers[K1B_NUM_EXCEPTION])(int) = {
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/**
 * The do_excp() functin dispatches an exception to the registered
 * exception handler.
 */
PUBLIC void do_excp(int excpnum)
{
	/* Unknown exception. */
	if ((excpnum < 0) || excpnum >= K1B_NUM_EXCEPTION)
		kpanic("unknown exception %x\n", excpnum);

	/* Unhandled exception. */
	if (k1b_excp_handlers[excpnum] == NULL)
		kpanic("unhandled exception %s\n", exceptions[excpnum].errmsg);

	kprintf("forwarding exception");
	k1b_excp_handlers[excpnum](excpnum);
}

/**
 * The k1b_excp_set_handler() function sets a handler function for
 * the exception @p excpnum.
 *
 * @note This function does not check if a handler is already
 * set for the target hardware exception.
 */
PUBLIC void k1b_excp_set_handler(int excpnum, void (*handler)(int))
{
	k1b_excp_handlers[excpnum] = handler;
	k1b_dcache_inval();
}
