#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "raylib.h"
#include "cofres.h"
#include "room.h"
#include "character.h"
#include "mobs.h"
#include "audio.h"

Texture2D items;
Cofre *cofres;
int total_cofres;
int items_recogidos;
int total_items;

extern int *terreno;
extern int alto_losa;
extern int ancho_losa;
extern int alto_sala;
extern int ancho_sala;
extern float delta;
extern Personaje personaje;
extern int cantidad_monstruos;
extern Monstruo *monstruos;
extern Sound *lista_sonidos;

void inicializa_textura_items() {
    items=LoadTexture(TEXTURA_ITEMS);
}

void almacenar_cofre(int indice_losa) { // Estoy pasando parametro el indice del array terreno al ser conceptualmente las losas del mapa
    if (cofres == NULL) {
        // Estas dos siguientes variable se inicializan o reinician solo cuando se está almacenando el primer cofre de un nivel.
        total_cofres=0;
        items_recogidos=0;
        total_items=0;

        cofres=malloc(sizeof(Cofre) * CANTIDAD_INICIAL_COFRES);
        if (cofres == NULL) {
            printf("\nERROR al reservar memoria para cofres");
            exit(-1);
        }
    } else {
        cofres=realloc(cofres, sizeof(Cofre) * (total_cofres + AUMENTO_CANTIDAD_COFRES));
        if (cofres == NULL) {
            printf("\nERROR al reservar memoria para cofres");
            exit(-1);
        }
    }

    printf("\nIndice_losa: %d", indice_losa);


    // Los datos de terrenos siguen una configuración tridimensional, pero están almacenados en un único plano fila a fila.
    // x corresponde a la posición de un dato en una fila determinada. Ajustamos el rango de indice_losa a la cantidad de ancho_sala
    cofres[total_cofres].losa.x=indice_losa%ancho_sala;
    // La y sería la fila de una de las capas (ancho*alto). Limitamos el rango de indice_losa a la cantidad de indices que hay en una capa. Luedo dividimos por el ancho_sala que corresponde con el tamaño de una fila
    // El valor entero del resultado es lo que nos interesa para decirnos la y de ese indice (losa).
    cofres[total_cofres].losa.y=(indice_losa%(alto_sala*ancho_sala))/ancho_sala;

    printf("\nIndice cofre: %d", total_cofres);
    printf("\nCOFRE: %.f - %.f\n\n", cofres[total_cofres].losa.x, cofres[total_cofres].losa.y);

    cofres[total_cofres].id=total_cofres;
    cofres[total_cofres].indice_losa=indice_losa;
    cofres[total_cofres].abierto=false;
    cofres[total_cofres].tiempo=0;
    cofres[total_cofres].hitbox=(Rectangle){cofres[total_cofres].losa.x * ancho_losa, cofres[total_cofres].losa.y * alto_losa+AJUSTE_ORIGEN_Y, ANCHO_HB_COFRE, ALTO_HB_COFRE};
    cofres[total_cofres].item_recolectable=false;
    cofres[total_cofres].monstruo=false;
    cofres[total_cofres].item.tiempo_animacion_item=0;
    cofres[total_cofres].item.transparencia_item=1.0f;
    cofres[total_cofres].indice_monstruo=-1;

    total_cofres++; // Aumentamos después de inicializar las variables para que haya concordancia con los índices y sobretodo no saltarnos cofres[0].
}

bool cofre_pulsado(Vector2 posicion_raton) {
    for (int i=0; i<total_cofres; i++) {
        if (cofres[i].abierto) continue;

        //printf("\nLosas distancia %.f - %.f", fabs(cofres[i].losa.x - personaje.losa.x), fabs(cofres[i].losa.y - personaje.losa.y));
        if (fabs(cofres[i].losa.x - personaje.losa.x) > 1 || fabs(cofres[i].losa.y - personaje.losa.y) > 1) continue; // fabs convierte un valor float o un cálculo con float en valor absoluto.

        if (CheckCollisionPointRec(posicion_raton, cofres[i].hitbox)) { // Pondré un cronometro. Cuando llegue a 0 la variable objeto_recogido será true y ya no se dibujará más
            PlaySound(lista_sonidos[ABRIR_COFRE]);
            animacion_cofre(&cofres[i]);
            if (cofres[i].item_recolectable) items_recogidos++;
            return true;
        } else continue;
    }

    return false; // Devolvemos false solo si durante la iteración nunca se ha devuelto true. No se ha pulsado ningún cofre.
}

void dibujar_hitbox() {
    for (int i=0; i<total_cofres; i++) {
        DrawRectangle(cofres[i].hitbox.x, cofres[i].hitbox.y, cofres[i].hitbox.width, cofres[i].hitbox.height, YELLOW);
    }
}

void liberar_cofres() {
    free(cofres);
    cofres=NULL;
}

void animacion_cofre(Cofre *c) {
    c->tiempo+=(delta);

    printf("\nDelta: %.2f", delta);
    printf("\n%.2f", c->tiempo);
    printf("\nIndice apertura cofre: %d", (c->indice_losa) + (alto_sala*ancho_sala));
    printf("\nTerreno cofre inicial: %d", terreno[(c->indice_losa) + (alto_sala*ancho_sala)]);

    if (c->tiempo >= TIEMPO_FOTOGRAMA_COFRE) {
        for (int i=0; i<(FOTOGRAMAS_COFRE-1)*2; i++) {
            /*if (i >= (FOTOGRAMAS_COFRE-1)) {
                terreno[(c->indice_losa) + (alto_sala*ancho_sala)]--;
                terreno[(c->indice_losa) + (alto_sala*ancho_sala)-ancho_sala]--;
            }*/
            if (i >= (FOTOGRAMAS_COFRE-1)) {
                terreno[(c->indice_losa) + (alto_sala*ancho_sala)]++;
                terreno[(c->indice_losa) + (alto_sala*ancho_sala)-ancho_sala]++;
                c->abierto=true;
            }

            c->tiempo=0;
        }
    }
}

void asignar_cofre_objeto_recolectable() {
    srand(time(NULL));

    int indice_aleatorio=rand()%total_cofres;

    cofres[indice_aleatorio].item_recolectable=true;
    total_items++;
}

void dibujar_recolectable() {
    for (int i=0; i<total_cofres; i++) {

        if (cofres[i].abierto) { // El dibujado del item recolectado solo se hará si en ese cofre hay un item y si ha sido pulsado para abrir.
            if (cofres[i].item_recolectable) {
                cofres[i].item.tiempo_animacion_item+=delta;
                float desplazamiento = -cofres[i].item.tiempo_animacion_item * VELOCIDAD_ITEM; // Ajustamos la velocidad del desplazamiento de la textura.

                cofres[i].item.transparencia_item-=delta; // Delta regularmente es 0.0167f, cantidad razonable para ir disminuyendo la opacidad de la textura que empieza en 1.0f;

                DrawTextureRec(items,
                 (Rectangle){TEXTURA_X_ITEM, TEXTURA_Y_ITEM, ANCHO_ITEM, ALTO_ITEM},
                 (Vector2){(cofres[i].losa.x * ancho_losa), ((cofres[i].losa.y * alto_losa) - alto_losa + desplazamiento)}, // Empezará a dibujarse en la losa superior donde está el cofre e irá subiendo progresivamente hasta desaparecer
                 (ColorAlpha)(WHITE, cofres[i].item.transparencia_item)); // Con ColorAlpha podemos manipular la transparencia (alpha) de la textura a nuestra conveniencia

                if (cofres[i].item.transparencia_item <= 0) cofres[i].item_recolectable=false; // Desactivamos el objeto recolectable una vez el dibujado sea totalmente transparente asi no hay que dibujarlo más.
            }

            if (cofres[i].monstruo) {
                monstruos[cofres[i].indice_monstruo].activo=true; // Activamos el monstruo del cofre.
                monstruos[cofres[i].indice_monstruo].estado=M_PERSIGUIENDO; // Inicialmente el monstruo está parado. Empieza a perseguir al personaje inmediatamente después de activarse
                cofres[i].monstruo=false; // Desactivamos la variable que indica si hay un monstruo en el cofre. Si no desactivamos se seguirá cumpliendo la condición en todos los frames y da lugar a errores inesperados.
            }
        }
    }
}
