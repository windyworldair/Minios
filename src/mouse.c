/*
 * File: mouse.c
 * Purpose: PS/2 mouse driver for MiniOS64 pointer input.
 * Author: zalcus, 2026
 */

#include "mouse.h"

#include "io.h"

static void io_wait(void)
{
    for (volatile uint32_t i = 0; i < 10000U; ++i) {
        __asm__ volatile ("pause");
    }
}

static void mouse_write(uint8_t data)
{
    io_wait();
    outb(0x64, 0xD4);
    io_wait();
    outb(0x60, data);
    io_wait();
}

static uint8_t mouse_read(void)
{
    io_wait();
    return inb(0x60);
}

void mouse_init(void)
{
    outb(0x64, 0xA8);
    io_wait();

    outb(0x64, 0x20);
    io_wait();
    uint8_t status = inb(0x60);
    status |= 0x02U;
    outb(0x64, 0x60);
    io_wait();
    outb(0x60, status);
    io_wait();

    mouse_write(0xF6);
    (void)mouse_read();

    mouse_write(0xF4);
    (void)mouse_read();
}

bool mouse_poll_event(mouse_event_t *event)
{
    static uint8_t packet[3];
    static uint8_t index = 0;

    if (event == 0) {
        return false;
    }

    event->dx = 0;
    event->dy = 0;
    event->left = false;
    event->right = false;
    event->middle = false;
    event->changed = false;

    if ((inb(0x64) & 0x21U) != 0x21U) {
        return false;
    }

    packet[index++] = inb(0x60);
    if (index < 3U) {
        return false;
    }
    index = 0;

    if ((packet[0] & 0x08U) == 0U) {
        return false;
    }

    int32_t dx = (int32_t)packet[1];
    int32_t dy = (int32_t)packet[2];

    if ((packet[0] & 0x10U) != 0U) {
        dx |= ~0xFF;
    }
    if ((packet[0] & 0x20U) != 0U) {
        dy |= ~0xFF;
    }

    event->dx = dx;
    event->dy = -dy;
    event->left = (packet[0] & 0x01U) != 0U;
    event->right = (packet[0] & 0x02U) != 0U;
    event->middle = (packet[0] & 0x04U) != 0U;
    event->changed = (dx != 0) || (dy != 0) || event->left || event->right || event->middle;
    return true;
}
