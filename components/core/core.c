#include "core.h"
#include "esp_random.h"

// ─── Util ──────────────────────────────────
static Emocion emocion_aleatoria(Emocion anterior) {
    Emocion nueva;
    do {
        nueva = (Emocion)(esp_random() % EMOCION_COUNT);
    } while (nueva == anterior);
    return nueva;
}

static Emocion emocion_inteligente(Emocion anterior) {
    Emocion nueva;

    do {
        int r = esp_random() % 100;

        if      (r < 60) nueva = EMOCION_NEUTRAL;
        else if (r < 85) nueva = EMOCION_HAPPY;
        else if (r < 95) nueva = EMOCION_SAD_HAPPY;
        else {
            Emocion raras[] = {
                EMOCION_ANGRY,
                EMOCION_SURPRISED,
                EMOCION_SLEEPY
            };
            nueva = raras[esp_random() % 3];
        }

    } while (nueva == anterior);

    return nueva;
}

static uint32_t blink_para_emocion(Emocion e) {
    switch (e) {
        case EMOCION_SLEEPY:    return  800 + (esp_random() % 500);
        case EMOCION_SURPRISED: return 5000 + (esp_random() % 3000);
        case EMOCION_ANGRY:     return 1500 + (esp_random() % 1000);
        default:                return 3000 + (esp_random() % 2000);
    }
}

// ─── Init ──────────────────────────────────
void robot_init(Robot *r) {
    r->estado = ROBOT_IDLE;

    r->emocion_actual   = EMOCION_NEUTRAL;
    r->emocion_anterior = EMOCION_NEUTRAL;

    r->blink       = false;
    r->t_blink     = 0;
    r->next_blink  = blink_para_emocion(EMOCION_NEUTRAL);

    r->mirada_x    = 0;
    r->t_mirada    = 0;
    r->next_mirada = 2000 + (esp_random() % 3000);

    r->t_estado = 0;

    r->t_emocion              = 0;
    r->proximo_cambio_emocion = 3000 + (esp_random() % 4000);
}

// ─── Update ────────────────────────────────
void robot_update(Robot *r, uint32_t dt, bool boton) {

    // ─── TIMERS ────────────────────────────
    r->t_blink   += dt;
    r->t_mirada  += dt;
    r->t_estado  += dt;
    r->t_emocion += dt;

    // ─── BLINK ─────────────────────────────
    if (r->t_blink >= r->next_blink) {
        r->blink = true;

        if (r->t_blink >= r->next_blink + 120) {
            r->blink      = false;
            r->t_blink    = 0;
            r->next_blink = blink_para_emocion(r->emocion_actual);
        }
    }

    // ─── MIRADA ────────────────────────────
    if (r->t_mirada >= r->next_mirada) {
        r->mirada_x    = (esp_random() % 11) - 5;
        r->t_mirada    = 0;
        r->next_mirada = 2000 + (esp_random() % 3000);
    }

    // ─── CAMBIO AUTOMÁTICO DE EMOCIÓN ──────
    if (r->t_emocion >= r->proximo_cambio_emocion) {
        r->emocion_anterior       = r->emocion_actual;
        r->emocion_actual         = emocion_inteligente(r->emocion_actual);
        r->proximo_cambio_emocion = r->t_emocion + 4000 + (esp_random() % 4000);
        r->next_blink             = blink_para_emocion(r->emocion_actual);
        r->t_blink                = 0;
    }

    // ─── BOTÓN ─────────────────────────────
    // Fuera de la FSM → responde inmediatamente sin importar el estado actual
    if (boton) {
        r->emocion_anterior = r->emocion_actual;
        r->emocion_actual   = emocion_aleatoria(r->emocion_anterior);
        r->next_blink       = blink_para_emocion(r->emocion_actual);
        r->t_blink          = 0;
        r->estado           = ROBOT_EMOCION;
        r->t_estado         = 0;  // reinicia el timer de 2s desde ahora
    }

    // ─── FSM ───────────────────────────────
    switch (r->estado) {

        case ROBOT_IDLE:
            break;

        case ROBOT_EMOCION:
            if (r->t_estado >= 2000) {
                r->estado   = ROBOT_IDLE;
                r->t_estado = 0;
            }
            break;
    }
}

// ─── Getter de estado visual ───────────────
FaceState robot_get_face(const Robot *r) {
    return (FaceState){
        .emocion  = r->emocion_actual,
        .blink    = r->blink,
        .mirada_x = r->mirada_x,
    };
}