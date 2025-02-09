#include "raylib.h"

#define MAX_ENEMIGOS 20
#define MIN_TIEMPO_APARICION 3
#define MAX_TIEMPO_APARICION 6

typedef struct {
    Rectangle area;
    Vector2 posicion;
    Vector2 desplazamiento;
    int velocidad;
    Vector2 losa;
    bool activo;
} Enemigo;

extern Texture2D rogues;
extern float delta;

void inicilizar_enemigos();
void crear_enemigo();
void actualizar_enemigos();
void dibujar_enemigos();
void libera_enemigos();
float tiempo_aparicion();

