/*
 * File: mouse.h
 * Purpose: PS/2 mouse initialization and packet polling for MiniOS64.
 * Author: zalcus, 2026
 */

#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int32_t dx;
    int32_t dy;
    bool left;
    bool right;
    bool middle;
    bool changed;
} mouse_event_t;

void mouse_init(void);
bool mouse_poll_event(mouse_event_t *event);

#endif /* MOUSE_H */
