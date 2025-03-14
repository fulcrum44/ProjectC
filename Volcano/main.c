#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#include "room.h"
#include "character.h"
#include "mobs.h"
#include "mouse.h"
#include "cofres.h"
#include "menu.h"
#include "teclado.h"
#include "crono.h"
#include "gui.h"

void inicializar();
void actualizar();
void dibujar();

// Resolución de pantalla solo usada para InitWindow. Luego se redimensiona ajustandose a la pantalla donde se esté ejecutando
const int screenWidth = 1440;
const int screenHeight = 1080;

//const int screenWidth = 1080;
//const int screenHeight = 720;

int pantalla;
bool pausa;

float delta;
int rango_horizontal;
int rango_vertical;

Personaje personaje;

Camera2D camara;

int main() {

    inicializar();

    // Main game loop
    while (!WindowShouldClose() && pantalla >= 0) {
        delta=GetFrameTime();
        actualizar();
        BeginDrawing();
        dibujar();
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    liberar_menu();

    return 0;
}

void inicializar() {
    // Init window

    InitWindow(screenWidth, screenHeight, "Volcano");

    /*int monitor=GetCurrentMonitor();
    int ancho_pantalla=GetMonitorWidth(monitor);
    int alto_pantalla=GetMonitorHeight(monitor);

    printf("\nAncho monitor: %d", ancho_pantalla);
    printf("\nAlto monitor: %d", alto_pantalla);

    SetWindowSize(ancho_pantalla, alto_pantalla);

    printf("\nAncho ventana: %d", GetScreenWidth());
    printf("\nAlto ventana: %d", GetScreenHeight()); // Da 9 menos que el del monitor por barra del borde superior

    SetWindowPosition(0,0);

    ToggleFullscreen();*/
    SetTargetFPS(60);

    // Definimos la primera pantalla a mostrar y mantenemos desactivado el menu de pausa.
    pantalla=PANTALLA_MENU;
    pausa=false;

    // Inicializamos el raton
    inicializar_raton();

    // Preparamos el juego. Nos salimos inmediatamente si ocurre un error
    int juego_preparado=preparar_juego();
    if (juego_preparado != 0) exit(-1);

    // Cámara
    camara=(Camera2D) {
        (Vector2){screenWidth/2, screenHeight/2},
        Vector2Zero(),
        0.0f,
        2.0f
    };

    // rango_horizontal=((screenWidth/54+5)/2)/camara.zoom; // Añadimos uno porque es muy fácil estar en un rango que incluya media celda en los lados, por lo que la añadimos directamente en caso de ser así y prevenimos fallos.
    // rango_vertical=((screenHeight/alto_losa+5)/2)/camara.zoom;

    // Menu
    inicializar_menu();

    // Texturas GUI
    inicializar_texturas_gui();

    // El resto de inicializaciones se pueden encontrar en menu.c
}

void actualizar() {
    tecla_pulsada();
    actualizar_raton();

    // Reloj
    formato_hora();

    if (pantalla == PANTALLA_JUEGO && !pausa) {
        actualizar_personaje(&personaje);
        actualizar_monstruos();
        camara.target=personaje.posicion;
    }
}

void dibujar() {
    switch(pantalla) {
        case PANTALLA_MENU:
            ClearBackground(FONDO_MENU);
            dibujar_menu_principal();
            break;

        case PANTALLA_JUEGO:
            ClearBackground(BLACK);
            BeginMode2D(camara);

            dibuja_nivel();
            dibujar_monstruos();
            dibujar_personaje(&personaje);
            dibujar_recolectable();
            if (pausa) dibujar_menu_pausa();
            EndMode2D();

            interfaz_grafica_juego(personaje);
            break;
    }

    //DrawFPS(100, 0);
}
