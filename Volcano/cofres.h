#pragma once

#include "raylib.h"

// MEMORIA COFRES
#define CANTIDAD_INICIAL_COFRES 1
#define AUMENTO_CANTIDAD_COFRES 1

// DATOS HITBOX COFRES
#define AJUSTE_ORIGEN_Y -3 // Esto es porque la textura de la apertura del cofre son dos losas en otra capa diferente a la del cofre en si. Con este ajuste el hitbox empezará a cubrir desde trocito pequeño de textura de la losa superior
#define ALTO_HB_COFRE 18
#define ANCHO_HB_COFRE 16

#define FOTOGRAMAS_COFRE 3
#define TIEMPO_FOTOGRAMA_COFRE 0.01f

// DATOS ITEM RECOLECTABLE COFRE
#define TEXTURA_ITEMS "resources\\items.png"
#define TEXTURA_X_ITEM 32
#define TEXTURA_Y_ITEM 288
#define ALTO_ITEM 16
#define ANCHO_ITEM 16
#define CANTIDAD_RECOLECTABLES 1
#define FOTOGRAMAS_ITEM 10
#define VELOCIDAD_ITEM 10

typedef struct {
    float tiempo_animacion_item;
    float transparencia_item;
} Item;

typedef struct {
    int id;
    int indice_losa; // Almacenamos el indice del array terreno donde se ubica el cofre
    bool abierto;
    float tiempo;
    Vector2 losa;
    Rectangle hitbox;
    bool item_recolectable;
    Item item;
} Cofre;

void inicializa_textura_items();
void almacenar_cofre(int);
bool cofre_pulsado(Vector2);
void animacion_cofre(Cofre*);
void asignar_cofre_objeto_recolectable();
void dibujar_recolectable();
void liberar_cofres();

void dibujar_hitbox(); // TEMPORAL
