#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include "stdbool.h"
#include "math.h"

#include "personaje.h"
#include "escenario.h"

#define DIM_TEXTO 14

// Prototipos
void inicializar();
void actualizar();
void dibujar();

// Constantes
const char* TOTAL_LOSAS="TOTAL LOSAS: %d/%d";

// Variables globales
Texture2D rogues;
Texture2D tiles;

Personaje ren;

int screenWidth;
int screenHeight;
int total_losas;
int losas_dibujadas;
int rango_horizontal;
int rango_vertical;
float delta;

Camera2D camara;

int main() {
    inicializar();

    // Main game loop
    while (!WindowShouldClose()) {    // Detect window close button or ESC key
        delta=GetFrameTime();
        actualizar();
        BeginDrawing();
        ClearBackground(BLACK);
        dibujar();
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void inicializar() {
    screenWidth = 1200;
    screenHeight = 675;

    InitWindow(screenWidth, screenHeight, "Rogues");

    SetTargetFPS(60);
    rango_horizontal=(screenWidth/2)/ANCHO_LOSA+1; // Añadimos uno porque es muy fácil estar en un rango que incluya media celda en los lados, por lo que la añadimos directamente en caso de ser así y prevenimos fallos.
    rango_vertical=(screenHeight/2)/ALTO_LOSA+1;

    // Texturas
    rogues = LoadTexture("rogues.png");
    tiles = LoadTexture("tiles.png");

    // Contamos losas
    total_losas=0;
    for (int i=0; i<ALTO_ESCENARIO; i++) {
        for (int j=0; j<ANCHO_ESCENARIO; j++) {
            if (ESCENARIO[i][j]!=0) total_losas++;
        }
    }

    // Preparamos personaje
    ren=crear_personaje();

    // Cámara
    camara=(Camera2D) {
        (Vector2){screenWidth/2, screenHeight/2},
        Vector2Zero(),
        0.0f,
        1.5f
    };
}

void actualizar() {
    actualizar_personaje(&ren);

    // Seguimiento cámara-personaje
    camara.target=ren.posicion;
}

void dibujar() {
    BeginMode2D(camara);

    // Dibujamos el escenario
    losas_dibujadas=dibuja_escenario();

    // Dibujamos el personaje - // preferible que sea lo último dibujado
    dibujar_personaje(&ren);

    EndMode2D();

    // Textos
    // Vector2 pospan=GetWorldToScreen2D(ren.posicion, camara); // Tengo dudas con esto
    DrawText(TextFormat("%d", GetFPS()), 10, 650, DIM_TEXTO, RED);
    DrawText(TextFormat(TOTAL_LOSAS, losas_dibujadas, total_losas), 125, 650, DIM_TEXTO, BLUE);
    DrawText(TextFormat("%.2f,%.2f / %.0f,%.0f", ren.posicion.x, ren.posicion.y, ren.losa.x, ren.losa.y), 350, 650, DIM_TEXTO, YELLOW);
}
