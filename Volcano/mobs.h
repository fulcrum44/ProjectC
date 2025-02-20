#pragma once

#include "raylib.h"

typedef enum {
    PARADO;
    DEAUMBULANDO,
    ATACANDO,
} Estado;

typedef struct {
    Vector2 posicion;
    Vector2 direccion_desplazamiento;
    int velocidad;
    Vector2 losa;
    float tiempo;
    int fotograma_actual;
    Rectangle fotograma;
    Estado estado;
    Estado textura_activa;
} Enemigo;

void crear_enemigo();
void actualizar_enemigos();
void actualizar_enemigo();
void dibujar_enemigo();
