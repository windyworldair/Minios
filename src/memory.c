/*
 * File: memory.c
 * Purpose: Bitmap-based frame allocator used by MiniOS64 phase-2 foundation.
 * Author: zalcus, 2026
 */

#include "memory.h"

enum {
    FRAME_SIZE = 4096U,
    TOTAL_FRAMES = 4096U,
    BITMAP_WORDS = TOTAL_FRAMES / 32U,
};

static uint32_t frame_bitmap[BITMAP_WORDS];
static uint32_t used_frames = 0U;

static inline bool bit_is_set(uint32_t idx)
{
    return (frame_bitmap[idx / 32U] & (1U << (idx % 32U))) != 0U;
}

static inline void bit_set(uint32_t idx)
{
    frame_bitmap[idx / 32U] |= (1U << (idx % 32U));
}

static inline void bit_clear(uint32_t idx)
{
    frame_bitmap[idx / 32U] &= ~(1U << (idx % 32U));
}

void memory_init(void)
{
    for (uint32_t i = 0; i < BITMAP_WORDS; ++i) {
        frame_bitmap[i] = 0U;
    }
    used_frames = 0U;

    const uint32_t reserved_frames = 256U;
    for (uint32_t i = 0; i < reserved_frames; ++i) {
        bit_set(i);
        used_frames++;
    }
}

bool memory_alloc_frame(uint64_t *out_addr)
{
    if (out_addr == 0) {
        return false;
    }

    for (uint32_t i = 0; i < TOTAL_FRAMES; ++i) {
        if (!bit_is_set(i)) {
            bit_set(i);
            used_frames++;
            *out_addr = (uint64_t)i * FRAME_SIZE;
            return true;
        }
    }

    return false;
}

void memory_free_frame(uint64_t addr)
{
    const uint32_t idx = (uint32_t)(addr / FRAME_SIZE);
    if (idx >= TOTAL_FRAMES || !bit_is_set(idx)) {
        return;
    }
    bit_clear(idx);
    used_frames--;
}

uint32_t memory_total_frames(void)
{
    return TOTAL_FRAMES;
}

uint32_t memory_used_frames(void)
{
    return used_frames;
}
