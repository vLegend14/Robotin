#include "faces.h"
#include "display.h"
#include "u8g2.h"
#include <stdbool.h>

// ─── Layout ─────────────────────────────────
#define EYE_L_X  16
#define EYE_R_X  78
#define EYE_Y     8
#define EYE_W    28
#define EYE_H    38
#define EYE_R     7
#define MOUTH_X  64
#define MOUTH_Y  51

// ─── Boca ───────────────────────────────────
static void boca_sonrisa(u8g2_t *u8g2) {
    u8g2_DrawArc(u8g2, MOUTH_X, MOUTH_Y + 4, 5, 128, 255);
}

static void boca_triste(u8g2_t *u8g2) {
    u8g2_DrawArc(u8g2, MOUTH_X, MOUTH_Y - 3, 5, 0, 128);
}

static void boca_o(u8g2_t *u8g2) {
    u8g2_DrawCircle(u8g2, MOUTH_X, MOUTH_Y, 4, U8G2_DRAW_ALL);
}

// ─── Ojos ───────────────────────────────────
static void ojo_solido(u8g2_t *u8g2, int x) {
    u8g2_DrawRBox(u8g2, x, EYE_Y, EYE_W, EYE_H, EYE_R);
}

static void ojo_blink(u8g2_t *u8g2, int x) {
    u8g2_DrawBox(u8g2, x, EYE_Y + EYE_H/2 - 1, EYE_W, 4);
}

static void ojo_happy(u8g2_t *u8g2, int x) {
    u8g2_DrawArc(u8g2, x + EYE_W/2, EYE_Y + EYE_H/2 + 6, EYE_W/2 - 1, 0, 128);
}

static void ojo_sad(u8g2_t *u8g2, int x) {
    u8g2_DrawArc(u8g2, x + EYE_W/2, EYE_Y + EYE_H/2 - 6, EYE_W/2 - 1, 128, 255);
    u8g2_DrawBox(u8g2, x + EYE_W - 5, EYE_Y + EYE_H/2 + 4, 2, 5);
}

static void ojo_sorprendido(u8g2_t *u8g2, int x) {
    u8g2_DrawRBox(u8g2, x - 2, EYE_Y - 3, EYE_W + 4, EYE_H + 5, EYE_R + 2);
}

static void ojo_enojado(u8g2_t *u8g2, int x, int lado) {
    u8g2_DrawRBox(u8g2, x, EYE_Y, EYE_W, EYE_H, EYE_R);
    u8g2_SetDrawColor(u8g2, 0);

    for (int i = 0; i < 7; i++) {
        if (lado < 0)
            u8g2_DrawHLine(u8g2, x + EYE_W - 7 + i, EYE_Y + i, 7 - i);
        else
            u8g2_DrawHLine(u8g2, x, EYE_Y + i, 7 - i);
    }

    u8g2_SetDrawColor(u8g2, 1);
}

static void ojo_sleepy(u8g2_t *u8g2, int x) {
    u8g2_DrawRBox(u8g2, x, EYE_Y, EYE_W, EYE_H, EYE_R);
    u8g2_SetDrawColor(u8g2, 0);
    u8g2_DrawBox(u8g2, x, EYE_Y, EYE_W, EYE_H / 2 + 1);
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_DrawHLine(u8g2, x, EYE_Y + EYE_H / 2, EYE_W);
}

// ─── Render ─────────────────────────────────
void face_render(FaceState *state) {
    u8g2_t *u8g2 = display_get();

    int offset = state->mirada_x;

    u8g2_ClearBuffer(u8g2);

    // ─── OJOS ─────────────────────────────
    if (state->blink) {
        ojo_blink(u8g2, EYE_L_X + offset);
        ojo_blink(u8g2, EYE_R_X + offset);
    } else {
        // 🔴 Fix: NEUTRAL ya no dibuja su boca acá, se unifica abajo
        switch (state->emocion) {
            case EMOCION_NEUTRAL:
                ojo_solido(u8g2, EYE_L_X + offset);
                ojo_solido(u8g2, EYE_R_X + offset);
                break;

            case EMOCION_HAPPY:
                ojo_happy(u8g2, EYE_L_X + offset);
                ojo_happy(u8g2, EYE_R_X + offset);
                break;

            case EMOCION_SAD:
            case EMOCION_SAD_HAPPY:
                ojo_sad(u8g2, EYE_L_X + offset);
                ojo_sad(u8g2, EYE_R_X + offset);
                break;

            case EMOCION_SURPRISED:
                ojo_sorprendido(u8g2, EYE_L_X + offset);
                ojo_sorprendido(u8g2, EYE_R_X + offset);
                break;

            case EMOCION_ANGRY:
                ojo_enojado(u8g2, EYE_L_X + offset, -1);
                ojo_enojado(u8g2, EYE_R_X + offset,  1);
                break;

            case EMOCION_SLEEPY:
                ojo_sleepy(u8g2, EYE_L_X + offset);
                ojo_sleepy(u8g2, EYE_R_X + offset);
                break;

            default:
                ojo_solido(u8g2, EYE_L_X + offset);
                ojo_solido(u8g2, EYE_R_X + offset);
                break;
        }
    }

    // ─── BOCA ─────────────────────────────
    // 🔴 Fix: fuera del if/else de blink → la boca persiste durante el parpadeo
    // 🔴 Fix: NEUTRAL unificado acá junto al resto
    switch (state->emocion) {
        case EMOCION_NEUTRAL:
        case EMOCION_HAPPY:
        case EMOCION_SAD_HAPPY:
            boca_sonrisa(u8g2);
            break;

        case EMOCION_SAD:
        case EMOCION_ANGRY:
            boca_triste(u8g2);
            break;

        case EMOCION_SURPRISED:
            boca_o(u8g2);
            break;

        default:
            break;
    }

    u8g2_SendBuffer(u8g2);
}