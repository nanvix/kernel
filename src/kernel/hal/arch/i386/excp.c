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

#include <arch/i386/excp.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/**
 * @brief Information about exceptions.
 *
 * Lookup table with information about exceptions.
 */
PRIVATE const struct
{
	int excpnum;        /**< ID             */
	const char *errmsg; /**< Error message. */
} exceptions[I386_NUM_EXCEPTION] = {
	{ I386_EXCP_DIVIDE,                      "division-by-zero error"        },
	{ I386_EXCP_DEBUG,                       "debug exception"               },
	{ I386_EXCP_NMI,                         "non-maskable interrupt"        },
	{ I386_EXCP_BREAKPOINT,                  "breakpoint exception"          },
	{ I386_EXCP_OVERFLOW,                    "overflow exception"            },
	{ I386_EXCP_BOUNDS,                      "bounds check exception"        },
	{ I386_EXCP_INVALID_OPCODE,              "invalid opcode exception"      },
	{ I386_EXCP_COPROCESSOR_NOT_AVAILABLE,   "coprocessor not available"     },
	{ I386_EXCP_DOUBLE_FAULT,                "double fault"                  },
	{ I386_EXCP_COPROCESSOR_SEGMENT_OVERRUN, "coprocessor segment overrun"   },
	{ I386_EXCP_INVALID_TSS,                 "invalid task state segment"    },
	{ I386_EXCP_SEGMENT_NOT_PRESENT,         "segment not present"           },
	{ I386_EXCP_STACK_SEGMENT_FAULT,         "static segment fault"          },
	{ I386_EXCP_GENERAL_PROTECTION,          "general protection fault"      },
	{ I386_EXCP_PAGE_FAULT,                  "page fault"                    },
	{ I386_EXCP_FPU_ERROR,                   "floating point unit exception" },
	{ I386_EXCP_ALIGNMENT_CHECK,             "alignment check"               },
	{ I386_EXCP_MACHINE_CHECK,               "machine exception"             },
	{ I386_EXCP_SIMD_ERROR,                  "smid unit exception"           },
	{ I386_EXCP_VIRTUAL_EXCEPTION,           "virtual exception"             },
	{ I386_EXCP_SECURITY_EXCEPTION,          "security exception"            }
};

/**
 * @brief Exception handlers.
 *
 * Lookup table with registered exception handlers.
 */
PRIVATE void (*i386_excp_handlers[I386_NUM_EXCEPTION])(int) = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/**
 * @brief Generic exception handler.
 *
 * @param excpnum Exception number.
 */
PRIVATE void generic_excp_handler(int excpnum)
{
	kpanic("%s", exceptions[excpnum].errmsg);
}

/**
 * @brief Exception dispatcher.
 *
 * @param ctx     Interrupted context.
 * @param excpnum Exception number.
 * @param addr    Exception address.
 * @param errcode Error code
 *
 * @note This function is called from assembly code.
 */
PUBLIC void do_excp(void *ctx, int excpnum, unsigned addr, int err)
{
	kprintf("%x %x %x %x", ctx, excpnum, addr, err);

	/* Nothing to do. */
	if (i386_excp_handlers[excpnum] == NULL)
		generic_excp_handler(excpnum);

	i386_excp_handlers[excpnum](excpnum);
}

/**
 * The i386_excp_set_handler() function sets a handler function for
 * the exception @p excpnum.
 *
 * @note This function does not check if a handler is already
 * set for the target hardware exception.
 */
PUBLIC void i386_excp_set_handler(int excpnum, void (*handler)(int))
{
	i386_excp_handlers[excpnum] = handler;
}
