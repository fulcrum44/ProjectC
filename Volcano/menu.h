#pragma once

#include "raylib.h"

// PANTALLAS
#define PANTALLA_MENU 0
#define PANTALLA_JUEGO 1
#define PANTALLA_SALIDA -1

#define TEXTURA_MENU "resources\\menu\\menu.png"
#define ESCALADO_IMAGEN 1.5f
#define FONDO_MENU (Color){49, 79, 155, 255}

// DATOS BOTONES MENU PRINCIPAL
#define CANTIDAD_BOTONES_MENU_PRINCIPAL 2
#define TEXTURA_BOTONES "resources\\menu\\buttons.png"
#define ALTO_BOTON 80
#define ANCHO_BOTON 348
#define AJUSTE_Y_BOTONES -250
#define ESPACIO_ENTRE_BOTONES 25

// TITULO
#define TEXTURA_TITULO "resources\\menu\\title.png"
#define AJUSTE_Y_TITULO 100

// DATOS MENU PAUSA
#define CANTIDAD_BOTONES_MENU_PAUSA 3
#define TEXTURA_MENU_PAUSA "resources\\menu_pausa\\menu_pausa.png"
#define TEXTURA_BOTONES_MENU_PAUSA "resources\\menu_pausa\\botones_menu_pausa.png"
#define ALTO_BOTON_PAUSA 44
#define ANCHO_BOTON_PAUSA 340
#define AJUSTE_X_BOTON_PAUSA -5
#define AJUSTE_Y_BOTON_PAUSA 50
#define ESPACIO_ENTRE_BOTONES_PAUSA 25
#define ESCALADO_MENU_PAUSA 2.0f

#define DESPEDIDA "\n\nCerrando juego...\n\n"

typedef enum {
    EMPEZAR_PARTIDA = 1,
    CONTINUAR = 0,
    MENU_PRINCIPAL = 0,
    SALIR = -1
} EtiquetaBotones;

typedef struct {
    int tipo;
    Rectangle area;
    Vector2 posicion;
    Rectangle hitbox;
} Boton;

void inicializar_menu();
void inicializar_menu_pausa();
void iniciar_partida();
void dibujar_menu_principal();
bool boton_menu_principal_pulsado(Vector2);
bool boton_menu_pausa_pulsado(Vector2);
void dibujar_menu_pausa();
void liberar_menu();
void liberar_menu_pausa();
