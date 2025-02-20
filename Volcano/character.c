#include "stdio.h"
#include "raylib.h"
#include "raymath.h"
#include "character.h"
#include "room.h"

Texture2D sprite;
float duracion_fotograma=TIEMPO_FOTOGRAMA;
int orientacion;

extern float delta;
extern int *terreno;
extern int alto_sala;
extern int ancho_sala;
extern int alto_losa;
extern int ancho_losa;


void inicializa_textura_personaje() { // Ahora mismo va a parecer un poco innecesario pero luego le podemos dar uso de verdad si tenemos varios personajes.
    sprite=LoadTexture(PERSONAJE_QUIETO);
}

void crear_personaje(Personaje *p) {
    p->posicion=(Vector2){200, 200};
    p->direccion_desplazamiento=(Vector2){0,0};
    p->velocidad=110;
    p->textura_activa=PARADO;
    p->tiempo=0;
    p->fotograma_actual=0;
    p->fotograma=(Rectangle){0,0, ANCHO_FOTOGRAMA, ALTO_FOTOGRAMA};
    p->hb_posicion.x=p->posicion.x+HB_X_ORIGEN;
    p->hb_posicion.y=p->posicion.y+HB_Y_ORIGEN;
}

void actualizar_personaje(Personaje *p) {
    actualizar_fotogramas_personaje(p);

    p->estado=PARADO;
    p->direccion_desplazamiento.x=0;
    p->direccion_desplazamiento.y=0;

    if (IsKeyDown(KEY_A)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.x=-1;
        orientacion=ORIENTACION_IZQ;
    }
    if (IsKeyDown(KEY_D)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.x=1;
        orientacion=ORIENTACION_DER;
    }
    if (IsKeyDown(KEY_W)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.y=-1;
        orientacion=ORIENTACION_ARRIBA;
    }
    if (IsKeyDown(KEY_S)) {
        p->estado=CORRIENDO;
        p->direccion_desplazamiento.y=1;
        orientacion=ORIENTACION_ABAJO;
    }

    // Comprobamos el estado del personaje y su dirección cargamos las texturas correspondientes

    if (p->estado == CORRIENDO || p->estado == PARADO) p->fotograma.y=ALTO_FOTOGRAMA * orientacion; // Cambio de direccion en los fotogramas

    if (p->estado == PARADO && !p->textura_activa == PARADO) {
        UnloadTexture(sprite);
        sprite=LoadTexture(PERSONAJE_QUIETO);
        p->textura_activa=PARADO;
    } else if (p->estado == CORRIENDO && !p->textura_activa == CORRIENDO) {
        UnloadTexture(sprite);
        sprite=LoadTexture(PERSONAJE_CORRIENDO);
        p->textura_activa=CORRIENDO;
    }

    // Normalizamos
    p->direccion_desplazamiento=Vector2Normalize(p->direccion_desplazamiento);
    Vector2 destino=Vector2Add(p->posicion, Vector2Scale(p->direccion_desplazamiento, p->velocidad*delta));
    Vector2 destino_hitbox=Vector2Add(p->hb_posicion, Vector2Scale(p->direccion_desplazamiento, p->velocidad*delta));

    // DEBUG
    //printf("\nDestino(%0.2f, %0.2f)", destino.x, destino.y);
    // printf("\n%d", *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (6 * ancho_sala) + 6));

    // Comprobamos si personaje se puede mover en la dirección pulsada
    if (!suelo_transitable(&p->losa, destino_hitbox)) {
        //printf("\nNo transitable");
        return;
    }
    p->posicion=destino;
    p->hb_posicion=destino_hitbox;

}

void actualizar_fotogramas_personaje(Personaje *p) {
    p->tiempo+=delta;
    //printf("\nDelta: %.2f", p->tiempo);
    if (p->tiempo >= duracion_fotograma) {
        p->fotograma_actual++;
        p->fotograma_actual%=FOTOGRAMAS; // Si orientacion_p es 3 el personaje está mirando hacia arriba
        p->tiempo=0;
        p->fotograma.x=p->fotograma_actual * ANCHO_FOTOGRAMA;

        //if (p->fotograma_actual == 0) p->fotograma.y+=ALTO_FOTOGRAMA; // Para un animacion en una posicion no hay que bajar en el eje Y
    }

    //printf("\n%d", p->fotograma_actual);
}

void dibujar_personaje(Personaje *p) {

    DrawTextureRec(sprite, p->fotograma, p->posicion, WHITE);
    //DrawTextureRec(sprite, p->area, p->posicion, WHITE);

    //DrawRectangle(p->hb_posicion.x,p->hb_posicion.y,15,7,WHITE);
}

bool suelo_transitable(Vector2 *losa, Vector2 destino) {
    int losa_x, losa_y;
    int esquina=0;
    bool transitable;
    int estado_terreno;


    do {
        transitable=true;
        esquina++;

        // DEBO REVISAR //losa_x=floor(destino.x + (esquina-1%2 * HB_LONG_HORIZONTAL)/ancho_losa);
        //losa_y=floor(destino.y + (esquina/2 * HB_LONG_VERTICAL)/alto_losa);

        switch(esquina) {
            case 1:
                losa_x=floor(destino.x/ancho_losa);
                losa_y=floor(destino.y/alto_losa);
                break;

            case 2:
                losa_x=floor((destino.x+HB_LONG_HORIZONTAL)/ancho_losa);
                losa_y=floor(destino.y/alto_losa);
                break;

            case 3:
                losa_x=floor(destino.x/ancho_losa);
                losa_y=floor((destino.y+HB_LONG_VERTICAL)/alto_losa);
                break;

            case 4:
                losa_x=floor((destino.x+HB_LONG_HORIZONTAL)/ancho_losa);
                losa_y=floor((destino.y+HB_LONG_VERTICAL)/alto_losa);
                break;

        }
        estado_terreno=*(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (losa_y * ancho_sala) + losa_x);
        if (estado_terreno == 0) transitable=false;
    } while (transitable && esquina < 4);

    if (transitable) return true;
    else return false;
}

