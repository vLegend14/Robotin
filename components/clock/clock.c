#include "clock.h"
#include "display.h"
#include "u8g2.h"
#include <stdio.h>

void clock_render(uint32_t elapsed_ms) {
    u8g2_t *u8g2 = display_get();

    uint32_t total_s = elapsed_ms / 1000;
    uint16_t ms      = elapsed_ms % 1000;
    uint8_t  hh      = total_s / 3600;
    uint8_t  mm      = (total_s % 3600) / 60;
    uint8_t  ss      = total_s % 60;

    char buf_time[10]; // "HH:MM:SS\0"
    char buf_ms[5];    // ".mmm\0"
    snprintf(buf_time, sizeof(buf_time), "%02d:%02d:%02d", hh, mm, ss);
    snprintf(buf_ms,   sizeof(buf_ms),   ".%03d", ms);

    u8g2_ClearBuffer(u8g2);

    // ─── Label ────────────────────────────
    u8g2_SetFont(u8g2, u8g2_font_6x10_tr);
    u8g2_DrawStr(u8g2, 49, 12, "uptime");
    u8g2_DrawHLine(u8g2, 0, 16, 128);

    // ─── HH:MM:SS (grande) ────────────────
    // 10x20, 8 chars = 80px → x = (128-80)/2 = 24
    u8g2_SetFont(u8g2, u8g2_font_10x20_mr);
    u8g2_DrawStr(u8g2, 24, 42, buf_time);

    // ─── .mmm (pequeño, centrado) ─────────
    // 6x10, 4 chars = 24px → x = (128-24)/2 = 52
    u8g2_SetFont(u8g2, u8g2_font_6x10_tr);
    u8g2_DrawStr(u8g2, 52, 58, buf_ms);

    u8g2_SendBuffer(u8g2);
}