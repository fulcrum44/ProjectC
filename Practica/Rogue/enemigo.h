#include "raylib.h"

#define MAX_ENEMIGOS 20
#define MIN_TIEMPO_APARICION 3
#define MAX_TIEMPO_APARICION 6
#define MIN_VIDA_ENEMIGOS 40
#define MAX_VIDA_ENEMIGOS 60
#define ALCANCE_ATAQUE 100
#define RATIO_ATAQUE 10

typedef enum {
    PATRULLANDO,
    CAZANDO,
    ATACANDO,
} Estado;

typedef struct {
    Rectangle area;
    Vector2 posicion;
    Vector2 desplazamiento;
    int velocidad;
    Vector2 losa;
    bool activo;
    int estado;
    float vida;
    Rectangle hitbox;
    Vector2 textoVida;
} Enemigo;

extern Texture2D rogues;
extern float delta;

void inicilizar_enemigos();
void crear_enemigo();
void actualizar_enemigos();
void dibujar_enemigos();
void libera_enemigos();
float tiempo_aparicion();
float vida_enemigo();
void actualizar_enemigo(Enemigo*);
