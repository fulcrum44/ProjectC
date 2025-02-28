#pragma once

#include "raylib.h"

#define PERSONAJE 0
#define MONSTRUO 1

// DATOS FOTOGRAMAS
#define ALTO_FOTOGRAMA 64
#define ANCHO_FOTOGRAMA 64
#define FOTOGRAMAS 12
#define FOTOGRAMAS_ESPALDA 4
#define TIEMPO_FOTOGRAMA 0.15f

// DIMENSIONES Y POSICION HITBOX RELATIVAS PERSONAJE
#define HB_X_ORIGEN -6
#define HB_Y_ORIGEN -6
#define HB_LONG_VERTICAL 7
#define HB_LONG_HORIZONTAL 13

// POSICIONES INCIALES NIVEL
#define ETIQ_NOMBRE_PERSONAJE "\"name\":\"personaje\","
#define ETIQ_X_OBJETO "\"x\":"
#define ETIQ_Y_OBJETO "\"y\":"


// ORIENTACIONES PERSONAJE
#define ORIENTACION_ABAJO 0
#define ORIENTACION_IZQ 1
#define ORIENTACION_DER 2
#define ORIENTACION_ARRIBA 3

// ANIMACIONES
#define PERSONAJE_QUIETO "resources\\character\\PNG\\Unarmed_Idle\\Unarmed_Idle_full.png"
#define PERSONAJE_CORRIENDO "resources\\character\\PNG\\Unarmed_Run\\Unarmed_Run_full.png"

// TIPOS DE SUELO
#define SUELO_NO_TRANSITABLE 0
#define PUERTA_SIGUIENTE_NIVEL 463
#define BOTON 776
#define BOTON_PULSADO 777
#define REJA_ABIERTA 854
#define REJA_CERRADA 827


typedef enum {
    P_PARADO,
    P_CORRIENDO,
    P_ATACANDO,
} EstadoPersonaje;

typedef struct {
    Vector2 posicion;
    Vector2 direccion_desplazamiento;
    int velocidad;
    // Vector2 losa; // No lo estamos usando ahora mismo
    float tiempo;
    int fotograma_actual;
    Rectangle fotograma;
    EstadoPersonaje estado;
    EstadoPersonaje textura_activa;
    Vector2 losa;
    Vector2 hb_posicion;
    Rectangle hitbox_ataque; // Este será el hitbox para el combate
} Personaje;

void crear_personaje(Personaje*);
void inicializa_textura_personaje();
void actualizar_personaje(Personaje*);
void dibujar_personaje(Personaje*);
void actualizar_fotogramas_personaje(Personaje*);
bool suelo_transitable(Vector2, int);
bool hb_esquina(Vector2, int);
void posicion_inicial_nivel(Personaje*);
Vector2 conversion_coordenadas_losa(Vector2);


