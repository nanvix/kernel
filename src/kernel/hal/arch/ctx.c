/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @todo TODO provide a detailed description for this function.
 */
void context_dump(const struct context *ctx)
{
    kprintf("[hal][cpu] eax=%x ebx=%x ecx=%x edx=%x",
            ctx->eax,
            ctx->ebx,
            ctx->ecx,
            ctx->edx);
    kprintf("[hal][cpu] esi=%x edi=%x ebp=%x", ctx->esi, ctx->edi, ctx->ebp);
    kprintf("[hal][cpu]  cs=%x  ds=%x", 0xff & ctx->cs, 0xff & ctx->ds);
    kprintf("[hal][cpu]  es=%x  fs=%x  gs=%x",
            0xff & ctx->es,
            0xff & ctx->fs,
            0xff & ctx->gs);
    kprintf("[hal][cpu] eip=%x eflags=%x", ctx->eip, ctx->eflags);
}

/**
 * @details Initializes an execution context.
 */
int context_create(struct context *ctx, const void *pgdir, const void *kbp,
                   const void *ksp)
{
    __memset(ctx, 0, sizeof(struct context));

    // Initialize context.
    ctx->cr3 = (word_t)pgdir;
    ctx->esp = (word_t)ksp;
    ctx->ebp = (word_t)kbp;
    ctx->esp0 = (word_t)kbp;

    return (0);
}
