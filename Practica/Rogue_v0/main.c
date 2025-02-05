#include "raylib.h"
#include "raymath.h"

#define ANCHO_ESCENARIO 65
#define ALTO_ESCENARIO 37
#define DIM_TEXTO 14

typedef struct {
    Rectangle area;
    Vector2 posicion;
    Vector2 direccion;
    int velocidad;
    Vector2 losa;
} Personaje;

// Prototipos
void inicializar();
void actualizar();
void dibujar();
bool posicion_libre(Vector2);

// Constantes
const int ANCHO_LOSA=32;
const int ALTO_LOSA=32;
const int ESCENARIO[ALTO_ESCENARIO][ANCHO_ESCENARIO]={
{1,2,2,2,2,2,2,2,1,0,1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{1,3,3,3,3,3,3,3,1,0,1,3,3,3,3,3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{1,3,3,3,3,3,3,3,1,0,1,3,3,3,3,3,3,3,3,1,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
{1,3,3,3,3,3,3,3,2,2,2,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,2,2,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,1,2,2,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3,3,1,0,0,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
{0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
};


const Rectangle REGIONES[4]={
    {0,0,0,0}, // Si es cero no se hace nada
    {0,160,ANCHO_LOSA,ALTO_LOSA},
    {32,160,ANCHO_LOSA,ALTO_LOSA},
    {0,352,ANCHO_LOSA,ALTO_LOSA},
};
const char* TOTAL_LOSAS="TOTAL LOSAS: %d/%d";

Texture2D rogues;
Texture2D tiles;

int total_losas;
int losas_dibujadas;

Camera2D camara;

int rango_horizontal;
int rango_vertical;

Personaje kit;

Vector2 destino;

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    inicializar();
    //--------------------------------------------------------------------------------------


    // Main game loop
    while (!WindowShouldClose()) {    // Detect window close button or ESC key
        BeginDrawing();
        ClearBackground(BLACK);
        actualizar();
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
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Rogues");

    SetTargetFPS(60);

    // Texturas
    rogues = LoadTexture("rogues.png");
    tiles = LoadTexture("tiles.png");

    // Preparamos personaje
    kit.area = (Rectangle){64, 64, 32, 32};
    kit.posicion = (Vector2){200, 200};
    kit.direccion = (Vector2){200, 200};
    kit.velocidad=100;

    // Contamos losas
    for (int i=0; i<ALTO_ESCENARIO; i++) {
        for (int j=0; j<ANCHO_ESCENARIO; j++) {
            if (ESCENARIO[i][j]!=0) total_losas++;
        }
    }

    rango_horizontal=(screenWidth/2)/ANCHO_LOSA;
    rango_vertical=(screenHeight/2)/ALTO_LOSA;

    // Cámara
    camara=(Camera2D) {
        (Vector2){screenWidth/2, screenHeight/2},
        Vector2Zero(),
        0.0f,
        1.5f
    };
}

void actualizar() {
    kit.direccion.x=0;
    kit.direccion.y=0;
    if (IsKeyDown(KEY_LEFT)) kit.direccion.x-=1;
    if (IsKeyDown(KEY_RIGHT)) kit.direccion.x+=1;
    if (IsKeyDown(KEY_UP)) kit.direccion.y-=1;
    if (IsKeyDown(KEY_DOWN)) kit.direccion.y+=1;

    // Normalizamos la direccion
    kit.direccion=Vector2Normalize(kit.direccion);
    destino=Vector2Add(kit.posicion, Vector2Scale(kit.direccion, kit.velocidad*GetFrameTime()));

    if (!posicion_libre(destino)) return;
    kit.posicion=destino;

    kit.losa.x=kit.posicion.x/ANCHO_LOSA;
    kit.losa.y=kit.posicion.y/ALTO_LOSA;

    camara.target=kit.posicion;
}

void dibujar() {
    BeginMode2D(camara);
    // Dibujamos el escenario
    // for (int i=0; i<ALTO_ESCENARIO; i++) {
    //    for (int j=0; j<ANCHO_ESCENARIO; j++) {
    //        if (ESCENARIO[i][j]<=0) continue;
    //        DrawTextureRec(tiles, REGIONES[ESCENARIO[i][j]], (Vector2){j*ANCHO_LOSA, i*ALTO_LOSA}, WHITE);
    //        losas_dibujadas++;
    //    }
    // }
    int col_min=(kit.losa.x-rango_horizontal>0)? kit.losa.x-rango_horizontal : 0;
    int col_max=(kit.losa.x+rango_horizontal<ANCHO_ESCENARIO)? kit.losa.x+rango_horizontal : ANCHO_ESCENARIO;
    int fila_min=(kit.losa.y-rango_vertical>0)? kit.losa.y-rango_vertical : 0;

    int fila_max=(kit.losa.y+rango_vertical<ALTO_ESCENARIO)? kit.losa.y+rango_vertical : ALTO_ESCENARIO;
    losas_dibujadas=0;
    for (int i=fila_min; i<fila_max; i++) {
        for (int j=col_min; j<col_max; j++) {
            if (ESCENARIO[i][j]<=0) continue;
            DrawTextureRec(tiles, REGIONES[ESCENARIO[i][j]], (Vector2){j*ANCHO_LOSA, i*ALTO_LOSA}, WHITE);
            losas_dibujadas++;
        }
    }

    // Dibujamos el personaje

    DrawTextureRec(rogues, kit.area, kit.posicion, WHITE); // preferible dibujar el personaje lo último

    EndMode2D();

    // Texto
    Vector2 pospan=GetWorldToScreen2D(kit.posicion, camara);
    DrawText(TextFormat("%d", GetFPS()), 10, 420, DIM_TEXTO, RED);
    DrawText(TextFormat(TOTAL_LOSAS, losas_dibujadas, total_losas), 125, 420, DIM_TEXTO, RED);
    DrawText(TextFormat("%.2f,%.2f / %.0f,%.0f", kit.posicion.x, kit.posicion.y, kit.losa.x, kit.losa.y), 350, 420, DIM_TEXTO, YELLOW);
}

bool posicion_libre(Vector2) {
    // Traducir posición del personaje a losa, es decir, a celda de la matriz
    int col=(int)(floor(destino.x/ANCHO_LOSA));
    int fila=(int)(floor(destino.y/ALTO_LOSA));

    for (int i=fila; i<fila+1; i++) {
        for (int j=col; j<col+1; j++) {
            if (ESCENARIO[i][j] <= 2) return false;
        }
    }
    return true;
}
