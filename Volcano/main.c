#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#include "room.h"
#include "character.h"

void inicializar();
void actualizar();
void dibujar();

const int screenWidth = 1440;
const int screenHeight = 1080;
float delta;
int rango_horizontal;
int rango_vertical;

Personaje personaje;

Camera2D camara;

int main() {

    inicializar();

    // Main game loop
    while (!WindowShouldClose()) {
        delta=GetFrameTime();
        actualizar();
        BeginDrawing();
        ClearBackground(BLACK);
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

    crear_personaje(&personaje);
    inicializa_textura_personaje();

}

void actualizar() {
    actualizar_personaje(&personaje);
    camara.target=personaje.posicion;
}

void dibujar() {
    BeginMode2D(camara);

    dibuja_nivel();
    dibujar_personaje(&personaje);


    EndMode2D();
    DrawText(TextFormat("Posicion: %.2f, %.2f", personaje.posicion.x, personaje.posicion.y), 250, 350, 12, GREEN);
}
