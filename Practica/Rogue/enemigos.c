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
    enemigos[indice].desplazamiento=(Vector2){0,0};

}

void actualizar_enemigos() {
    // Configuramos el desplazamiento de los enemigos activos
    for (int i=0; i<MAX_ENEMIGOS; i++) {
        if (enemigos[i].activo) {
            // Restamos las coordenadas en las que se ubican personaje y enemigo. Tanto si el resultado es negativo o positivo, obtendremos el desplazamiento que debe realizar el enemigo para llegar hasta el personaje.
            enemigos[i].desplazamiento=Vector2Subtract(ren.posicion, enemigos[i].posicion);

            if (fabs(enemigos[i].desplazamiento.x) >= 200 || fabs(enemigos[i].desplazamiento.y) >= 200) continue;

            // Normalizamos desplazamiento
            enemigos[i].desplazamiento=Vector2Normalize(enemigos[i].desplazamiento);
            Vector2 destino=Vector2Add(enemigos[i].posicion, Vector2Scale(enemigos[i].desplazamiento, enemigos[i].velocidad*delta));

            if (posicion_libre(destino)) enemigos[i].posicion=destino;

            enemigos[i].losa.x=enemigos[i].posicion.x/ANCHO_LOSA;
            enemigos[i].losa.y=enemigos[i].posicion.y/ALTO_LOSA;
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
    }
}

float tiempo_aparicion() {
    return rand()%(MAX_TIEMPO_APARICION-MIN_TIEMPO_APARICION)+MIN_TIEMPO_APARICION;
}

void libera_enemigos() {
    free(nidos);
}
