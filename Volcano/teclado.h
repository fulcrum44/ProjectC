#pragma once

#include "raylib.h"

#define TECLAS_MOVIMIENTO 4

typedef struct {
    int tecla;
    Vector2 direccion;
    int orientacion;
} MovimientoPersonaje;

bool tecla_pulsada();
