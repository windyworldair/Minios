/*
 * File: apps.c
 * Purpose: Four interactive applications for MiniOS64 workspace.
 * Author: zalcus, 2026
 */

#include "apps.h"

#include "pfs.h"

static char up(char c)
{
    if (c >= 'a' && c <= 'z') return (char)(c - ('a' - 'A'));
    return c;
}

static int32_t parse_num(const char *s, uint32_t a, uint32_t b)
{
    int32_t v = 0;
    for (uint32_t i = a; i < b; ++i) v = v * 10 + (s[i] - '0');
    return v;
}

static bool calc_eval(const char *s, uint32_t n, int32_t *out)
{
    uint32_t opi = n;
    char op = '\0';
    for (uint32_t i = 0; i < n; ++i) {
        if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
            opi = i;
            op = s[i];
            break;
        }
    }
    if (opi == 0U || opi + 1U >= n) return false;
    for (uint32_t i = 0; i < opi; ++i) if (s[i] < '0' || s[i] > '9') return false;
    for (uint32_t i = opi + 1U; i < n; ++i) if (s[i] < '0' || s[i] > '9') return false;

    int32_t l = parse_num(s, 0U, opi);
    int32_t r = parse_num(s, opi + 1U, n);
    if (op == '+') *out = l + r;
    else if (op == '-') *out = l - r;
    else if (op == '*') *out = l * r;
    else if (op == '/') {
        if (r == 0) return false;
        *out = l / r;
    } else return false;
    return true;
}

static bool hit(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t rw, int32_t rh)
{
    return x >= rx && y >= ry && x < (rx + rw) && y < (ry + rh);
}

static void persist_state(const apps_state_t *s)
{
    pfs_save_app_state(s);
}

void apps_init(apps_state_t *s, uint32_t w, uint32_t h)
{
    s->active = APP_NOTES;
    s->note_len = 0U;
    s->note[0] = '\0';
    for (uint32_t i = 0; i < APPS_TASK_COUNT; ++i) s->tasks[i] = false;
    s->calc_len = 0U;
    s->calc_expr[0] = '\0';
    s->calc_result = 0;
    s->ticks = 0U;
    s->mouse_x = (int32_t)(w / 2U);
    s->mouse_y = (int32_t)(h / 2U);
    s->mouse_left_down = false;
    s->mouse_left_prev = false;

    (void)pfs_load_app_state(s);
}

bool apps_on_key(apps_state_t *s, const key_event_t *ev)
{
    if (!ev->pressed) return false;

    if (ev->tab) {
        s->active = (app_id_t)(((uint32_t)s->active + 1U) % 4U);
        return true;
    }
    if (ev->ch >= '1' && ev->ch <= '4') {
        s->active = (app_id_t)(ev->ch - '1');
        return true;
    }

    if (s->active == APP_NOTES) {
        if (ev->backspace) {
            if (s->note_len > 0U) s->note[--s->note_len] = '\0';
            persist_state(s);
            return true;
        }
        if (ev->ch != '\0' && s->note_len < APPS_NOTE_MAX) {
            s->note[s->note_len++] = up(ev->ch);
            s->note[s->note_len] = '\0';
            persist_state(s);
            return true;
        }
        return false;
    }

    if (s->active == APP_TASKS) {
        const char c = up(ev->ch);
        if (c >= 'Q' && c <= 'T') {
            uint32_t idx = (uint32_t)(c - 'Q');
            if (idx < APPS_TASK_COUNT) {
                s->tasks[idx] = !s->tasks[idx];
                persist_state(s);
                return true;
            }
        }
        return false;
    }

    if (s->active == APP_CALC) {
        if (ev->backspace) {
            if (s->calc_len > 0U) s->calc_expr[--s->calc_len] = '\0';
            return true;
        }
        if (ev->enter || ev->ch == '=') {
            int32_t out = 0;
            if (calc_eval(s->calc_expr, s->calc_len, &out)) s->calc_result = out;
            return true;
        }
        if (((ev->ch >= '0' && ev->ch <= '9') || ev->ch == '+' || ev->ch == '-' || ev->ch == '*' || ev->ch == '/') && s->calc_len < APPS_CALC_MAX) {
            s->calc_expr[s->calc_len++] = ev->ch;
            s->calc_expr[s->calc_len] = '\0';
            return true;
        }
    }

    return false;
}

bool apps_on_mouse(apps_state_t *s, const mouse_event_t *ev, uint32_t w, uint32_t h)
{
    bool changed = false;

    s->mouse_x += ev->dx;
    s->mouse_y += ev->dy;
    if (s->mouse_x < 0) s->mouse_x = 0;
    if (s->mouse_y < 0) s->mouse_y = 0;
    if (s->mouse_x >= (int32_t)w) s->mouse_x = (int32_t)w - 1;
    if (s->mouse_y >= (int32_t)h) s->mouse_y = (int32_t)h - 1;
    if (ev->dx != 0 || ev->dy != 0) changed = true;

    s->mouse_left_prev = s->mouse_left_down;
    s->mouse_left_down = ev->left;
    const bool click = (!s->mouse_left_prev && s->mouse_left_down);

    if (click) {
        if (hit(s->mouse_x, s->mouse_y, 20, 34, 90, 26)) { s->active = APP_NOTES; changed = true; }
        else if (hit(s->mouse_x, s->mouse_y, 120, 34, 90, 26)) { s->active = APP_TASKS; changed = true; }
        else if (hit(s->mouse_x, s->mouse_y, 220, 34, 90, 26)) { s->active = APP_CALC; changed = true; }
        else if (hit(s->mouse_x, s->mouse_y, 320, 34, 90, 26)) { s->active = APP_STATUS; changed = true; }

        if (s->active == APP_TASKS) {
            for (int i = 0; i < 4; ++i) {
                if (hit(s->mouse_x, s->mouse_y, 60, 112 + i * 30, 18, 18)) {
                    s->tasks[i] = !s->tasks[i];
                    persist_state(s);
                    changed = true;
                }
            }
        }
    }

    return changed;
}

bool apps_on_tick(apps_state_t *s)
{
    s->ticks++;
    return (s->ticks % 120U) == 0U;
}

static void draw_tabs(framebuffer_t *fb, app_id_t a)
{
    const char *names[4] = {"1 NOTES", "2 TASKS", "3 CALC", "4 STATUS"};
    for (uint32_t i = 0; i < 4U; ++i) {
        uint32_t x = 20U + i * 100U;
        uint32_t color = (i == (uint32_t)a) ? 0x002A4E7D : 0x00384961;
        gfx_fill_rect(fb, x, 34U, 90U, 26U, color);
        gfx_draw_rect_outline(fb, x, 34U, 90U, 26U, 0x009BB2CC);
        gfx_draw_text(fb, x + 8U, 43U, names[i], 0x00E8F1FA);
    }
}

static void draw_notes(framebuffer_t *fb, const apps_state_t *s)
{
    gfx_draw_panel(fb, 20, 70, 760, 490, 0x002A4E7D, 0x00F6F9FD);
    gfx_draw_text(fb, 40, 82, "NOTES APP - TYPE TEXT", 0x00EAF1FB);
    gfx_fill_rect(fb, 40, 118, 720, 420, 0x00FFFFFF);
    gfx_draw_rect_outline(fb, 40, 118, 720, 420, 0x00CAD7E8);
    gfx_draw_text(fb, 50, 132, s->note, 0x001B2D44);
}

static void draw_tasks(framebuffer_t *fb, const apps_state_t *s)
{
    const char *labels[4] = {"Q CHECK MAIL", "W BUILD PROJECT", "E EXERCISE", "T READ DOCS"};
    gfx_draw_panel(fb, 20, 70, 760, 490, 0x002A4E7D, 0x00EFF4FB);
    gfx_draw_text(fb, 40, 82, "TASKS APP - CLICK BOX OR Q/W/E/T", 0x00EAF1FB);
    for (uint32_t i = 0; i < 4U; ++i) {
        uint32_t y = 110U + i * 30U;
        gfx_fill_rect(fb, 60, y + 2U, 18, 18, 0x00FFFFFF);
        gfx_draw_rect_outline(fb, 60, y + 2U, 18, 18, 0x004F6582);
        if (s->tasks[i]) {
            gfx_fill_rect(fb, 63, y + 5U, 12, 12, 0x0022C55E);
        }
        gfx_draw_text(fb, 90, y + 7U, labels[i], 0x001D3048);
    }
}

static void draw_calc(framebuffer_t *fb, const apps_state_t *s)
{
    char result[24] = "RESULT 0";
    int32_t v = s->calc_result;
    char tmp[14];
    uint32_t n = 0U;
    bool neg = false;
    if (v < 0) {
        neg = true;
        v = -v;
    }
    do {
        tmp[n++] = (char)('0' + (v % 10));
        v /= 10;
    } while (v > 0 && n < 13U);

    uint32_t p = 7U;
    if (neg) result[p++] = '-';
    for (uint32_t i = 0; i < n; ++i) result[p++] = tmp[n - i - 1U];
    result[p] = '\0';

    gfx_draw_panel(fb, 20, 70, 760, 490, 0x002A4E7D, 0x00F3F7FD);
    gfx_draw_text(fb, 40, 82, "CALCULATOR APP - e.g. 12+7 ENTER", 0x00EAF1FB);
    gfx_fill_rect(fb, 60, 130, 280, 30, 0x00FFFFFF);
    gfx_draw_rect_outline(fb, 60, 130, 280, 30, 0x004F6582);
    gfx_draw_text(fb, 72, 142, s->calc_expr, 0x001D3048);
    gfx_draw_text(fb, 60, 188, result, 0x001D3048);
}

static void draw_status(framebuffer_t *fb, const apps_state_t *s)
{
    uint32_t cpu = 10U + (s->ticks % 85U);
    uint32_t mem = 20U + ((s->ticks / 2U) % 75U);
    uint32_t io = 25U + ((s->ticks / 3U) % 70U);

    gfx_draw_panel(fb, 20, 70, 760, 490, 0x002A4E7D, 0x00EEF5FB);
    gfx_draw_text(fb, 40, 82, "SYSTEM STATUS APP", 0x00EAF1FB);

    gfx_draw_text(fb, 50, 128, "CPU", 0x001D3048);
    gfx_fill_rect(fb, 120, 126, 520, 14, 0x00CFDCEC);
    gfx_fill_rect(fb, 120, 126, (520U * cpu) / 100U, 14, 0x001D4ED8);

    gfx_draw_text(fb, 50, 170, "MEM", 0x001D3048);
    gfx_fill_rect(fb, 120, 168, 520, 14, 0x00CFDCEC);
    gfx_fill_rect(fb, 120, 168, (520U * mem) / 100U, 14, 0x000EA5E9);

    gfx_draw_text(fb, 50, 212, "IO", 0x001D3048);
    gfx_fill_rect(fb, 120, 210, 520, 14, 0x00CFDCEC);
    gfx_fill_rect(fb, 120, 210, (520U * io) / 100U, 14, 0x0010B981);
}

void apps_render(framebuffer_t *fb, const apps_state_t *s)
{
    gfx_clear(fb, 0x000E1624);
    gfx_fill_rect(fb, 0, 0, fb->width, 28, 0x00111926);
    gfx_draw_text(fb, 10, 10, "MINIOS64 KERNEL LOOP: EVENT-DRIVEN", 0x00DDE8F6);

    draw_tabs(fb, s->active);
    if (s->active == APP_NOTES) draw_notes(fb, s);
    else if (s->active == APP_TASKS) draw_tasks(fb, s);
    else if (s->active == APP_CALC) draw_calc(fb, s);
    else draw_status(fb, s);

    gfx_draw_mouse_cursor(fb, s->mouse_x, s->mouse_y, 0x00F8FBFF);
}
