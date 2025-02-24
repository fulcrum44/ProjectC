#pragma once

#include "raylib.h"
// Se puede ver en mobsTypesData.c que están definidas y organizadas las variables constantes de cada tipo de monstruo existente en el juego
// En el encabezado llamamos a esas variables con extern, siendo accesibles en otros archivos incluyendo dicho encabezado
#include "mobsTypesData.h"

#define TIPOS_MONSTRUO 6

// DATOS ESTADO MONSTRUOS
#define PROB_CAMBIO_DIRECCION 2
#define RANGO_VISION 150
#define RANGO_ATAQUE 20

// DATOS FOTOGRAMAS
#define TIEMPO_FOTOGRAMA 0.15f

// ETIQUETAS EXTRACCIÓN DATOS MONSTRUOS
#define ETIQ_OBJETOS_SALA "\"objects\":["
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
    Dimensiones hitbox_colision; // Tamaño del hitbox del monstruo según el tipo
    Vector2 origen_hb; // Punto de origen, segun el tipo de monstruo, para el "dibujado" del hitbox con respecto a la posicion del dibujo completo del monstruo
    int velocidad;
    int vida;
    int dmg; // Daño ataque
} Tipo;

typedef enum {
    M_PARADO,
    M_DEAMBULANDO,
    M_PERSIGUIENDO,
    M_ATACANDO,
} EstadoMonstruo;

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
    EstadoMonstruo textura_activa;
    Vector2 hb_posicion; // Este
    Rectangle hitbox_ataque; // También podemos acceder a eso a traves del tipo
    int vida;
    int ataque_hp;
} Monstruo;

void inicializa_monstruos();
void actualizar_monstruos();
void actualizar_monstruo(Monstruo*);
void dibujar_monstruos();
void actualizar_fotogramas_monstruo(Monstruo*);
void libera_monstruos();
EtiquetaMonstruo conversion_char_enum(char*);
