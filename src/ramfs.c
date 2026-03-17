/*
 * File: ramfs.c
 * Purpose: Simple fixed-size RAM filesystem implementation.
 * Author: zalcus, 2026
 */

#include "ramfs.h"

typedef struct {
    bool used;
    char name[RAMFS_NAME_MAX];
    char data[RAMFS_DATA_MAX];
    uint32_t len;
} ramfs_file_t;

static ramfs_file_t files[RAMFS_MAX_FILES];

static uint32_t str_len(const char *s)
{
    uint32_t n = 0U;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

static bool str_eq(const char *a, const char *b)
{
    uint32_t i = 0U;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

static void str_copy(char *dst, const char *src, uint32_t max)
{
    uint32_t i = 0U;
    if (max == 0U) return;
    while (src[i] != '\0' && i + 1U < max) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

void ramfs_init(void)
{
    for (uint32_t i = 0; i < RAMFS_MAX_FILES; ++i) {
        files[i].used = false;
        files[i].name[0] = '\0';
        files[i].data[0] = '\0';
        files[i].len = 0U;
    }
}

bool ramfs_write_file(const char *name, const char *data, uint32_t len)
{
    if (name == 0 || data == 0) return false;
    if (str_len(name) >= RAMFS_NAME_MAX) return false;
    if (len >= RAMFS_DATA_MAX) return false;

    int32_t slot = -1;
    for (uint32_t i = 0; i < RAMFS_MAX_FILES; ++i) {
        if (files[i].used && str_eq(files[i].name, name)) {
            slot = (int32_t)i;
            break;
        }
        if (!files[i].used && slot < 0) {
            slot = (int32_t)i;
        }
    }

    if (slot < 0) return false;

    ramfs_file_t *f = &files[(uint32_t)slot];
    f->used = true;
    str_copy(f->name, name, RAMFS_NAME_MAX);
    for (uint32_t i = 0; i < len; ++i) {
        f->data[i] = data[i];
    }
    f->data[len] = '\0';
    f->len = len;
    return true;
}

bool ramfs_read_file(const char *name, char *out, uint32_t max_len, uint32_t *out_len)
{
    if (name == 0 || out == 0 || max_len == 0U || out_len == 0) return false;

    for (uint32_t i = 0; i < RAMFS_MAX_FILES; ++i) {
        if (files[i].used && str_eq(files[i].name, name)) {
            const uint32_t n = (files[i].len < (max_len - 1U)) ? files[i].len : (max_len - 1U);
            for (uint32_t j = 0; j < n; ++j) {
                out[j] = files[i].data[j];
            }
            out[n] = '\0';
            *out_len = n;
            return true;
        }
    }

    return false;
}
