/*
 * File: pfs.h
 * Purpose: Persistent filesystem wrapper over ATA sector storage.
 * Author: zalcus, 2026
 */

#ifndef PFS_H
#define PFS_H

#include <stdbool.h>
#include <stdint.h>

#include "apps.h"

void pfs_init(void);
bool pfs_load_app_state(apps_state_t *state);
void pfs_save_app_state(const apps_state_t *state);

#endif /* PFS_H */
