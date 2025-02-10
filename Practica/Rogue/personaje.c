#include "personaje.h"
#include "raymath.h"
#include "escenario.h"

extern float delta;
extern Texture2D rogues;

Personaje crear_personaje() {
    Personaje ren;

    ren.area=(Rectangle){64, 64, 32, 32};
    ren.posicion=(Vector2){200, 200};
    ren.desplazamiento=(Vector2){0, 0};
    ren.velocidad=110;
    ren.hitbox=(Rectangle){ren.posicion.x, ren.posicion.y, ANCHO_LOSA, ALTO_LOSA};
    ren.vida=VIDA_PERSONAJE;

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
    //Vector2 destino_texto=Vector2Add(p->textoVida, Vector2Scale(p->desplazamiento, p->velocidad*delta));

    // Comprobamos que podemos movernos en la dirección pulsada
    if (!posicion_libre(destino)) return;
    p->posicion=destino;
    //p->textoVida=destino_texto;

    p->losa.x=p->posicion.x/ANCHO_LOSA;
    p->losa.y=p->posicion.y/ALTO_LOSA;
    p->hitbox.x=p->posicion.x;
    p->hitbox.y=p->posicion.y;
}

float actualizar_vida_personaje() { // Teniendo la función aquí en el archivo de personaje tenemos más control concreto de la vida del personaje sin meternos en el código del enemigo directamente.
    return delta*RATIO_ATAQUE;
}

void dibujar_personaje(Personaje *p) {
    DrawTextureRec(rogues, p->area, p->posicion, WHITE);
    DrawText(TextFormat("%.0f", p->vida), p->posicion.x+12, p->posicion.y-10, 8, (p->vida < 30)? RED : GREEN);
}
