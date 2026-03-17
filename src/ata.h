/*
 * File: ata.h
 * Purpose: ATA PIO sector read/write interfaces.
 * Author: zalcus, 2026
 */

#ifndef ATA_H
#define ATA_H

#include <stdbool.h>
#include <stdint.h>

bool ata_read_sector(uint32_t lba, uint8_t *buffer512);
bool ata_write_sector(uint32_t lba, const uint8_t *buffer512);

#endif /* ATA_H */
