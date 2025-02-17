#include "raylib.h"
#include "room.h"

void actualizar();
void dibujar();

const int screenWidth = 1440;
const int screenHeight = 1080;
float delta;

int main() {
    // Init window

    InitWindow(screenWidth, screenHeight, "Volcano");
    SetTargetFPS(60);

    // Preparamos el juego. No salimos inmediatamente si ocurre un error
    int juego_preparado=preparar_juego();
    if (juego_preparado != 0) return juego_preparado;


    // Main game loop
    while (!WindowShouldClose()) {
        delta=GetFrameTime();
        //actualizar();
        BeginDrawing();
        ClearBackground(BLACK);
        dibujar();
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}

void dibujar() {
    dibuja_nivel();
}
