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

int pantalla;
bool pausa;
bool fin_partida;

float delta;
int rango_horizontal;
int rango_vertical;

Personaje personaje;

Camera2D camara;
Music musica;
Sound sound;
Image icono;
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

    // Primero recogemos los datos del monitor donde ejecutamos el juego
    int monitor=GetCurrentMonitor();
    int ancho_pantalla=GetMonitorWidth(monitor);
    int alto_pantalla=GetMonitorHeight(monitor);

    // Ajustamos la proporcion de la ventana según el monitor
    float ancho_pantalla_ajustado=ancho_pantalla*0.75;
    float alto_pantalla_ajustado=alto_pantalla*0.75;

    // Dimensionamos la ventana
    SetWindowSize(ancho_pantalla_ajustado, alto_pantalla_ajustado);

    // Posicionamos la ventana al centro
    SetWindowPosition(ancho_pantalla_ajustado/5,alto_pantalla_ajustado/5);

    //ToggleFullscreen(); // Prueba. La intencion es crear un boton dentro del juego donde controlar esto.
    SetTargetFPS(60);

    // Cargamos y establecemos icono para la ventana
    icono=LoadImage("resources\\icono.png");
    SetWindowIcon(icono); // Para verlo desactiva pantalla completa manualmente desde el código.

    // Inicializamos dipositivo de audio
    InitAudioDevice();

    // Cargamos sonidos
    cargar_sonidos();

    // Inicializamos fuente de texto personalizada
    texto=LoadFontEx(RUTA_FUENTE_PERSONALIZADA, 100, NULL, 0);

    // Definimos la primera pantalla a mostrar y mantenemos desactivado el menu de pausa y el estado de fin de partida.
    pantalla=PANTALLA_MENU;
    pausa=false;
    fin_partida=false;

    // Inicializamos el raton
    inicializar_raton();

    // Preparamos el juego. Nos salimos inmediatamente si ocurre un error
    int juego_preparado=preparar_juego();
    if (juego_preparado != 0) exit(-1);

    // Cámara
    camara=(Camera2D) {
        (Vector2){(IsWindowFullscreen())? ancho_pantalla_ajustado/2 : GetScreenWidth()/2, (IsWindowFullscreen())? alto_pantalla_ajustado/2 : GetScreenHeight()/2},
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
            if (pausa && fin_partida) dibujar_menu_fin_partida();
            EndMode2D();

            interfaz_grafica_juego(personaje);
            break;
    }
}
