/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/cc.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/libcore.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of entries in the GDT.
 */
#define GDT_LENGTH 6

/**
 * @name GDT Entries
 */
/**@{*/
#define GDT_NULL 0      /** Null.       */
#define GDT_CODE_DPL0 1 /** Code DPL 0. */
#define GDT_DATA_DPL0 2 /** Data DPL 0. */
#define GDT_CODE_DPL3 3 /** Code DPL 3. */
#define GDT_DATA_DPL3 4 /** Data DPL 3. */
#define GDT_TSS 5       /** TSS.        */
/**@}*/

/**
 * @name GDT Segment Selectors
 */
/**@{*/
#define KERNEL_CS (GDTE_SIZE * GDT_CODE_DPL0)   /** Kernel code. */
#define KERNEL_DS (GDTE_SIZE * GDT_DATA_DPL0)   /** Kernel data. */
#define USER_CS (GDTE_SIZE * GDT_CODE_DPL3 + 3) /** User code.   */
#define USER_DS (GDTE_SIZE * GDT_DATA_DPL3 + 3) /** User data.   */
#define TSS (GDTE_SIZE * GDT_TSS)               /** TSS.         */
/**@}*/

/*============================================================================*
 * Variables                                                                  *
 *============================================================================*/

/**
 * @brief Global Descriptor Table (GDT).
 */
static struct gdte gdt[GDT_LENGTH];

/**
 * @brief Pointer to Global Descriptor Table (GDTPTR)
 */
static struct gdtptr gdtptr;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Loads the Global Descriptor Table (GDT).
 *
 * @param gdtptr Global descriptor table pointer.
 */
static void gdt_load(const struct gdtptr *_gdtptr)
{
    kprintf("[hal][cpu] loading gdt...");

    asm volatile("mov %0, %%eax;\
			lgdt (%%eax);\
			ljmp %1, $reload_cs;\
			reload_cs:\
			movw %2, %%ax;\
			movw %%ax, %%ds;\
			movw %%ax, %%es;\
			movw %%ax, %%fs;\
			movw %%ax, %%gs;\
            movw %%ax, %%ss;"
                 :
                 : "r"(_gdtptr), "N"(KERNEL_CS), "N"(KERNEL_DS)
                 : "memory", "eax");
}

/**
 * @brief Sets an entry of the GDT.
 *
 * @param n           Target entry.
 * @param base        Base address of segment.
 * @param limit       Limit (size) of the segment.
 * @param granularity Granularity of segment.
 * @param access      Access permissions.
 */
static void set_gdte(int n, unsigned base, unsigned limit, unsigned granularity,
                     unsigned access)
{
    // Set the base address of the segment.
    gdt[n].base_low = (base & 0xffffff);
    gdt[n].base_high = (base >> 24) & 0xff;

    // Set the limit of the segment.
    gdt[n].limit_low = (limit & 0xffff);
    gdt[n].limit_high = (limit >> 16) & 0xf;

    // Set the granularity of the segment.
    gdt[n].granularity = granularity;
    gdt[n].access = access;
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Returns the segment selector of the kernel code segment.
 */
unsigned gdt_kernel_cs(void)
{
    return (KERNEL_CS);
}

/**
 * @details Initializes the Global Descriptor Table (GDT).
 */
void gdt_init(void)
{
    kprintf("[hal][cpu] initializing gdt...");

    // Ensure that size of structures match what we expect.
    KASSERT_SIZE(sizeof(struct gdte), GDTE_SIZE);
    KASSERT_SIZE(sizeof(struct gdtptr), GDTPTR_SIZE);

    // Initialize the TSS.
    const struct tss *tss = tss_init(KERNEL_DS);

    // Blank the GDT and GDTPTR structures.
    __memset(gdt, 0, sizeof(gdt));
    __memset(&gdtptr, 0, GDTPTR_SIZE);

    // Initialize GDT structure.
    set_gdte(GDT_NULL, 0, 0x00000, 0x0, 0x00);
    set_gdte(GDT_CODE_DPL0, 0, 0xfffff, 0xc, 0x9a);
    set_gdte(GDT_DATA_DPL0, 0, 0xfffff, 0xc, 0x92);
    set_gdte(GDT_CODE_DPL3, 0, 0xfffff, 0xc, 0xfa);
    set_gdte(GDT_DATA_DPL3, 0, 0xfffff, 0xc, 0xf2);
    set_gdte(GDT_TSS, (unsigned)tss, TSS_SIZE, 0x00, 0x89);

    // Initialize the GDTPTR structure.
    gdtptr.size = sizeof(gdt) - 1;
    gdtptr.ptr = (unsigned)gdt;

    // Load the GDT.
    gdt_load(&gdtptr);

    // Load the TSS.
    tss_load(TSS);
}
