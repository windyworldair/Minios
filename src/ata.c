/*
 * File: ata.c
 * Purpose: Minimal ATA PIO implementation for storage-backed filesystem foundation.
 * Author: zalcus, 2026
 */

#include "ata.h"

#include "io.h"

static bool ata_wait_ready(void)
{
    for (uint32_t i = 0; i < 1000000U; ++i) {
        const uint8_t s = inb(0x1F7);
        if ((s & 0x80U) == 0U && (s & 0x08U) != 0U) {
            return true;
        }
    }
    return false;
}

bool ata_read_sector(uint32_t lba, uint8_t *buffer512)
{
    if (buffer512 == 0) return false;

    outb(0x1F6, (uint8_t)(0xE0U | ((lba >> 24U) & 0x0FU)));
    outb(0x1F2, 1U);
    outb(0x1F3, (uint8_t)(lba & 0xFFU));
    outb(0x1F4, (uint8_t)((lba >> 8U) & 0xFFU));
    outb(0x1F5, (uint8_t)((lba >> 16U) & 0xFFU));
    outb(0x1F7, 0x20U);

    if (!ata_wait_ready()) return false;

    for (uint32_t i = 0; i < 256U; ++i) {
        const uint16_t w = (uint16_t)inb(0x1F0) | ((uint16_t)inb(0x1F0) << 8U);
        buffer512[i * 2U] = (uint8_t)(w & 0xFFU);
        buffer512[i * 2U + 1U] = (uint8_t)(w >> 8U);
    }
    return true;
}

bool ata_write_sector(uint32_t lba, const uint8_t *buffer512)
{
    if (buffer512 == 0) return false;

    outb(0x1F6, (uint8_t)(0xE0U | ((lba >> 24U) & 0x0FU)));
    outb(0x1F2, 1U);
    outb(0x1F3, (uint8_t)(lba & 0xFFU));
    outb(0x1F4, (uint8_t)((lba >> 8U) & 0xFFU));
    outb(0x1F5, (uint8_t)((lba >> 16U) & 0xFFU));
    outb(0x1F7, 0x30U);

    if (!ata_wait_ready()) return false;

    for (uint32_t i = 0; i < 256U; ++i) {
        const uint16_t w = (uint16_t)buffer512[i * 2U] | ((uint16_t)buffer512[i * 2U + 1U] << 8U);
        outb(0x1F0, (uint8_t)(w & 0xFFU));
        outb(0x1F0, (uint8_t)(w >> 8U));
    }

    outb(0x1F7, 0xE7);
    return ata_wait_ready();
}
