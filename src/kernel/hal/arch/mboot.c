/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <grub/mboot2.h>
#include <nanvix/kernel/lib.h>

/**
 * @details Prints Multiboot information.
 */
void mboot_print(unsigned magic, unsigned long addr)
{
    struct mboot_tag *tag;
    unsigned size;

    /*  Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        kprintf("Invalid magic number: %x", (unsigned)magic);
        return;
    }

    if (addr & 7) {
        kprintf("Unaligned mbi: %x", addr);
        return;
    }

    size = *(unsigned *)addr;
    kprintf("Announced mbi size %x", size);
    for (tag = (struct mboot_tag *)(addr + 8); tag->type != MBOOT_TAG_TYPE_END;
         tag = (struct mboot_tag *)((mboot_uint8_t *)tag +
                                    ((tag->size + 7) & ~7))) {
        kprintf("Tag %x, Size %x", tag->type, tag->size);
        switch (tag->type) {
        case MBOOT_TAG_TYPE_CMDLINE:
            kprintf("Command line = %s",
                    ((struct mboot_tag_string *)tag)->string);
            break;
        case MBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            kprintf("Boot loader name = %s",
                    ((struct mboot_tag_string *)tag)->string);
            break;
        case MBOOT_TAG_TYPE_MODULE:
            kprintf("Module at %x-%x. Command line %s",
                    ((struct mboot_tag_module *)tag)->mod_start,
                    ((struct mboot_tag_module *)tag)->mod_end,
                    ((struct mboot_tag_module *)tag)->cmdline);
            break;
        case MBOOT_TAG_TYPE_BASIC_MEMINFO:
            kprintf("mem_lower = %dKB, mem_upper = %dKB",
                    ((struct mboot_tag_basic_meminfo *)tag)->mem_lower,
                    ((struct mboot_tag_basic_meminfo *)tag)->mem_upper);
            break;
        case MBOOT_TAG_TYPE_BOOTDEV:
            kprintf("Boot device %x,%d,%d",
                    ((struct mboot_tag_bootdev *)tag)->biosdev,
                    ((struct mboot_tag_bootdev *)tag)->slice,
                    ((struct mboot_tag_bootdev *)tag)->part);
            break;
        case MBOOT_TAG_TYPE_MMAP: {
            mboot_memory_map_t *mmap;

            kprintf("mmap");

            for (mmap = ((struct mboot_tag_mmap *)tag)->entries;
                 (mboot_uint8_t *)mmap < (mboot_uint8_t *)tag + tag->size;
                 mmap = (mboot_memory_map_t *)((unsigned long)mmap +
                                               ((struct mboot_tag_mmap *)tag)
                                                   ->entry_size))
                kprintf(" base_addr = %x%x,"
                        " length = %x%x, type = %x",
                        (unsigned)(mmap->addr >> 32),
                        (unsigned)(mmap->addr & 0xffffffff),
                        (unsigned)(mmap->len >> 32),
                        (unsigned)(mmap->len & 0xffffffff),
                        (unsigned)mmap->type);
        } break;
        case MBOOT_TAG_TYPE_FRAMEBUFFER: {
            mboot_uint32_t color;
            unsigned i;
            struct mboot_tag_framebuffer *tagfb =
                (struct mboot_tag_framebuffer *)tag;
            void *fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;

            switch (tagfb->common.framebuffer_type) {
            case MBOOT_FRAMEBUFFER_TYPE_INDEXED: {
                unsigned best_distance, distance;
                struct mboot_color *palette;

                palette = tagfb->framebuffer_palette;

                color = 0;
                best_distance = 4 * 256 * 256;

                for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
                    distance =
                        (0xff - palette[i].blue) * (0xff - palette[i].blue) +
                        palette[i].red * palette[i].red +
                        palette[i].green * palette[i].green;
                    if (distance < best_distance) {
                        color = i;
                        best_distance = distance;
                    }
                }
            } break;

            case MBOOT_FRAMEBUFFER_TYPE_RGB:
                color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
                        << tagfb->framebuffer_blue_field_position;
                break;

            case MBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                color = '\\' | 0x0100;
                break;

            default:
                color = 0xffffffff;
                break;
            }

            for (i = 0; i < tagfb->common.framebuffer_width &&
                        i < tagfb->common.framebuffer_height;
                 i++) {
                switch (tagfb->common.framebuffer_bpp) {
                case 8: {
                    mboot_uint8_t *pixel =
                        fb + tagfb->common.framebuffer_pitch * i + i;
                    *pixel = color;
                } break;
                case 15:
                case 16: {
                    mboot_uint16_t *pixel =
                        fb + tagfb->common.framebuffer_pitch * i + 2 * i;
                    *pixel = color;
                } break;
                case 24: {
                    mboot_uint32_t *pixel =
                        fb + tagfb->common.framebuffer_pitch * i + 3 * i;
                    *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
                } break;

                case 32: {
                    mboot_uint32_t *pixel =
                        fb + tagfb->common.framebuffer_pitch * i + 4 * i;
                    *pixel = color;
                } break;
                default:
                    break;
                }
            }
            break;
        }
        default:
            break;
        }
    }
    tag = (struct mboot_tag *)((mboot_uint8_t *)tag + ((tag->size + 7) & ~7));
    kprintf("Total mbi size %x", (unsigned)tag - addr);
}
