#pragma once

#include "raylib.h"

#define PERSONAJE 0
#define MONSTRUO 1

#define CANTIDAD_HITBOXES 5

// POSICIONES INCIALES NIVEL
#define ETIQ_NOMBRE_PERSONAJE "\"name\":\"personaje\","
#define ETIQ_X_OBJETO "\"x\":"
#define ETIQ_Y_OBJETO "\"y\":"

// TEXTURAS
#define CANTIDAD_TEXTURAS_PERSONAJE 4

// DATOS FOTOGRAMAS
#define ALTO_FOTOGRAMA 64
#define ANCHO_FOTOGRAMA 64
#define FOTOGRAMAS 12
#define FOTOGRAMAS_MUERTE 7
#define TIEMPO_FOTOGRAMA 0.15f
#define TIEMPO_FOTOGRAMA_ATAQUE 0.08f
#define TIEMPO_FOTOGRAMA_MUERTE 0.2f

// DIMENSIONES Y POSICION HITBOX MOVIMIENTO RELATIVAS PERSONAJE
#define HB_X_ORIGEN -6
#define HB_Y_ORIGEN -6
#define HB_LONG_VERTICAL 7
#define HB_LONG_HORIZONTAL 13

// TIPOS DE SUELO
#define SUELO_NO_TRANSITABLE 0
#define PUERTA_SIGUIENTE_NIVEL 463
#define BOTON 776
#define BOTON_PULSADO 777
#define REJA_ABIERTA 854
#define REJA_CERRADA 827

// DIMENSIONES Y POSICIONES HITBOX MUÑECO Y HITBOX ATAQUE


// COMBATE
#define VIDA_PERSONAJE 200
#define DANYO_ATAQUE_PERSONAJE 60


typedef enum {
    P_PARADO,
    P_CORRIENDO,
    P_ATACANDO,
    P_ELIMINADO
} EstadoPersonaje;

typedef enum { // Definido en el orden que está en las filas de la textura del personaje.
    ORIENTACION_ABAJO,
    ORIENTACION_IZQ,
    ORIENTACION_DER,
    ORIENTACION_ARRIBA
} EtiquetaOrientacion;

typedef enum { // LOS INDICES LOS ATAQUE COINCIDEN CON LOS INDICES DE ORIENTACION
    ATAQUE_INFERIOR,
    ATAQUE_IZQUIERDA,
    ATAQUE_DERECHA,
    ATAQUE_SUPERIOR,
    TORSO,
} EtiquetaHitbox;

typedef struct {
    Vector2 posicion;
    Vector2 direccion_desplazamiento;
    int velocidad;
    float tiempo;
    int fotograma_actual;
    int fotogramas_ataque_restantes;
    Rectangle fotograma;
    EstadoPersonaje estado;
    EstadoPersonaje textura_activa;
    Vector2 losa;
    Vector2 hb_posicion;
    Rectangle hitboxes[CANTIDAD_HITBOXES];
    int vida;
    int danyo;
    int total_muertes;
} Personaje;

void crear_personaje(Personaje*);
void inicializa_textura_personaje();
void actualizar_personaje(Personaje*);
void movimiento_personaje(Personaje*, Vector2, int);
void ataque_personaje();
void dibujar_personaje(Personaje*);
void actualizar_fotogramas_personaje(Personaje*);
bool suelo_transitable(Vector2, int);
bool hb_esquina(Vector2, int);
void posicion_inicial_nivel(Personaje*);
void muerte_personaje(Personaje*);
void reinicio_personaje(Personaje*);
Vector2 conversion_coordenadas_losa(Vector2);


