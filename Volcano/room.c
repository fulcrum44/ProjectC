#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raylib.h"
#include "room.h"
#include "character.h"
#include "mobs.h"

int nivel_actual;
char nombre_archivo[20];
int cantidad_niveles=0;
char *datos_archivo;
int *terreno;
int ancho_sala;
int alto_sala;
int *col_tileset;
int *losas_tileset;
int ancho_losa;
int alto_losa;
int losa_x_reja;
int losa_y_reja;
int total_botones=0;

Texture2D volcan;
Texture2D cofres;
Rectangle **tiles;

extern Personaje personaje;

int preparar_juego() {
    int indice=0;

    while(true) {
        sprintf(nombre_archivo, "%s%d%s", NOMBRE_BASE_ARCHIVO, indice, EXTENSION_ARCHIVO);
        if (FileExists(nombre_archivo)) cantidad_niveles++;
        else break;
        indice++;
    }

    if (cantidad_niveles == 0) return(-1);

    // Empezamos el juego en la sala de preparación
    inicializa_nivel(0);

    return 0;
}

void inicializa_nivel(int nivel) {
    terreno=NULL;
    datos_archivo=NULL;
    col_tileset=NULL;
    losas_tileset=NULL;
    tiles=NULL;
    char *cursor=NULL;
    char nombre_tileset[20];
    nivel_actual=nivel;
    // Construimos nombre del archivo a leer
    sprintf(nombre_archivo, "%s%d%s", NOMBRE_BASE_ARCHIVO, nivel_actual, EXTENSION_ARCHIVO);

    // Leemos los datos del archivo y almacenamos
    datos_archivo=LoadFileText(nombre_archivo);

    // Buscamos los datos comunes referentes a las salas construidas y el tileset usado.
    sscanf(strstr(datos_archivo, ETIQ_ANCHO_SALA)+strlen(ETIQ_ANCHO_SALA), "%d", &ancho_sala); // TODAS LAS CAPAS TIENEN LAS MISMAS DIMENSIONES
    sscanf(strstr(datos_archivo, ETIQ_ALTO_SALA)+strlen(ETIQ_ALTO_SALA), "%d", &alto_sala);
    cursor=strstr(datos_archivo, ETIQ_COL_TILESET); // Nos colocomas antes de la primera ocurrencia de la etiqueta. Tenemos dos tilesets, debemos recoger los datos de cada uno.

    col_tileset=malloc(sizeof(int) * CANTIDAD_TILESETS);
    if (col_tileset == NULL) {
        printf("\nERROR al reservar memoria para col_tileset");
        exit(-1);
    }

    losas_tileset=malloc(sizeof(int) * CANTIDAD_TILESETS);
    if (losas_tileset == NULL) {
        printf("\nERROR al reservar memoria para losas_tileset");
        exit(-1);
    }

    for (int i=0; i<CANTIDAD_TILESETS; i++) {
        sscanf(strstr(cursor, ETIQ_COL_TILESET)+strlen(ETIQ_COL_TILESET), "%d", &col_tileset[i]);
        cursor=strstr(cursor, ETIQ_LOSAS_TILESET); // Avanzamos al siguiente "tilecount:"
        sscanf(strstr(cursor, ETIQ_LOSAS_TILESET)+strlen(ETIQ_LOSAS_TILESET), "%d", &losas_tileset[i]);
        cursor=strstr(cursor, ETIQ_COL_TILESET); // Avanzamos al siguiente "columns:"
    }
    sscanf(strstr(datos_archivo, ETIQ_ANCHO_LOSA)+strlen(ETIQ_ANCHO_LOSA), "%d", &ancho_losa);
    sscanf(strstr(datos_archivo, ETIQ_ALTO_LOSA)+strlen(ETIQ_ALTO_LOSA), "%d", &alto_losa);
    //sscanf(strstr(datos_archivo, ETIQ_NOMBRE_TILESET)+strlen(ETIQ_NOMBRE_TILESET), "%[^\"]", nombre_tileset); // No lo estoy usando?

    // DEBUG
    printf("\n%d", ancho_sala);
    printf("\n%d", alto_sala);
    for (int i=0; i<CANTIDAD_TILESETS; i++) {
        printf("\nColumnas tileset %d: %d", i+1, col_tileset[i]);
        printf("\nTotal losas tileset %d: %d", i+1, losas_tileset[i]);
    }
    printf("\n%d", ancho_losa);
    printf("\n%d\n", alto_losa);
    //printf("%s", nombre_tileset);

    //datos_archivo=LoadFileText(nombre_archivo);

    // Leemos y guardamos los datos propios de la configuración de aspecto de la sala.
    terreno=malloc(sizeof(int)*CANTIDAD_CAPAS_SALA*ancho_sala*alto_sala);
    if (terreno == NULL) {
        printf("\nERROR al reservar memoria para el terreno %d", nivel_actual);
        exit(-1);
    }


    cursor=strstr(datos_archivo, ETIQ_DATOS_SALA)+strlen(ETIQ_DATOS_SALA); // Nos colocamos justo después de la etiqueta usada. Los valores de las capas empiezan a partir de ahí.
    for (int i=0; i<CANTIDAD_CAPAS_SALA; i++){
        for (int j=0; j<alto_sala; j++){
            for (int k=0; k<ancho_sala; k++){
                sscanf(cursor, "%d", (terreno + (i * ancho_sala * alto_sala) + (j * ancho_sala) + k));

                // Almacenamos en una variable aparte los botones presentes en el terreno del nivel actual.
                if (*(terreno + (i * ancho_sala * alto_sala) + (j * ancho_sala) + k) == BOTON) total_botones++;

                // Almacenamos también la posición en losas donde se encuentra la reja cerrada del nivel actual.
                if (*(terreno + (i * ancho_sala * alto_sala) + (j * ancho_sala) + k) == REJA_CERRADA) {
                    losa_x_reja=k;
                    losa_y_reja=j;
                }

                if (j == alto_sala - 1 && k == ancho_sala - 1) continue; // No queremos cambiar aún el cursor cuando llegemos al último dato leído de la capa actual.
                cursor=strstr(cursor, ",")+2; // Avanzamos el cursor a nuestra conveniencia.
            }
        }
        cursor=strstr(cursor, ETIQ_DATOS_SALA)+strlen(ETIQ_DATOS_SALA); // El cursor se moverá al inicio de los datos que estamos leyendo de la siguiente capa.
    }

    //Debug
    /*for (int i=0; i<CANTIDAD_CAPAS_SALA; i++) {
        printf("\n\n Capa: %d", i);
        for (int j=0; j<alto_sala; j++) {
            printf("\n");
            for (int k=0; k<ancho_sala; k++) {
                printf("%d ", *(terreno + (i * ancho_sala * alto_sala) + (j * ancho_sala) + k));
            }
        }
    } */

    // Cargamos texturas
    volcan=LoadTexture("resources\\volcano_set.png");
    cofres=LoadTexture("resources\\craftables.png");

    // Array con las texturas de la sala
    int filas_tileset[CANTIDAD_TILESETS];

    for (int i=0; i<CANTIDAD_TILESETS; i++) filas_tileset[i]=losas_tileset[i]/col_tileset[i];

    tiles = (Rectangle**)malloc(sizeof(Rectangle*) * CANTIDAD_TILESETS);
    if (tiles == NULL) {
        printf("\nERROR al reservar memoria para los tiles %d", nivel_actual);
            exit(-1);
    }

    for (int i=0; i<CANTIDAD_TILESETS; i++) {
        tiles[i]=(Rectangle*)malloc(sizeof(Rectangle)* (losas_tileset[i] + 1));

        if (tiles[i] == NULL) {
            printf("\nERROR al reservar memoria para los tiles %d", nivel_actual);
            exit(-1);
        }
    }

    for (int i=0; i<CANTIDAD_TILESETS; i++) tiles[i][0]=(Rectangle){0,0,0,0}; // Los indices de un tileset empiezan en 1. Inutilizamos la primera posicion del array.


    for (int i=0; i<CANTIDAD_TILESETS; i++) {
        for (int j=0; j<filas_tileset[i]; j++) {
            for (int k=0; k<col_tileset[i]; k++) {
                tiles[i][1 + (j * col_tileset[i] + k)]=(Rectangle){k*ancho_losa, j*alto_losa, ancho_losa, alto_losa};
            }
        }
    }

}

void dibuja_nivel() {
    for (int i=0; i<CANTIDAD_CAPAS_SALA; i++) {
        for (int j=0; j<alto_sala; j++) {
            for (int k=0; k<ancho_sala; k++) {
                int id_losa=terreno[(i * ancho_sala * alto_sala) + (j * ancho_sala) + k];
                int indice_tileset=0;

                if (id_losa == 0) continue;

                //printf("\n%d", losas_tileset[0]);
                //printf("\n%d", id_losa);

                /*if (id_losa >= losas_tileset[0]) {
                    indice_tileset=1;
                    id_losa-=900;
                }*/

                while (id_losa >= losas_tileset[indice_tileset]) {
                    id_losa-=losas_tileset[indice_tileset];
                    indice_tileset++;
                }

                /*if (id_losa <= 0 || id_losa >= losas_tileset[indice_tileset]) {
                    printf("\nERROR: ID_LOSA %d fuera de rango en tileset %d", id_losa, indice_tileset);
                    continue;
                }*/

                Vector2 posicion={k*ancho_losa, j*alto_losa};
                DrawTextureRec((indice_tileset == 0)? volcan : cofres, tiles[indice_tileset][id_losa], posicion, WHITE);
            }
        }
    }
}

void finaliza_nivel() {
    UnloadFileText(datos_archivo);
    free(terreno);
    for (int i=0; i<CANTIDAD_TILESETS; i++) {
        free(tiles[i]);
    }
    free(tiles);
    free(col_tileset);
    free(losas_tileset);
    libera_monstruos();
    UnloadTexture(volcan); // Realmente no haría falta ya que en todos los niveles uso las mismas texturas
    UnloadTexture(cofres);
}

void siguiente_nivel() {
    finaliza_nivel();
    nivel_actual++;
    inicializa_nivel(nivel_actual);
    posicion_inicial_nivel(&personaje);
    inicializa_monstruos();
}
