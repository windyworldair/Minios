/*
 * File: keyboard.c
 * Purpose: PS/2 keyboard driver for interactive MiniOS64 apps.
 * Author: zalcus, 2026
 */

#include "keyboard.h"

#include "io.h"

static bool shift_down = false;

static char scancode_to_char(uint8_t scancode, bool shift)
{
    switch (scancode) {
        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';
        case 0x0C: return shift ? '_' : '-';
        case 0x0D: return shift ? '+' : '=';
        case 0x10: return shift ? 'Q' : 'q';
        case 0x11: return shift ? 'W' : 'w';
        case 0x12: return shift ? 'E' : 'e';
        case 0x13: return shift ? 'R' : 'r';
        case 0x14: return shift ? 'T' : 't';
        case 0x15: return shift ? 'Y' : 'y';
        case 0x16: return shift ? 'U' : 'u';
        case 0x17: return shift ? 'I' : 'i';
        case 0x18: return shift ? 'O' : 'o';
        case 0x19: return shift ? 'P' : 'p';
        case 0x1E: return shift ? 'A' : 'a';
        case 0x1F: return shift ? 'S' : 's';
        case 0x20: return shift ? 'D' : 'd';
        case 0x21: return shift ? 'F' : 'f';
        case 0x22: return shift ? 'G' : 'g';
        case 0x23: return shift ? 'H' : 'h';
        case 0x24: return shift ? 'J' : 'j';
        case 0x25: return shift ? 'K' : 'k';
        case 0x26: return shift ? 'L' : 'l';
        case 0x2C: return shift ? 'Z' : 'z';
        case 0x2D: return shift ? 'X' : 'x';
        case 0x2E: return shift ? 'C' : 'c';
        case 0x2F: return shift ? 'V' : 'v';
        case 0x30: return shift ? 'B' : 'b';
        case 0x31: return shift ? 'N' : 'n';
        case 0x32: return shift ? 'M' : 'm';
        case 0x33: return shift ? '<' : ',';
        case 0x34: return shift ? '>' : '.';
        case 0x35: return shift ? '?' : '/';
        case 0x39: return ' ';
        default: return '\0';
    }
}

void keyboard_init(void)
{
    shift_down = false;
}

bool keyboard_poll_event(key_event_t *event)
{
    if (event == 0) {
        return false;
    }

    event->pressed = false;
    event->ch = '\0';
    event->backspace = false;
    event->enter = false;
    event->tab = false;

    if ((inb(0x64) & 0x01U) == 0U) {
        return false;
    }

    const uint8_t sc = inb(0x60);

    if (sc == 0x2A || sc == 0x36) {
        shift_down = true;
        return false;
    }
    if (sc == 0xAA || sc == 0xB6) {
        shift_down = false;
        return false;
    }

    if ((sc & 0x80U) != 0U) {
        return false;
    }

    if (sc == 0x0E) {
        event->backspace = true;
        event->pressed = true;
        return true;
    }
    if (sc == 0x1C) {
        event->enter = true;
        event->pressed = true;
        return true;
    }
    if (sc == 0x0F) {
        event->tab = true;
        event->pressed = true;
        return true;
    }

    const char ch = scancode_to_char(sc, shift_down);
    if (ch != '\0') {
        event->ch = ch;
        event->pressed = true;
        return true;
    }

    return false;
}
