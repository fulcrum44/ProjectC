#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "raylib.h"
#include "raymath.h"
#include "time.h"
#include "enemigo.h"
#include "crono.h"
#include "escenario.h"
#include "personaje.h"

Vector2 *nidos; // Guardamos la ubicación de los nidos presentes en el terreno
int contador_nidos;
Enemigo enemigos[MAX_ENEMIGOS];
Crono crono;

extern Personaje ren;

void inicilizar_enemigos() {
    srand(time(NULL));
    nidos=NULL;
    contador_nidos=0;

    // Contamos los nidos del terreno
    for (int i=0; i<ALTO_ESCENARIO; i++) { // Coordenada y
        for (int j=0; j<ANCHO_ESCENARIO; j++) { // Coordenada x
            if (ESCENARIO[i][j] == 4) {
                if (contador_nidos == 0) nidos=malloc(sizeof(Vector2));
                else {
                    Vector2 *aux=realloc(nidos, sizeof(Vector2) * (contador_nidos+1));
                    if (aux == NULL) exit(-1);
                    nidos=aux;
                }

                nidos[contador_nidos]=(Vector2){j,i};
                contador_nidos++;
            }
        }
    }

    // Creamos todos los enemigos al inicio de la ejecución del juego. Estarán desactivados de inicio hasta recibir señal de activación
    for (int i=0; i<MAX_ENEMIGOS; i++) {
        enemigos[i].activo=false; // En esta parte del código no son relevantes las otras propiedades del enemigo. Se tratarán en otra función.
    }

    // Los enemigos ya están inicializados. Ya podemos iniciar nuestro cronometro que controlará la aparición de dichos enemigos.
    iniciar_crono(&crono, MIN_TIEMPO_APARICION); // Los segundos que pasan entre aparición de enemigos puede ser fijo o más dinámico con un rango más amplío y posteriormente elegido aleatoriamente

}

void crear_enemigo() {
    // Comprobamos si queda algún enemigo desactivado
    int indice=-1;
    for (int i=0; i<MAX_ENEMIGOS; i++) {
        if (enemigos[i].activo==false) {
            indice=i;
            break;
        }
    }

    if (indice<0) return; // NO hay enemigos aún desactivados. Nos salimos sin crear nada.

    // Asignamos aleatoriamente el nido del enemigo y asignamos la posición del nido como posición inicial de nuestro nuevo enemigo
    Vector2 nido_elegido=nidos[rand()%contador_nidos];

    enemigos[indice].posicion=(Vector2){nido_elegido.x * ANCHO_LOSA, nido_elegido.y * ALTO_LOSA};
    enemigos[indice].area=(Rectangle){160,0,32,32};
    enemigos[indice].activo=true;
    enemigos[indice].velocidad=100;

    do { // De inicio un enemigo suponemos que está patrullando y se mueve hacia un lado cambiando de dirección al encontrarse un muro
        // Queremos solamente -1 ó 1 para determinar el sentido del movimiento
        enemigos[indice].desplazamiento.x=rand()%3-1;
        enemigos[indice].desplazamiento.y=rand()%3-1;
    } while (enemigos[indice].desplazamiento.x == 0 && enemigos[indice].desplazamiento.y == 0 );
    enemigos[indice].estado=PATRULLANDO;
    enemigos[indice].vida=vida_enemigo();
    // Hemos estado usando Rectangle para dibujar un trozo de un spritesheet que es algo puramente estático. Podemos usar igualmente la función para seleccionar una parte de nuestro dibujo en ejecución del juego y hacerlo dinámico como es un hitbox de un personaje o un enemigo
    enemigos[indice].hitbox=(Rectangle){enemigos[indice].posicion.x, enemigos[indice].posicion.y, ANCHO_LOSA, ALTO_LOSA};
    enemigos[indice].textoVida=(Vector2){enemigos[indice].posicion.x+12, enemigos[indice].posicion.y-10};

}

void actualizar_enemigos() {
    // Configuramos el desplazamiento de los enemigos activos
    for (int i=0; i<MAX_ENEMIGOS; i++) {
        if (enemigos[i].activo) {
            actualizar_enemigo(&enemigos[i]);
        }
    }

    actualizar_crono(&crono);

    if (crono_termina(&crono)) {
        crear_enemigo();
        iniciar_crono(&crono, tiempo_aparicion());
    }
}

void dibujar_enemigos() {
    for (int i=0; i<MAX_ENEMIGOS; i++) {
        if (!enemigos[i].activo) continue; // Solo dibujamos los enemigos activos
        DrawTextureRec(rogues, enemigos[i].area, enemigos[i].posicion, WHITE);
        DrawText(TextFormat("%.0f", enemigos[i].vida), enemigos[i].textoVida.x, enemigos[i].textoVida.y, 8, (enemigos[i].vida < 30)? RED : GREEN);
    }
}

float tiempo_aparicion() {
    return rand()%(MAX_TIEMPO_APARICION-MIN_TIEMPO_APARICION)+MIN_TIEMPO_APARICION;
}

void libera_enemigos() {
    free(nidos);
}

float vida_enemigo() {
    return rand()%(MAX_VIDA_ENEMIGOS-MIN_VIDA_ENEMIGOS)+MIN_VIDA_ENEMIGOS;
}

void actualizar_enemigo(Enemigo *e) {
    // Comprobamos el estado del enemigo
    if (CheckCollisionRecs(e->hitbox, ren.hitbox)) e->estado=ATACANDO;
    else if (Vector2Distance(e->posicion, ren.posicion) <= ALCANCE_ATAQUE) e->estado=CAZANDO;
    else e->estado=PATRULLANDO;

    // Acción del enemigo según su estado
    switch (e->estado) {
        case PATRULLANDO:
            break;

        case CAZANDO:
            // Restamos las coordenadas en las que se ubican personaje y enemigo. Tanto si el resultado es negativo o positivo, obtendremos el desplazamiento que debe realizar el enemigo para llegar hasta el personaje.
            e->desplazamiento=Vector2Subtract(ren.posicion, e->posicion);
            break;

        case ATACANDO:
            e->desplazamiento.x=0;
            e->desplazamiento.y=0;
            e->vida-=RATIO_ATAQUE*delta;
            ren.vida-=actualizar_vida_personaje();

            if (e->vida <= 0) e->activo=false; // Enemigo eliminado
            break;
    }

    // Desplazamiento
    // Normalizamos desplazamiento
    e->desplazamiento=Vector2Normalize(e->desplazamiento);
    Vector2 destino=Vector2Add(e->posicion, Vector2Scale(e->desplazamiento, e->velocidad*delta));

    if (posicion_libre(destino)) {
            e->posicion=destino;
            e->losa=(Vector2){floor(e->posicion.x/ANCHO_LOSA), floor(e->posicion.y/ALTO_LOSA)};
            e->hitbox.x=e->posicion.x;
            e->hitbox.y=e->posicion.y;
            e->textoVida.x=e->posicion.x+12;
            e->textoVida.y=e->posicion.y-10;
    } else {
        e->desplazamiento.x*=-1;
        e->desplazamiento.y*=-1;
    }
}
