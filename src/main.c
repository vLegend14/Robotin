#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "display.h"
#include "core.h"
#include "faces.h"
#include "clock.h"

#define BTN_EMOCION       GPIO_NUM_3   // botón original → emoción aleatoria
#define BTN_RELOJ         GPIO_NUM_2   // botón nuevo    → muestra el reloj
#define RELOJ_DURACION_MS 5000

// ─── Modos de la app ──────────────────────
typedef enum {
    MODO_ROBOT,
    MODO_RELOJ,
} AppMode;

// ─── Botones ──────────────────────────────
static void btn_init(void) {
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << BTN_EMOCION) | (1ULL << BTN_RELOJ),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&cfg);
}

// Detección de flanco genérica
static bool flanco(gpio_num_t pin, bool *prev) {
    bool actual = (gpio_get_level(pin) == 0);
    bool risen  = actual && !(*prev);
    *prev       = actual;
    return risen;
}

// ─── Robot ────────────────────────────────
static Robot robot;

// ─── Main ─────────────────────────────────
void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(500));

    display_init();
    btn_init();
    robot_init(&robot);

    const uint32_t TICK = 20;

    AppMode  modo       = MODO_ROBOT;
    uint32_t t_boot_ms  = 0;
    uint32_t t_reloj_ms = 0;

    static bool prev_emocion = false;
    static bool prev_reloj   = false;

    while (1) {
        bool btn_emocion = flanco(BTN_EMOCION, &prev_emocion);
        bool btn_reloj   = flanco(BTN_RELOJ,   &prev_reloj);
        t_boot_ms += TICK;

        switch (modo) {

            // ── Modo robot ────────────────────────────
            case MODO_ROBOT:
                // GPIO3: emoción aleatoria (comportamiento original)
                robot_update(&robot, TICK, btn_emocion);

                // GPIO2: activa el reloj 5s sin interrupciones
                if (btn_reloj) {
                    modo       = MODO_RELOJ;
                    t_reloj_ms = 0;
                    clock_render(t_boot_ms);
                } else {
                    FaceState fs = robot_get_face(&robot);
                    face_render(&fs);
                }
                break;

            // ── Modo reloj (5 seg, sin interrupciones) ──
            case MODO_RELOJ:
                // El robot sigue "vivo" internamente pero no se ve
                robot_update(&robot, TICK, false);

                t_reloj_ms += TICK;
                clock_render(t_boot_ms);

                if (t_reloj_ms >= RELOJ_DURACION_MS) {
                    modo = MODO_ROBOT;
                }
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(TICK));
    }
}