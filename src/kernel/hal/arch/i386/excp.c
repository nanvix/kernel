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

#include <arch/i386/context.h>
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
	int num;            /**< Number.        */
	const char *errmsg; /**< Error message. */
} exceptions[I386_NUM_EXCEPTIONS] = {
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
PRIVATE void (*i386_excp_handlers[I386_NUM_EXCEPTIONS])(const struct exception *, const struct context *) = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/**
 * @brief Generic exception handler.
 *
 * @param excp Exception information.
 * @param ctx  Interrupted context.
 */
PRIVATE void generic_excp_handler(const struct exception *excp, const struct context *ctx)
{
	/* Dump execution context. */
	kprintf("[i386] eax=%x ebx=%x ecx=%x edx=%x", ctx->eax, ctx->ebx, ctx->ecx, ctx->edx);
	kprintf("[i386] esi=%x edi=%x ebp=%x esp=%x", ctx->esi, ctx->edi, ctx->ebp, ctx->useresp);
	kprintf("[i386]  cs=%x  ds=%x  ss=%x", 0xff & ctx->cs, 0xff & ctx->ds, 0xff & ctx->ss);
	kprintf("[i386]  es=%x  fs=%x  gs=%x", 0xff & ctx->es, 0xff & ctx->fs, 0xff & ctx->gs);
	kprintf("[i386] eip=%x eflags=%x", ctx->eip, ctx->eflags);

	/* Dump exception information. */
	kpanic("%s", exceptions[excp->num].errmsg);
}

/**
 * @brief High-level exception dispatcher.
 *
 * @param excp Exception information.
 * @param ctx  Interrupted context.
 *
 * @note This function is called from assembly code.
 */
PUBLIC void do_excp(const struct exception *excp, const struct context *ctx)
{
	/* Nothing to do. */
	if (i386_excp_handlers[excp->num] == NULL)
		generic_excp_handler(excp, ctx);

	kprintf("forwarding exception");

	i386_excp_handlers[excp->num](excp, ctx);
}

/**
 * The i386_excp_set_handler() function sets a handler function for
 * the exception @p num.
 *
 * @note This function does not check if a handler is already set for
 * the target hardware exception.
 */
PUBLIC void i386_excp_set_handler(
	int num,
	void (*handler)(const struct exception *, const struct context *)
)
{
	i386_excp_handlers[num] = handler;
}
