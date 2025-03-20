#pragma once

#include "raylib.h"
// Se puede ver en mobsTypesData.c que están definidas y organizadas las variables constantes de cada tipo de monstruo existente en el juego
// En el encabezado llamamos a esas variables con extern, siendo accesibles en otros archivos incluyendo dicho encabezado
#include "mobsTypesData.h"

#define TIPOS_MONSTRUO 6

#define DELAY_ATAQUE 60
#define RANGO_PERSECUCION 100
#define RETROCESO 25

// DATOS FOTOGRAMAS
#define TIEMPO_FOTOGRAMA 0.15f

// TEXTURA
#define TEXTURA_MONSTRUOS "resources\\mobs.png"

// ETIQUETAS EXTRACCIÓN DATOS MONSTRUOS
#define ETIQ_NAME_MOBS_SALA "\"name\":\"mobs\","
#define ETIQ_TIPO_OBJETO "\"type\":\""
#define ETIQ_X_OBJETO "\"x\":"
#define ETIQ_Y_OBJETO "\"y\":"


typedef enum { // Las constantes de un enum tienen por defecto un índice empezando desde el 0. Usaremos las constantes para manipular los datos de los monstruos según su tipo
    DUENDE_MAGMA, // 0
    CHISPA_MAGMA, // 1
    SLIME,        // 2
    CENTINELA,    // 3
    SETA_MAGMA,   // 4
    CABEZON       // 5
} EtiquetaMonstruo;

typedef struct {
    int id;
    Rectangle textura; // Area de la textura en la imagen fuente.
    Dimensiones fotograma; // Almacenamos tamaño de un fotograma según el tipo
    Dimensiones cuadricula_fotogramas; // Almacenamos cuantas filas y columnas de fotogramas tiene el area de la textura del tipo de monstruo
    Dimensiones hitbox_colision; // Tamaño del hitbox de colisiones con el entorno del monstruo según el tipo
    Dimensiones hitbox_combate; // Area de golpe del monstruo para ser atacado.
    Vector2 origen_hb_colision; // Punto de origen, segun el tipo de monstruo, para el "dibujado" del hitbox de colisiones con respecto a la posicion del dibujo completo del monstruo
    Vector2 origen_hb_combate; // Punto de origen, segun el tipo de monstruo, para el "dibujado" del hitbox del combate con respecto a la posicion del dibujo completo del monstruo
    int velocidad;
    int vida;
    int dmg; // Daño ataque
} Tipo;

typedef enum {
    M_PARADO,
    M_DEAMBULANDO,
    M_PERSIGUIENDO,
    M_ATACANDO,
    M_ELIMINADO
} EstadoMonstruo;

typedef enum { // Definido en el orden que está en las filas de la textura del personaje.
    ORIENTACION_MONSTRUO_ABAJO,
    ORIENTACION_MONSTRUO_DER,
    ORIENTACION_MONSTRUO_ARRIBA,
    ORIENTACION_MONSTRUO_IZQ
} EtiquetaOrientacionMonstruo;

typedef struct {
    Tipo tipo;
    Vector2 posicion;
    Vector2 direccion_desplazamiento;
    int velocidad;
    Vector2 losa;
    bool activo;
    float tiempo;
    int fotograma_actual;
    Rectangle fotograma;
    EstadoMonstruo estado;
    Vector2 hb_posicion;
    Rectangle hitbox_combate;
    int vida;
    int danyo;
    int duracion_ataque; // Esto es como un delay para que el monstruo no pueda atacar en todos los FPS. En monstruos con ataque de contacto es importante ponerlo si no queremos que se la situación de choque durante varios FPS seguidos y el personaje muera en cuestión de segundos.
} Monstruo;

void inicializa_monstruos();
void actualizar_monstruos();
void actualizar_centinela(Monstruo*);
void actualizar_seta_magma(Monstruo*);
void dibujar_monstruos();
void actualizar_fotogramas_monstruo(Monstruo*);
void libera_monstruos();
void muerte_monstruo(Monstruo*);
void ataque_centinela(Monstruo*);
void ataque_seta_magma(Monstruo*);
void desplazamiento_en_cruz(Monstruo*, Vector2);
void retroceso_monstruo(Monstruo*, int);
EtiquetaMonstruo conversion_char_enum(char*);
