#include "personaje.h"
#include "raymath.h"
#include "escenario.h"

extern float delta;
extern Texture2D rogues;

Personaje crear_personaje() {
    Personaje ren;

    ren.area = (Rectangle){64, 64, 32, 32};
    ren.posicion = (Vector2){200, 200};
    ren.desplazamiento = (Vector2){0, 0};
    ren.velocidad=100;

    return ren;
}

void actualizar_personaje(Personaje *p) {
    p->desplazamiento.x=0;
    p->desplazamiento.y=0;
    if (IsKeyDown(KEY_LEFT)) {
            p->desplazamiento.x-=1;
            p->area.width*=(p->area.width < 0)? 1 : -1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
            p->desplazamiento.x+=1;
            p->area.width*=(p->area.width > 0)? 1 : -1;
    }
    if (IsKeyDown(KEY_UP)) p->desplazamiento.y-=1;
    if (IsKeyDown(KEY_DOWN)) p->desplazamiento.y+=1;

    // Normalizamos la desplazamiento
    p->desplazamiento=Vector2Normalize(p->desplazamiento);
    Vector2 destino=Vector2Add(p->posicion, Vector2Scale(p->desplazamiento, p->velocidad*delta));

    // Comprobamos que podemos movernos en la dirección pulsada
    if (!posicion_libre(destino)) return;
    p->posicion=destino;

    p->losa.x=p->posicion.x/ANCHO_LOSA;
    p->losa.y=p->posicion.y/ALTO_LOSA;
}

void dibujar_personaje(Personaje *p) {
    DrawTextureRec(rogues, p->area, p->posicion, WHITE);
}
