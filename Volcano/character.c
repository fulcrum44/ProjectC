#include "stdio.h"
#include "string.h"
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
extern char *datos_archivo;
extern int losa_x_reja;
extern int losa_y_reja;


void inicializa_textura_personaje() { // Ahora mismo va a parecer un poco innecesario pero luego le podemos dar uso de verdad si tenemos varios personajes.
    sprite=LoadTexture(PERSONAJE_QUIETO);
}

void crear_personaje(Personaje *p) {
    posicion_inicial_nivel(p); // ESTO DEBE VARIAR SEGÚN EL NIVEL EN EL QUE ENTREMOS
    p->direccion_desplazamiento=(Vector2){0,0};
    p->velocidad=110;
    p->textura_activa=P_PARADO;
    p->tiempo=0;
    p->fotograma_actual=0;
    p->fotograma=(Rectangle){0,0, ANCHO_FOTOGRAMA, ALTO_FOTOGRAMA};
}

void actualizar_personaje(Personaje *p) {
    actualizar_fotogramas_personaje(p);

    p->estado=P_PARADO;
    p->direccion_desplazamiento.x=0;
    p->direccion_desplazamiento.y=0;

    bool arriba = IsKeyDown(KEY_W);
    bool izquierda = IsKeyDown(KEY_A);
    bool abajo = IsKeyDown(KEY_S);
    bool derecha = IsKeyDown(KEY_D);

    if (izquierda) {
        p->estado=P_CORRIENDO;
        p->direccion_desplazamiento.x=-1;
        orientacion=ORIENTACION_IZQ;
    }
    if (derecha) {
        p->estado=P_CORRIENDO;
        p->direccion_desplazamiento.x=1;
        orientacion=ORIENTACION_DER;
    }
    if (arriba) {
        p->estado=P_CORRIENDO;
        p->direccion_desplazamiento.y=-1;
        orientacion=ORIENTACION_ARRIBA;
    }
    if (abajo) {
        p->estado=P_CORRIENDO;
        p->direccion_desplazamiento.y=1;
        orientacion=ORIENTACION_ABAJO;
    }

    if ((derecha || izquierda) && !suelo_transitable(Vector2Add(p->hb_posicion, (Vector2){(derecha)? 1 : -1, 0}))) {
        p->direccion_desplazamiento.x=0;
    }
    if ((arriba || abajo) && !suelo_transitable(Vector2Add(p->hb_posicion, (Vector2){0, (abajo)? 1 : -1}))) {
        p->direccion_desplazamiento.y=0;
    }


    // Comprobamos el estado del personaje y su dirección cargamos las texturas correspondientes

    if (p->estado == P_CORRIENDO || p->estado == P_PARADO) p->fotograma.y=ALTO_FOTOGRAMA * orientacion; // Cambio de direccion en los fotogramas

    if (p->estado == P_PARADO && !p->textura_activa == P_PARADO) {
        UnloadTexture(sprite);
        sprite=LoadTexture(PERSONAJE_QUIETO);
        p->textura_activa=P_PARADO;
    } else if (p->estado == P_CORRIENDO && !p->textura_activa == P_CORRIENDO) {
        UnloadTexture(sprite);
        sprite=LoadTexture(PERSONAJE_CORRIENDO);
        p->textura_activa=P_CORRIENDO;
    }

    // Normalizamos
    p->direccion_desplazamiento=Vector2Normalize(p->direccion_desplazamiento);
    Vector2 destino=Vector2Add(p->posicion, Vector2Scale(p->direccion_desplazamiento, p->velocidad*delta));
    Vector2 destino_hitbox=Vector2Add(p->hb_posicion, Vector2Scale(p->direccion_desplazamiento, p->velocidad*delta));

    // DEBUG
    //printf("\nDestino(%0.2f, %0.2f)", destino.x, destino.y);
    // printf("\n%d", *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (6 * ancho_sala) + 6));

    // Comprobamos si personaje se puede mover en la dirección pulsada. Usamos la hb_posicion para eso.
    if (!suelo_transitable(destino_hitbox)) {
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

    DrawRectangle(p->hb_posicion.x,p->hb_posicion.y,HB_LONG_HORIZONTAL,HB_LONG_VERTICAL,WHITE);
}

bool suelo_transitable(Vector2 destino) { // Antes le pasabamos el puntero a Vector2 losa. Puede que más adelante lo use.
    int losa_x, losa_y;
    int esquina=0;
    bool transitable;
    int estado_terreno;

    // hb_posicion es un punto en la coordenadas, pero vamos a simular un rectangulo moviendo el vector para poder comprobar las cuatro esquinas del rectángulo ficticio
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

        //printf("\n%d", estado_terreno);
        if (estado_terreno == 0) transitable=false;
        if (estado_terreno == 463) siguiente_nivel();
        if (estado_terreno == 776) {
            *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (losa_y * ancho_sala) + losa_x)=777;
            printf("\nREJA: %d - %d", losa_y_reja, losa_x_reja);
            *(terreno + (CAPA_COLISION * ancho_sala * alto_sala) + (losa_y_reja * ancho_sala) + losa_x_reja)=0;
            *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (losa_y_reja * ancho_sala) + losa_x_reja)=854;
        }
    } while (transitable && esquina < 4);

    if (transitable) return true;
    else return false;
}

void posicion_inicial_nivel(Personaje *p) {
    char *cursor;

    cursor=strstr(datos_archivo, ETIQ_NOMBRE_PERSONAJE)+strlen(ETIQ_NOMBRE_PERSONAJE);
    sscanf(strstr(cursor, ETIQ_X_OBJETO)+strlen(ETIQ_X_OBJETO), "%f", &p->posicion.x);
    sscanf(strstr(cursor, ETIQ_Y_OBJETO)+strlen(ETIQ_Y_OBJETO), "%f", &p->posicion.y);

    printf("\nx: %f - y: %f", p->posicion.x, p->posicion.y);

    p->hb_posicion.x=p->posicion.x+HB_X_ORIGEN;
    p->hb_posicion.y=p->posicion.y+HB_Y_ORIGEN;
}

