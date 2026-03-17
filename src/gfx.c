/*
 * File: gfx.c
 * Purpose: Primitive 2D renderer and compact text drawing for MiniOS64.
 * Author: zalcus, 2026
 */

#include "gfx.h"

static inline uint32_t clamp_u32(uint32_t value, uint32_t min, uint32_t max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static void draw_glyph_5x7(framebuffer_t *fb, uint32_t x, uint32_t y, char c, uint32_t color)
{
    if (!fb->ready) return;

    if (c >= 'a' && c <= 'z') {
        c = (char)(c - ('a' - 'A'));
    }

    uint8_t rows[7] = {0};
    switch (c) {
        case 'A': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x1F; rows[4]=0x11; rows[5]=0x11; rows[6]=0x11; break;
        case 'B': rows[0]=0x1E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x1E; rows[4]=0x11; rows[5]=0x11; rows[6]=0x1E; break;
        case 'C': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x10; rows[3]=0x10; rows[4]=0x10; rows[5]=0x11; rows[6]=0x0E; break;
        case 'D': rows[0]=0x1C; rows[1]=0x12; rows[2]=0x11; rows[3]=0x11; rows[4]=0x11; rows[5]=0x12; rows[6]=0x1C; break;
        case 'E': rows[0]=0x1F; rows[1]=0x10; rows[2]=0x10; rows[3]=0x1E; rows[4]=0x10; rows[5]=0x10; rows[6]=0x1F; break;
        case 'F': rows[0]=0x1F; rows[1]=0x10; rows[2]=0x10; rows[3]=0x1E; rows[4]=0x10; rows[5]=0x10; rows[6]=0x10; break;
        case 'G': rows[0]=0x0F; rows[1]=0x10; rows[2]=0x10; rows[3]=0x17; rows[4]=0x11; rows[5]=0x11; rows[6]=0x0F; break;
        case 'H': rows[0]=0x11; rows[1]=0x11; rows[2]=0x11; rows[3]=0x1F; rows[4]=0x11; rows[5]=0x11; rows[6]=0x11; break;
        case 'I': rows[0]=0x1F; rows[1]=0x04; rows[2]=0x04; rows[3]=0x04; rows[4]=0x04; rows[5]=0x04; rows[6]=0x1F; break;
        case 'J': rows[0]=0x07; rows[1]=0x02; rows[2]=0x02; rows[3]=0x02; rows[4]=0x12; rows[5]=0x12; rows[6]=0x0C; break;
        case 'K': rows[0]=0x11; rows[1]=0x12; rows[2]=0x14; rows[3]=0x18; rows[4]=0x14; rows[5]=0x12; rows[6]=0x11; break;
        case 'L': rows[0]=0x10; rows[1]=0x10; rows[2]=0x10; rows[3]=0x10; rows[4]=0x10; rows[5]=0x10; rows[6]=0x1F; break;
        case 'M': rows[0]=0x11; rows[1]=0x1B; rows[2]=0x15; rows[3]=0x11; rows[4]=0x11; rows[5]=0x11; rows[6]=0x11; break;
        case 'N': rows[0]=0x11; rows[1]=0x19; rows[2]=0x15; rows[3]=0x13; rows[4]=0x11; rows[5]=0x11; rows[6]=0x11; break;
        case 'O': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x11; rows[4]=0x11; rows[5]=0x11; rows[6]=0x0E; break;
        case 'P': rows[0]=0x1E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x1E; rows[4]=0x10; rows[5]=0x10; rows[6]=0x10; break;
        case 'Q': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x11; rows[4]=0x15; rows[5]=0x12; rows[6]=0x0D; break;
        case 'R': rows[0]=0x1E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x1E; rows[4]=0x14; rows[5]=0x12; rows[6]=0x11; break;
        case 'S': rows[0]=0x0F; rows[1]=0x10; rows[2]=0x10; rows[3]=0x0E; rows[4]=0x01; rows[5]=0x01; rows[6]=0x1E; break;
        case 'T': rows[0]=0x1F; rows[1]=0x04; rows[2]=0x04; rows[3]=0x04; rows[4]=0x04; rows[5]=0x04; rows[6]=0x04; break;
        case 'U': rows[0]=0x11; rows[1]=0x11; rows[2]=0x11; rows[3]=0x11; rows[4]=0x11; rows[5]=0x11; rows[6]=0x0E; break;
        case 'V': rows[0]=0x11; rows[1]=0x11; rows[2]=0x11; rows[3]=0x11; rows[4]=0x11; rows[5]=0x0A; rows[6]=0x04; break;
        case 'W': rows[0]=0x11; rows[1]=0x11; rows[2]=0x11; rows[3]=0x15; rows[4]=0x15; rows[5]=0x15; rows[6]=0x0A; break;
        case 'X': rows[0]=0x11; rows[1]=0x11; rows[2]=0x0A; rows[3]=0x04; rows[4]=0x0A; rows[5]=0x11; rows[6]=0x11; break;
        case 'Y': rows[0]=0x11; rows[1]=0x11; rows[2]=0x0A; rows[3]=0x04; rows[4]=0x04; rows[5]=0x04; rows[6]=0x04; break;
        case 'Z': rows[0]=0x1F; rows[1]=0x01; rows[2]=0x02; rows[3]=0x04; rows[4]=0x08; rows[5]=0x10; rows[6]=0x1F; break;
        case '0': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x13; rows[3]=0x15; rows[4]=0x19; rows[5]=0x11; rows[6]=0x0E; break;
        case '1': rows[0]=0x04; rows[1]=0x0C; rows[2]=0x04; rows[3]=0x04; rows[4]=0x04; rows[5]=0x04; rows[6]=0x0E; break;
        case '2': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x01; rows[3]=0x02; rows[4]=0x04; rows[5]=0x08; rows[6]=0x1F; break;
        case '3': rows[0]=0x1E; rows[1]=0x01; rows[2]=0x01; rows[3]=0x0E; rows[4]=0x01; rows[5]=0x01; rows[6]=0x1E; break;
        case '4': rows[0]=0x02; rows[1]=0x06; rows[2]=0x0A; rows[3]=0x12; rows[4]=0x1F; rows[5]=0x02; rows[6]=0x02; break;
        case '5': rows[0]=0x1F; rows[1]=0x10; rows[2]=0x10; rows[3]=0x1E; rows[4]=0x01; rows[5]=0x01; rows[6]=0x1E; break;
        case '6': rows[0]=0x0E; rows[1]=0x10; rows[2]=0x10; rows[3]=0x1E; rows[4]=0x11; rows[5]=0x11; rows[6]=0x0E; break;
        case '7': rows[0]=0x1F; rows[1]=0x01; rows[2]=0x02; rows[3]=0x04; rows[4]=0x08; rows[5]=0x08; rows[6]=0x08; break;
        case '8': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x0E; rows[4]=0x11; rows[5]=0x11; rows[6]=0x0E; break;
        case '9': rows[0]=0x0E; rows[1]=0x11; rows[2]=0x11; rows[3]=0x0F; rows[4]=0x01; rows[5]=0x01; rows[6]=0x0E; break;
        case ':': rows[0]=0x00; rows[1]=0x04; rows[2]=0x00; rows[3]=0x00; rows[4]=0x04; rows[5]=0x00; rows[6]=0x00; break;
        case '.': rows[0]=0x00; rows[1]=0x00; rows[2]=0x00; rows[3]=0x00; rows[4]=0x00; rows[5]=0x0C; rows[6]=0x0C; break;
        case '-': rows[0]=0x00; rows[1]=0x00; rows[2]=0x00; rows[3]=0x1F; rows[4]=0x00; rows[5]=0x00; rows[6]=0x00; break;
        case '+': rows[0]=0x00; rows[1]=0x04; rows[2]=0x04; rows[3]=0x1F; rows[4]=0x04; rows[5]=0x04; rows[6]=0x00; break;
        case '/': rows[0]=0x01; rows[1]=0x02; rows[2]=0x04; rows[3]=0x08; rows[4]=0x10; rows[5]=0x00; rows[6]=0x00; break;
        case '*': rows[0]=0x00; rows[1]=0x15; rows[2]=0x0E; rows[3]=0x1F; rows[4]=0x0E; rows[5]=0x15; rows[6]=0x00; break;
        case '=': rows[0]=0x00; rows[1]=0x1F; rows[2]=0x00; rows[3]=0x1F; rows[4]=0x00; rows[5]=0x00; rows[6]=0x00; break;
        case '_': rows[0]=0x00; rows[1]=0x00; rows[2]=0x00; rows[3]=0x00; rows[4]=0x00; rows[5]=0x00; rows[6]=0x1F; break;
        case ' ': default: break;
    }

    for (uint32_t row = 0; row < 7U; ++row) {
        for (uint32_t col = 0; col < 5U; ++col) {
            if ((rows[row] >> (4U - col)) & 1U) {
                gfx_fill_rect(fb, x + col, y + row, 1U, 1U, color);
            }
        }
    }
}

void gfx_init(framebuffer_t *fb, uint32_t *addr, uint32_t width, uint32_t height, uint32_t pitch_bytes, uint8_t bpp)
{
    fb->base = addr;
    fb->width = width;
    fb->height = height;
    fb->pitch_pixels = pitch_bytes / 4U;
    fb->ready = (addr != 0) && (width > 0U) && (height > 0U) && (bpp == 32U);
}

void gfx_clear(framebuffer_t *fb, uint32_t color)
{
    if (!fb->ready) return;
    for (uint32_t y = 0; y < fb->height; ++y) {
        uint32_t *row = fb->base + ((uint64_t)y * fb->pitch_pixels);
        for (uint32_t x = 0; x < fb->width; ++x) {
            row[x] = color;
        }
    }
}

void gfx_fill_rect(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    if (!fb->ready || w == 0U || h == 0U) return;

    uint32_t x0 = clamp_u32(x, 0U, fb->width);
    uint32_t y0 = clamp_u32(y, 0U, fb->height);
    uint32_t x1 = clamp_u32(x + w, 0U, fb->width);
    uint32_t y1 = clamp_u32(y + h, 0U, fb->height);

    for (uint32_t py = y0; py < y1; ++py) {
        uint32_t *row = fb->base + ((uint64_t)py * fb->pitch_pixels);
        for (uint32_t px = x0; px < x1; ++px) {
            row[px] = color;
        }
    }
}

void gfx_draw_rect_outline(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    if (!fb->ready || w < 2U || h < 2U) return;
    gfx_fill_rect(fb, x, y, w, 1U, color);
    gfx_fill_rect(fb, x, y + h - 1U, w, 1U, color);
    gfx_fill_rect(fb, x, y, 1U, h, color);
    gfx_fill_rect(fb, x + w - 1U, y, 1U, h, color);
}

void gfx_draw_text(framebuffer_t *fb, uint32_t x, uint32_t y, const char *text, uint32_t color)
{
    if (!fb->ready || text == 0) return;

    uint32_t cx = x;
    for (uint32_t i = 0U; text[i] != '\0'; ++i) {
        const char c = text[i];
        if (c == '\n') {
            cx = x;
            y += 9U;
            continue;
        }
        draw_glyph_5x7(fb, cx, y, c, color);
        cx += 6U;
    }
}

void gfx_draw_panel(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t header_color, uint32_t body_color)
{
    gfx_fill_rect(fb, x, y, w, h, body_color);
    gfx_fill_rect(fb, x, y, w, 22U, header_color);
    gfx_draw_rect_outline(fb, x, y, w, h, 0x0033445A);
}

void gfx_draw_mouse_cursor(framebuffer_t *fb, int32_t x, int32_t y, uint32_t color)
{
    if (!fb->ready) return;
    for (int32_t i = 0; i < 12; ++i) {
        if (x + i >= 0 && y + i >= 0) {
            gfx_fill_rect(fb, (uint32_t)(x + i), (uint32_t)y, 1U, 1U, color);
            gfx_fill_rect(fb, (uint32_t)x, (uint32_t)(y + i), 1U, 1U, color);
        }
    }
    gfx_fill_rect(fb, (uint32_t)x, (uint32_t)y, 2U, 2U, 0x00000000);
}
