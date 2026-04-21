#pragma once
#include <stdbool.h>

typedef enum {
    EMOCION_NEUTRAL,
    EMOCION_HAPPY,
    EMOCION_SAD,
    EMOCION_SAD_HAPPY,
    EMOCION_SURPRISED,
    EMOCION_ANGRY,
    EMOCION_SLEEPY,
    EMOCION_COUNT
} Emocion;

typedef struct {
    Emocion emocion;
    int mirada_x;
    bool blink;
} FaceState;

void face_render(FaceState *state);