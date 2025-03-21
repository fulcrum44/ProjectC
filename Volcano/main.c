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
#include "audio.h"

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
Music musica;
Sound sound;
Font texto;

int main() {

    inicializar();

    // Main game loop
    while (!WindowShouldClose() && pantalla >= 0) {
        actualizar_musica(musica);
        delta=GetFrameTime();
        actualizar();
        BeginDrawing();
        dibujar();
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context
    CloseAudioDevice();

    liberar_sonidos();
    liberar_menu();

    return 0;
}

void inicializar() {
    // Init window

    InitWindow(screenWidth, screenHeight, "Volcano");

    int monitor=GetCurrentMonitor();
    int ancho_pantalla=GetMonitorWidth(monitor);
    int alto_pantalla=GetMonitorHeight(monitor);

    SetWindowSize(ancho_pantalla, alto_pantalla);
    SetWindowPosition(0,0);

    ToggleFullscreen();
    SetTargetFPS(60);

    // Inicializamos dipositivo de audio
    InitAudioDevice();

    // Cargamos sonidos
    cargar_sonidos();

    // Inicializamos fuente de texto personalizada
    texto=LoadFontEx(RUTA_FUENTE_PERSONALIZADA, 60, NULL, 0);

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
        (Vector2){(IsWindowFullscreen())? ancho_pantalla/2 : screenWidth/2, (IsWindowFullscreen())? alto_pantalla/2 : screenHeight/2},
        Vector2Zero(),
        0.0f,
        2.5f
    };

    rango_horizontal=((ancho_pantalla/ANCHO_LOSA+10)/2)/camara.zoom; // Le doy un margen de 10 para que vaya sobrado.
    rango_vertical=((alto_pantalla/ALTO_LOSA+10)/2)/camara.zoom;

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
}
