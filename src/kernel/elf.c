/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <elf.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <stdbool.h>
#include <stdint.h>

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Asserts if the target ELF header is valid.
 *
 * @param header Target ELF header.
 *
 * @returns True if the target ELF header is valid, and false otherwise.
 */
static bool elf32_fhdr_is_valid(const struct elf32_fhdr *header)
{
    // Check if header is a valid pointer.
    if (header == NULL) {
        kprintf("ERROR: header is NULL");
        return (false);
    }

    // Check ELF magic value.
    if ((header->e_ident[0] != ELFMAG0) || (header->e_ident[1] != ELFMAG1) ||
        (header->e_ident[2] != ELFMAG2) || (header->e_ident[3] != ELFMAG3))
        return (false);

    return (true);
}

/**
 * @brief Gets the string representation of an ELF program header type.
 *
 * @param type Numeric representation for the type.
 *
 * @return A string representation for the type.
 */
static const char *elf32_phdr_get_type_str(uint32_t type)
{
    switch (type) {
        case PT_NULL:
            return "PT_NULL";
        case PT_LOAD:
            return "PT_LOAD";
        case PT_DYNAMIC:
            return "PT_DYNAMIC";
        case PT_INTERP:
            return "PT_INTERP";
        case PT_NOTE:
            return "PT_NOTE";
        case PT_SHLIB:
            return "PT_SHLIB";
        case PT_PHDR:
            return "PT_PHDR";
        default:
            return "PT_UNKNOWN";
    }
}

/**
 * @brief Gets the string representation of an ELF program header flags.
 *
 * @param flags Numeric representation for the flags.
 *
 * @return A string representation for the flags.
 */
static const char *elf32_phdr_get_flags_str(uint32_t flags)
{
    switch (flags) {
        case PF_X:
            return "--x";
        case PF_W:
            return "-w-";
        case PF_R:
            return "r---";
        case PF_X | PF_W:
            return "-wx";
        case PF_X | PF_R:
            return "r-x";
        case PF_W | PF_R:
            return "rw-";
        case PF_X | PF_W | PF_R:
            return "rwx";
        default:
            return "---";
    }
}

/**
 * @brief Prints an ELF program header.
 *
 * @param phdr Target ELF program header.
 */
static void elf32_phdr_print(const struct elf32_phdr *phdr)
{
    // Check if program header is valid.
    if (phdr == NULL) {
        kprintf("ERROR: program header is NULL");
        return;
    }

    // Get string representation for type.
    const char *type = elf32_phdr_get_type_str(phdr->p_type);

    // Get stirng representation for flags.
    const char *flags = elf32_phdr_get_flags_str(phdr->p_flags);

    // Print program header.
    kprintf(
        "INFO: type=%s, offset=%d, vaddr=%x, paddr=%x, filesz=%d, memsz=%d, "
        "flags=%s, align=%d",
        type,
        phdr->p_offset,
        phdr->p_vaddr,
        phdr->p_paddr,
        phdr->p_filesz,
        phdr->p_memsz,
        flags,
        phdr->p_align);
}

/**
 * @brief Loads an ELF 32 executable.
 *
 * @param elf Target ELF file.
 * @param dry_run If true, the ELF file is not loaded.
 *
 * @returns Upon successful completion, the entry point of the ELF file is
 * returned. On failure, zero is returned instead.
 */
static vaddr_t do_elf32_load(const struct elf32_fhdr *elf, bool dry_run)
{
    /* Bad ELF file. */
    if (!elf32_fhdr_is_valid(elf)) {
        kprintf("ERROR: invalid ELF file");
        return (0);
    }

    const struct elf32_phdr *phdr =
        (struct elf32_phdr *)((char *)elf + elf->e_phoff);

    /* Load segments. */
    for (unsigned i = 0; i < elf->e_phnum; i++) {
        bool w = false;
        bool x = false;

        // Check if segment is loadable.
        if (phdr[i].p_type != PT_LOAD) {
            // It is not, thus skip it.
            continue;
        }

        // Check if segment is valid.
        if (phdr[i].p_filesz > phdr[i].p_memsz) {
            kprintf("ERROR: broken ELF file");
            return (0);
        }

        const vaddr_t addr = ALIGN(phdr[i].p_vaddr, phdr[i].p_align);

        /* Text section. */
        // Check if segment is executable.
        if (!(phdr[i].p_flags ^ (PF_R | PF_X))) {
            x = true;
        }

        // Check if segment is writable.
        else if (!(phdr[i].p_flags ^ (PF_R | PF_W))) {
            w = true;
        }

        // Print program header.
        if (dry_run) {
            elf32_phdr_print(&phdr[i]);
        }

        paddr_t pbase = PADDR((char *)elf + phdr[i].p_offset);
        const paddr_t pend = ALIGN(pbase + phdr[i].p_filesz, PAGE_SIZE);
        for (vaddr_t vbase = VADDR(addr); pbase <= pend;
             vbase += PAGE_SIZE, pbase += PAGE_SIZE) {
            if (vbase < USER_BASE_VIRT) {
                kprintf("ERROR: invalid load address");
                return (0);
            }

            // Check if we are running on dry mode.
            if (!dry_run) {
                // We are not, thus load segment.
                const struct process *curr = process_get_curr();
                int result = vmem_map(
                    curr->vmem, vbase, pbase >> PAGE_SHIFT, PAGE_SIZE, w, x);

                // FIXME: rollback instead of panicking.
                KASSERT(result == 0);
            }
        }
    }

    return (elf->e_entry);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Loads an ELF 32 executable.
 */
vaddr_t elf32_load(const struct elf32_fhdr *elf)
{
    if (do_elf32_load(elf, true) == 0) {
        return (0);
    }

    return (do_elf32_load(elf, false));
}
