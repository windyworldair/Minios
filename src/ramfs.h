/*
 * File: ramfs.h
 * Purpose: In-memory filesystem for MiniOS64 app persistence layer.
 * Author: zalcus, 2026
 */

#ifndef RAMFS_H
#define RAMFS_H

#include <stdbool.h>
#include <stdint.h>

#define RAMFS_MAX_FILES 8
#define RAMFS_NAME_MAX 24
#define RAMFS_DATA_MAX 256

void ramfs_init(void);
bool ramfs_write_file(const char *name, const char *data, uint32_t len);
bool ramfs_read_file(const char *name, char *out, uint32_t max_len, uint32_t *out_len);

#endif /* RAMFS_H */
