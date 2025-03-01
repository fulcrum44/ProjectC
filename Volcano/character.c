#include "stdio.h"
#include "string.h"
#include "raylib.h"
#include "raymath.h"
#include "character.h"
#include "room.h"
#include "teclado.h"

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
extern int total_botones;

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

    // Importante reiniciar estas variables. Si llegados a un frame hemos dejado de movernos no nos interesa acumular cálculos de ejecuciones anteriores. Si seguimos corriendo tampoco queremos que el desplazamiento crezca exponencialmente.
    p->estado=P_PARADO;
    p->direccion_desplazamiento.x=0;
    p->direccion_desplazamiento.y=0;

    // Comprobamos qué tecla estamos pulsando para movernos. Controlamos también si el jugador está pulsado otra tecla que no sea solamente de movimiento del personaje.
    tecla_pulsada();

    // Comprobamos el estado del personaje y su dirección cargamos las texturas correspondientes

    if (p->estado == P_CORRIENDO || p->estado == P_PARADO) p->fotograma.y=ALTO_FOTOGRAMA * orientacion; // Cambio de direccion en los fotogramas

    if (p->estado == P_PARADO && p->textura_activa != P_PARADO) {
        UnloadTexture(sprite); // Ineficiente
        sprite=LoadTexture(PERSONAJE_QUIETO);
        p->textura_activa=P_PARADO;
    } else if (p->estado == P_CORRIENDO && p->textura_activa != P_CORRIENDO) {
        UnloadTexture(sprite); // Ineficiente
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
    if (!suelo_transitable(destino_hitbox, PERSONAJE)) {
        return;
    }

    p->posicion=destino;
    p->hb_posicion=destino_hitbox;
    p->losa=conversion_coordenadas_losa(p->posicion);

    //printf("\n%.f - %.f", p->losa.x, p->losa.y);

}

void actualizar_fotogramas_personaje(Personaje *p) {
    p->tiempo+=delta;
    //printf("\nDelta: %.2f", p->tiempo);
    if (p->tiempo >= duracion_fotograma) {
        p->fotograma_actual++;
        p->fotograma_actual%=FOTOGRAMAS; // Si orientacion_p es 3 el personaje está mirando hacia arriba
        p->tiempo=0;
        p->fotograma.x=p->fotograma_actual * ANCHO_FOTOGRAMA;
    }
}

void dibujar_personaje(Personaje *p) {

    //DrawTextureRec(sprite, p->fotograma, p->posicion, WHITE);
    DrawTexturePro(sprite, p->fotograma, (Rectangle){p->posicion.x, p->posicion.y, ANCHO_FOTOGRAMA, ALTO_FOTOGRAMA}, (Vector2){31, 43}, 0.0f, WHITE);

    //DrawRectangle(p->hb_posicion.x,p->hb_posicion.y,HB_LONG_HORIZONTAL,HB_LONG_VERTICAL,WHITE);
}

bool suelo_transitable(Vector2 destino, int sujeto) { // Antes le pasabamos el puntero a Vector2 losa. Puede que más adelante lo use.
    if (!hb_esquina((Vector2){destino.x, destino.y}, sujeto)) return false;
    if (!hb_esquina((Vector2){destino.x+HB_LONG_HORIZONTAL, destino.y}, sujeto)) return false;
    if (!hb_esquina((Vector2){destino.x+HB_LONG_HORIZONTAL, destino.y+HB_LONG_VERTICAL}, sujeto)) return false;
    if (!hb_esquina((Vector2){destino.x+HB_LONG_HORIZONTAL, destino.y+HB_LONG_VERTICAL}, sujeto)) return false;

    return true;
}

bool hb_esquina(Vector2 esquina, int sujeto) {
    int losa_esquina;

    Vector2 losa=conversion_coordenadas_losa(esquina);

    losa_esquina=*(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + ((int)losa.y * ancho_sala) + (int)losa.x);

    if (losa_esquina == SUELO_NO_TRANSITABLE) return false;

    if (sujeto == MONSTRUO) return true;

    if (losa_esquina == PUERTA_SIGUIENTE_NIVEL) siguiente_nivel();

    if (losa_esquina == BOTON) {
        *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + ((int)losa.y * ancho_sala) + (int)losa.x)=BOTON_PULSADO;
        total_botones--;

        if (total_botones <= 0) { // Todos los botones tienen que haber sido pulsados para que la reja se abra
            *(terreno + (CAPA_COLISION * ancho_sala * alto_sala) + (losa_y_reja * ancho_sala) + losa_x_reja)=0;
            *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (losa_y_reja * ancho_sala) + losa_x_reja)=REJA_ABIERTA;
        }
    }

    return true;
}

Vector2 conversion_coordenadas_losa(Vector2 coordenada) {
    Vector2 losa;

    losa.x=floor(coordenada.x/ancho_losa);
    losa.y=floor(coordenada.y/alto_losa);

    return losa;
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

void movimiento_personaje(Personaje *p, Vector2 desplazamiento, int orientacion_final) {
    // Primero vamos a proceder como si el destino al que queremos movernos fuese transitable y normalizamos el desplazamiento para evitar imprecisiones.
    desplazamiento=Vector2Normalize(desplazamiento);
    Vector2 destino=Vector2Add(p->hb_posicion, Vector2Scale(desplazamiento, p->velocidad*delta));

    // Comprobamos si de verdad nos podemos mover en la direccion pretendida. Comprobamos en los dos ejes.
    bool transitable_x=suelo_transitable((Vector2){destino.x, p->hb_posicion.y}, PERSONAJE);
    bool transitable_y=suelo_transitable((Vector2){p->hb_posicion.x, destino.y}, PERSONAJE);

    // Creamos un Vector2 que podamos manipular localmente
    Vector2 desplazamiento_definitivo=(Vector2){desplazamiento.x, desplazamiento.y};

    // Anulamos desplazamiento en el eje donde por el que no podamos transitar si fuera el caso.
    if (!transitable_x) desplazamiento_definitivo.x=0;
    if (!transitable_y) desplazamiento_definitivo.y=0;

    // Llegados aquí ya tenemos el desplazamiento correcto.

    p->estado=P_CORRIENDO;
    p->direccion_desplazamiento=desplazamiento_definitivo;
    orientacion=orientacion_final;
}

