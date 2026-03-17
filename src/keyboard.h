/*
 * File: keyboard.h
 * Purpose: PS/2 keyboard polling driver and key translation.
 * Author: zalcus, 2026
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

typedef struct {
    bool pressed;
    char ch;
    bool backspace;
    bool enter;
    bool tab;
} key_event_t;

void keyboard_init(void);
bool keyboard_poll_event(key_event_t *event);

#endif /* KEYBOARD_H */
