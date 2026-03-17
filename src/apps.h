/*
 * File: apps.h
 * Purpose: Application workspace state and rendering APIs.
 * Author: zalcus, 2026
 */

#ifndef APPS_H
#define APPS_H

#include "gfx.h"
#include "keyboard.h"
#include "mouse.h"


#define APPS_NOTE_MAX 120
#define APPS_CALC_MAX 31
#define APPS_TASK_COUNT 4

typedef enum {
    APP_NOTES = 0,
    APP_TASKS = 1,
    APP_CALC = 2,
    APP_STATUS = 3,
} app_id_t;

typedef struct {
    app_id_t active;
    char note[APPS_NOTE_MAX + 1];
    uint32_t note_len;
    bool tasks[APPS_TASK_COUNT];
    char calc_expr[APPS_CALC_MAX + 1];
    uint32_t calc_len;
    int32_t calc_result;
    uint32_t ticks;
    int32_t mouse_x;
    int32_t mouse_y;
    bool mouse_left_down;
    bool mouse_left_prev;
} apps_state_t;

void apps_init(apps_state_t *state, uint32_t width, uint32_t height);
bool apps_on_key(apps_state_t *state, const key_event_t *ev);
bool apps_on_mouse(apps_state_t *state, const mouse_event_t *ev, uint32_t width, uint32_t height);
bool apps_on_tick(apps_state_t *state);
void apps_render(framebuffer_t *fb, const apps_state_t *state);

#endif /* APPS_H */
