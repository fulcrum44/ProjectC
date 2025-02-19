#include "stdio.h"
#include "raylib.h"
#include "raymath.h"
#include "character.h"
#include "room.h"

Texture2D sprite;
float duracion_fotograma=TIEMPO_FOTOGRAMA;

extern float delta;
extern int *terreno;
extern int alto_sala;
extern int ancho_sala;
extern int alto_losa;
extern int ancho_losa;


void inicializa_personaje() { // Ahora mismo va a parecer un poco innecesario pero luego le podemos dar uso de verdad si tenemos varios personajes.
    sprite=LoadTexture("resources\\character\\PNG\\Unarmed_Idle\\Unarmed_Idle_full.png");
}

void crear_personaje(Personaje *p) {
    //p->area=(Rectangle){64, 64, 32, 32};
    p->posicion=(Vector2){200, 200};
    p->direccion_desplazamiento=(Vector2){0,0};
    p->velocidad=110;
    p->tiempo=0;
    p->fotograma_actual=0;
    p->fotograma=(Rectangle){0,0, ANCHO_FOTOGRAMA, ALTO_FOTOGRAMA};
    p->estado=PARADO;
}

void actualizar_personaje(Personaje *p) {
    actualizar_fotogramas_personaje(p);

    p->direccion_desplazamiento.x=0;
    p->direccion_desplazamiento.y=0;

    if (IsKeyDown(KEY_LEFT)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.x=-1;
    } else if (IsKeyDown(KEY_RIGHT)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.x=1;
    } else if (IsKeyDown(KEY_UP)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.y=-1;
    } else if (IsKeyDown(KEY_DOWN)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.y=1;
    }

    // Normalizamos
    p->direccion_desplazamiento=Vector2Normalize(p->direccion_desplazamiento);
    Vector2 destino=Vector2Add(p->posicion, Vector2Scale(p->direccion_desplazamiento, p->velocidad*delta));

    // printf("\nDestino(%0.2f, %0.2f)", destino.x, destino.y);

   // printf("\n%d", *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (6 * ancho_sala) + 6));

    // Comprobamos si personaje se puede mover en la dirección pulsada
    if (!suelo_transitable(&p->losa, destino)) {
        //printf("\nNo transitable");
        return;
    }
    p->posicion=destino;
}

void actualizar_fotogramas_personaje(Personaje *p) {
    p->tiempo+=delta;
    //printf("\nDelta: %.2f", p->tiempo);
    if (p->tiempo >= duracion_fotograma) {
        p->fotograma_actual++;
        p->fotograma_actual%=FOTOGRAMAS;
        p->tiempo=0;
        p->fotograma.x=p->fotograma_actual * ANCHO_FOTOGRAMA;

        //if (p->fotograma_actual == 0) p->fotograma.y+=ALTO_FOTOGRAMA; // Para un animacion en una posicion no hay que bajar en el eje Y
    }

    //printf("\n%d", p->fotograma_actual);
}

void dibujar_personaje(Personaje *p) {

    DrawTextureRec(sprite, p->fotograma, p->posicion, WHITE);
    //DrawTextureRec(sprite, p->area, p->posicion, WHITE);

    //DrawRectangle(200,200,30,30,WHITE);
}

bool suelo_transitable(Vector2 *losa, Vector2 destino) {
    // El array terreno contiene los datos de cada losa y sus capas de la sala
    // Convertimos coordenadas del destino en losas

    int losa_x=(int)destino.x/ancho_sala;
    int losa_y=(int)destino.y/alto_sala;

    int estado_terreno=*(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (losa_x * ancho_sala) + losa_y);

    printf("\n%d", estado_terreno);

    if (estado_terreno != 0) return true;
    else return false;
}

