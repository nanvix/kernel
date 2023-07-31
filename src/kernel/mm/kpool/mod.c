/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Variables                                                                  *
 *============================================================================*/

/**
 * @brief Reference count for kernel pages.
 */
static int kpages[NUM_KPAGES] = {
    0,
};

/*============================================================================*
 * kpage_get()                                                                *
 *============================================================================*/

/**
 * The kpage_get() function allocates a page of the kernel page pool.
 * If @p clean is non zero, the page is filled with zeros before it is
 * handled to the caller. The call fails if there are no free kernel
 * pages available in the pool.
 *
 * @retval NULL Cannot allocate a kernel page.
 *
 * @author Pedro Henrique Penna
 */
void *kpage_get(int clean)
{
    void *kpg;  /* Kernel page. */
    unsigned i; /* Loop index.  */

    /* Search for a free kernel page. */
    for (i = 0; i < NUM_KPAGES; i++) {
        /* Found it. */
        if (kpages[i] == 0)
            goto found;
    }

    kprintf("[kernel][mm] kernel page pool overflow");

    return (NULL);

found:

    /* Set page as used. */
    kpg = (void *)kpool_id_to_addr(i);
    kpages[i]++;

    /* Clean page. */
    if (clean)
        __memset(kpg, 0, PAGE_SIZE);

    return (kpg);
}

/*============================================================================*
 * kpage_put()                                                                *
 *============================================================================*/

/**
 * The kpage_put() function releases the kernel page pointed to by @p
 * kpg. If the @p kpg does not point to a valid kernel page nor it
 * reffers to a page that is currently used, the call fails.
 *
 * @retval -EVAINL Invalid kernel page.
 * @retval -1 Target kernel page not allocated.
 *
 * @author Pedro Henrique Penna
 */
int kpage_put(void *kpg)
{
    int i;

    /* Invalid address. */
    if (!kpool_is_kpage(VADDR(kpg))) {
        kprintf("mm: not a kernel page");
        return (-1);
    }

    i = kpool_addr_to_id(VADDR(kpg));

    /* Double free. */
    if (kpages[i] == 0) {
        kprintf("[kernel][mm] double free on kernel page");
        return (-1);
    }

    kpages[i]--;

    return (0);
}

/*============================================================================*
 * kpool_init()                                                               *
 *============================================================================*/

/**
 * The kpool_init() function initializes internal structures of the
 * kernel page allocator. Additionally, if the kernel is compiled
 * without the @p NDEBUG build option, unit tests on the kernel page
 * allocator are launched once its initialization is completed.
 *
 * @author Pedro Henrique Penna
 */
void kpool_init(void)
{
    kprintf("[kernel][mm] initializing the kernel page allocator");

    for (unsigned i = 0; i < NUM_KPAGES; i++) {
        kpages[i] = 0;
    }

    test_kpool();
}
