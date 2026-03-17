/*
 * File: pfs.c
 * Purpose: Persist app state to fixed ATA sector as filesystem MVP.
 * Author: zalcus, 2026
 */

#include "pfs.h"

#include "ata.h"

enum {
    PFS_LBA = 2048U,
    PFS_MAGIC = 0x31534650U,
};

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t note_len;
    uint8_t tasks_mask;
    char note[120];
    uint8_t reserved[512 - 4 - 2 - 1 - 120];
} pfs_record_t;

void pfs_init(void)
{
}

bool pfs_load_app_state(apps_state_t *state)
{
    uint8_t buf[512];
    if (!ata_read_sector(PFS_LBA, buf)) {
        return false;
    }

    const pfs_record_t *r = (const pfs_record_t *)(const void *)buf;
    if (r->magic != PFS_MAGIC) {
        return false;
    }

    uint32_t n = r->note_len;
    if (n > APPS_NOTE_MAX) n = APPS_NOTE_MAX;

    for (uint32_t i = 0; i < n; ++i) {
        state->note[i] = r->note[i];
    }
    state->note[n] = '\0';
    state->note_len = n;

    for (uint32_t i = 0; i < APPS_TASK_COUNT; ++i) {
        state->tasks[i] = ((r->tasks_mask >> i) & 1U) != 0U;
    }

    return true;
}

void pfs_save_app_state(const apps_state_t *state)
{
    uint8_t buf[512];
    for (uint32_t i = 0; i < 512U; ++i) buf[i] = 0U;

    pfs_record_t *r = (pfs_record_t *)(void *)buf;
    r->magic = PFS_MAGIC;
    r->note_len = (uint16_t)state->note_len;

    uint8_t mask = 0U;
    for (uint32_t i = 0; i < APPS_TASK_COUNT; ++i) {
        if (state->tasks[i]) mask |= (uint8_t)(1U << i);
    }
    r->tasks_mask = mask;

    for (uint32_t i = 0; i < state->note_len && i < APPS_NOTE_MAX; ++i) {
        r->note[i] = state->note[i];
    }

    (void)ata_write_sector(PFS_LBA, buf);
}
