#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#include "room.h"
#include "character.h"
#include "mobs.h"
#include "mouse.h"
#include "cofres.h"
#include "menu.h"

void inicializar();
void actualizar();
void dibujar();

const int screenWidth = 1440;
const int screenHeight = 1080;

//const int screenWidth = 1080;
//const int screenHeight = 720;

int pantalla;
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

    return 0;
}

void inicializar() {
    // Init window

    InitWindow(screenWidth, screenHeight, "Volcano");
    SetTargetFPS(60);

    // Definimos la primera pantalla a mostrar
    pantalla=PANTALLA_MENU;

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

    // Personaje
    crear_personaje(&personaje);
    inicializa_textura_personaje();

    // Items
    inicializa_textura_items();

    // Monstruos
    inicializa_monstruos();

}

void actualizar() {
    if (pantalla == PANTALLA_JUEGO) {
        actualizar_personaje(&personaje);
        actualizar_monstruos();
        camara.target=personaje.posicion;
    }

    actualizar_raton();
}

void dibujar() {
    switch(pantalla) {
        case 0:
            ClearBackground(FONDO_MENU);
            dibujar_menu_principal();
            break;

        case 1:
            ClearBackground(BLACK);
            BeginMode2D(camara);

            dibuja_nivel();
            dibujar_monstruos();
            dibujar_personaje(&personaje);
            actualizar_raton();
            dibujar_recolectable();
            //dibujar_hitbox();
            DrawText(TextFormat("Posicion: %.2f, %.2f", personaje.posicion.x, personaje.posicion.y), 250, 350, 12, GREEN);
            DrawFPS(250, 450);

            EndMode2D();
            break;
    }
}
