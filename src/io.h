/*
 * File: io.h
 * Purpose: x86 port I/O helpers for MiniOS64 drivers.
 * Author: zalcus, 2026
 */

#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

#endif /* IO_H */
