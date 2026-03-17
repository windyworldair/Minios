/*
 * File: gfx.h
 * Purpose: Framebuffer drawing and text primitives for MiniOS64 GUI.
 * Author: zalcus, 2026
 */

#ifndef GFX_H
#define GFX_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t *base;
    uint32_t width;
    uint32_t height;
    uint32_t pitch_pixels;
    bool ready;
} framebuffer_t;

void gfx_init(framebuffer_t *fb, uint32_t *addr, uint32_t width, uint32_t height, uint32_t pitch_bytes, uint8_t bpp);
void gfx_clear(framebuffer_t *fb, uint32_t color);
void gfx_fill_rect(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void gfx_draw_rect_outline(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void gfx_draw_text(framebuffer_t *fb, uint32_t x, uint32_t y, const char *text, uint32_t color);
void gfx_draw_panel(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t header_color, uint32_t body_color);
void gfx_draw_mouse_cursor(framebuffer_t *fb, int32_t x, int32_t y, uint32_t color);

#endif /* GFX_H */
