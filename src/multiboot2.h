/*
 * File: multiboot2.h
 * Purpose: Minimal Multiboot2 structures used by MiniOS64.
 * Author: zalcus, 2026
 */

#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

enum {
    MB2_TAG_TYPE_END = 0,
    MB2_TAG_TYPE_FRAMEBUFFER = 8,
};

typedef struct __attribute__((packed)) {
    uint32_t type;
    uint32_t size;
} mb2_tag_t;

typedef struct __attribute__((packed)) {
    uint64_t address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type;
    uint16_t reserved;
} mb2_framebuffer_tag_payload_t;

typedef struct __attribute__((packed)) {
    uint32_t type;
    uint32_t size;
    mb2_framebuffer_tag_payload_t fb;
} mb2_framebuffer_tag_t;

#endif /* MULTIBOOT2_H */
