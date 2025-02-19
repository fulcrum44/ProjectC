#pragma once

#include "raylib.h"

// DATOS FOTOGRAMAS
#define ALTO_FOTOGRAMA 64
#define ANCHO_FOTOGRAMA 64
#define FOTOGRAMAS 12
#define FOTOGRAMAS_ESPALDA 4
#define TIEMPO_FOTOGRAMA 0.15f

// DIMENSIONES Y POSICION HITBOX RELATIVAS PERSONAJE
#define HB_X_ORIGEN 24
#define HB_Y_ORIGEN 37
#define HB_LONG_VERTICAL 7
#define HB_LONG_HORIZONTAL 15

// ORIENTACIONES PERSONAJE
#define ORIENTACION_ABAJO 0
#define ORIENTACION_IZQ 1
#define ORIENTACION_DER 2
#define ORIENTACION_ARRIBA 3

// ANIMACIONES
#define PERSONAJE_QUIETO "resources\\character\\PNG\\Unarmed_Idle\\Unarmed_Idle_full.png"
#define PERSONAJE_CORRIENDO "resources\\character\\PNG\\Unarmed_Run\\Unarmed_Run_full.png"


typedef enum {
    PARADO,
    CORRIENDO,
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
    Vector2 hb_posicion;
    Rectangle hitbox;
} Personaje;

void crear_personaje(Personaje*);
void inicializa_textura_personaje();
void actualizar_personaje(Personaje*);
void dibujar_personaje(Personaje*);
void actualizar_fotogramas_personaje(Personaje*);
bool suelo_transitable(Vector2*, Vector2);


