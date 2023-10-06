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
 * @brief Initializes an execution context.
 *
 */
void context_create(struct context *ctx, const void *pgdir, void *stack,
                    void (*func)(void))
{
    __memset(ctx, 0, sizeof(struct context));
    __memset(stack, 0, PAGE_SIZE);

    // TODO: review this once user mode is working.
    word_t *stackp = (word_t *)((word_t)stack + PAGE_SIZE);
    *--stackp = EFLAGS_IF;              /* eflags      */
    *--stackp = gdt_kernel_cs();        /* cs          */
    *--stackp = (word_t)func;           /* user eip    */
    *--stackp = (word_t)__leave_kernel; /* kernel eip  */

    ctx->cr3 = (word_t)pgdir;
    ctx->esp = (word_t)stackp;
    ctx->ebp = (word_t)stackp;
    ctx->eflags = 0x00;
}
