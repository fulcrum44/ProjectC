#pragma once
#include "raylib.h"
// personaje.h es incluido en varios archivos. No queremos duplicaciones durante la compilación, por ello usamos #pragma once.

#define VIDA_PERSONAJE 100
#define RATIO_ATAQUE 10


typedef struct {
    Rectangle area;
    Vector2 posicion;
    Vector2 desplazamiento;
    int velocidad;
    Vector2 losa;
    Rectangle hitbox;
    float vida;
    Vector2 textoVida;
} Personaje;

Personaje crear_personaje();
void actualizar_personaje(Personaje*);
void dibujar_personaje(Personaje*);
float actualizar_vida_personaje();
