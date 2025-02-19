#pragma once

#include "raylib.h"

#define ALTO_FOTOGRAMA 64
#define ANCHO_FOTOGRAMA 64
#define FOTOGRAMAS 12
#define FOTOGRAMAS_ESPALDA 4
#define TIEMPO_FOTOGRAMA 0.15f

typedef enum {
    PARADO,
    CORRIENDO,
    ATACANDO,
} Estado;

typedef struct {
    Rectangle area;
    Vector2 posicion;
    Vector2 direccion_desplazamiento;
    int velocidad;
    Vector2 losa;
    float tiempo;
    int fotograma_actual;
    Rectangle fotograma;
    Estado estado;
    Rectangle hitbox;
} Personaje;

void inicializa_personaje();
void crear_personaje(Personaje*);
void actualizar_personaje(Personaje*);
void dibujar_personaje(Personaje*);
void actualizar_fotogramas_personaje(Personaje*);
bool suelo_transitable(Vector2*, Vector2);


