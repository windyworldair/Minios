/*
 * File: kernel.c
 * Purpose: MiniOS64 kernel startup with efficient event-driven app loop.
 * Author: zalcus, 2026
 */

#include "kernel.h"

#include "apps.h"
#include "gfx.h"
#include "interrupts.h"
#include "keyboard.h"
#include "memory.h"
#include "mouse.h"
#include "multiboot2.h"
#include "pfs.h"

static bool load_framebuffer_from_multiboot(uint64_t mb_addr, framebuffer_t *out_fb)
{
    const uint8_t *base = (const uint8_t *)(uintptr_t)mb_addr;
    const uint32_t total_size = *(const uint32_t *)base;

    uint32_t offset = 8U;
    while (offset + 8U <= total_size) {
        const mb2_tag_t *tag = (const mb2_tag_t *)(base + offset);
        if (tag->type == MB2_TAG_TYPE_END) break;

        if (tag->type == MB2_TAG_TYPE_FRAMEBUFFER && tag->size >= sizeof(mb2_framebuffer_tag_t)) {
            const mb2_framebuffer_tag_t *fb_tag = (const mb2_framebuffer_tag_t *)tag;
            gfx_init(out_fb,
                     (uint32_t *)(uintptr_t)fb_tag->fb.address,
                     fb_tag->fb.width,
                     fb_tag->fb.height,
                     fb_tag->fb.pitch,
                     fb_tag->fb.bpp);
            return out_fb->ready;
        }
        offset += (tag->size + 7U) & ~7U;
    }
    return false;
}

void kernel_main(uint64_t multiboot_info_addr)
{
    framebuffer_t fb = {0};
    apps_state_t apps = {0};

    if (!load_framebuffer_from_multiboot(multiboot_info_addr, &fb)) {
        for (;;) __asm__ volatile ("hlt");
    }

    memory_init();
    idt_init();
    pic_remap(0x20U, 0x28U);
    pit_init(100U);
    pfs_init();

    uint64_t test_frame = 0U;
    if (memory_alloc_frame(&test_frame)) {
        memory_free_frame(test_frame);
    }

    keyboard_init();
    mouse_init();
    apps_init(&apps, fb.width, fb.height);
    apps_render(&fb, &apps);

    uint32_t spin = 0U;
    for (;;) {
        bool dirty = false;

        key_event_t key;
        while (keyboard_poll_event(&key)) {
            if (apps_on_key(&apps, &key)) dirty = true;
        }

        mouse_event_t mouse;
        while (mouse_poll_event(&mouse)) {
            if (apps_on_mouse(&apps, &mouse, fb.width, fb.height)) dirty = true;
        }

        if (++spin >= 4000U) {
            spin = 0U;
            if (apps_on_tick(&apps)) dirty = true;
        }

        if (dirty) {
            apps_render(&fb, &apps);
        }

        __asm__ volatile ("pause");
    }
}
