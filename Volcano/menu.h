#pragma once

#include "raylib.h"

// PANTALLAS
#define PANTALLA_MENU 0
#define PANTALLA_JUEGO 1
#define PANTALLA_SALIDA -1

#define TEXTURA_MENU "resources\\menu\\menu.png"
#define ESCALADO_IMAGEN 1.5f
#define FONDO_MENU (Color){49, 79, 155, 255}

// TITULO
#define TEXTURA_TITULO "resources\\menu\\title.png"
#define AJUSTE_Y_TITULO 100


// DATOS BOTONES MENU
#define CANTIDAD_BOTONES_MENU 2
#define TEXTURA_BOTONES "resources\\menu\\buttons.png"
#define ALTO_BOTON 80
#define ANCHO_BOTON 348
#define AJUSTE_Y_BOTONES -250
#define ESPACIO_ENTRE_BOTONES 25

#define DESPEDIDA "\n\nCerrando juego...\n\n"

typedef struct {
    int tipo;
    Rectangle area;
    Vector2 posicion;
    Rectangle hitbox;
} Boton;

void inicializar_menu();
void dibujar_menu();
bool boton_menu_pulsado(Vector2);
