#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "faces.h"

// ─── Estados del robot ─────────────────────
typedef enum {
    ROBOT_IDLE,
    ROBOT_EMOCION,
} RobotState;

// ─── Estado interno ────────────────────────
typedef struct {
    RobotState estado;

    Emocion emocion_actual;
    Emocion emocion_anterior;

    // animaciones
    bool blink;
    uint32_t t_blink;
    uint32_t next_blink;

    // mirada
    int mirada_x;
    uint32_t t_mirada;
    uint32_t next_mirada;

    // control de estado FSM (se puede resetear)
    uint32_t t_estado;

    // timer de emoción independiente (nunca se resetea)
    uint32_t t_emocion;
    uint32_t proximo_cambio_emocion;
} Robot;

// ─── API ───────────────────────────────────
void robot_init(Robot *r);
void robot_update(Robot *r, uint32_t dt, bool boton);
FaceState robot_get_face(const Robot *r);