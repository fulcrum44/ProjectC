#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raylib.h"
#include "room.h"
#include "unistd.h"

int nivel_actual;
char nombre_archivo[20];
int cantidad_niveles=0;
char *datos_archivo;
int *terreno;
int ancho_sala;
int alto_sala;
int col_tileset;
int losas_tileset;
int ancho_losa;
int alto_losa;

Texture2D volcan;
Rectangle *tiles;

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
    inicializa_nivel(1);

    return 0;
}

void inicializa_nivel(int nivel) {
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
    sscanf(strstr(datos_archivo, ETIQ_COL_TILESET)+strlen(ETIQ_COL_TILESET), "%d", &col_tileset);
    sscanf(strstr(datos_archivo, ETIQ_LOSAS_TILESET)+strlen(ETIQ_LOSAS_TILESET), "%d", &losas_tileset);
    sscanf(strstr(datos_archivo, ETIQ_ANCHO_LOSA)+strlen(ETIQ_ANCHO_LOSA), "%d", &ancho_losa);
    sscanf(strstr(datos_archivo, ETIQ_ALTO_LOSA)+strlen(ETIQ_ALTO_LOSA), "%d", &alto_losa);
    sscanf(strstr(datos_archivo, ETIQ_NOMBRE_TILESET)+strlen(ETIQ_NOMBRE_TILESET), "%[^\n]", nombre_tileset);

    printf("\n%d", ancho_sala);
    printf("\n%d", alto_sala);
    printf("\n%d", col_tileset);
    printf("\n%d", losas_tileset);
    printf("\n%d", ancho_losa);
    printf("\n%d\n", alto_losa);

    //UnloadFileText(nombre_archivo);
    datos_archivo=LoadFileText(nombre_archivo);

    // Leemos y guardamos los datos propios de la configuración de aspecto de la sala.
    terreno=malloc(sizeof(int)*CANTIDAD_CAPAS_SALA*ancho_sala*alto_sala);
    if (terreno == NULL) {
        printf("\nERROR al reservar memoria para el terreno %d", nivel_actual);
        exit(-1);
    }

    cursor=strstr(datos_archivo, ETIQ_DATOS_SALA)+strlen(ETIQ_DATOS_SALA);
    for (int i=0; i<CANTIDAD_CAPAS_SALA; i++){
        //printf("\nCursor: %d", cursor);
        for (int j=0; j<alto_sala; j++){
            for (int k=0; k<ancho_sala; k++){
                sscanf(cursor, "%d", (terreno + (i * ancho_sala * alto_sala) + (j * ancho_sala) + k));
                if (j == alto_sala - 1 && k == ancho_sala - 1) continue; // No queremos cambiar aún el cursor cuando llegemos al último dato leído de la capa actual.
                cursor=strstr(cursor, ",")+2;
                //printf("\nCursor: %d", cursor);
            }
        }
        // cursor=strstr(datos_archivo, ETIQ_DATOS_SALA)+strlen(ETIQ_DATOS_SALA); /////////////////////// ESTO ESTABA JODIENDO EL DIBUJADO ////////////////////////////
        cursor=strstr(cursor, ETIQ_DATOS_SALA)+strlen(ETIQ_DATOS_SALA); // El cursor se moverá al inicio de los datos que estamos leyendo de la siguiente capa.
    }

    // Cargamos texturas
    volcan=LoadTexture("src\\volcano_set.png");

    // Array con las texturas de la sala
    int filas_tileset=losas_tileset/col_tileset;

    tiles = malloc(sizeof(Rectangle)* (losas_tileset + 1));
    if (tiles == NULL) {
        printf("\nERROR al reservar memoria para los tiles %d", nivel_actual);
            exit(-1);
    }

    tiles[0]=(Rectangle){0,0,0,0}; // Los indices de un tileset empiezan en 0. Inutilizamos la primera posicion del array.

    for (int i=0; i<filas_tileset; i++) {
        for (int j=0; j<col_tileset; j++) {
            tiles[1 + (i * col_tileset) + j]=(Rectangle){j*ancho_losa, i*alto_losa, ancho_losa, alto_losa};
        }
    }

    printf("\nHola");
}

void dibuja_nivel() {
    for (int i=0; i<CANTIDAD_CAPAS_SALA; i++) {
        for (int j=0; j<alto_sala; j++) {
            for (int k=0; k<ancho_sala; k++) {
                int id_losa=terreno[(i * ancho_sala * alto_sala) + (j * ancho_sala) + k];
                Vector2 posicion={k*ancho_losa, j*alto_losa};
                DrawTextureRec(volcan, tiles[id_losa], posicion, WHITE);
            }
        }
    }
}

void finaliza_nivel() {
    free(terreno);
    free(tiles);
    UnloadTexture(volcan); // Realmente no haría falta ya que en todos los niveles uso las mismas texturas
}

void siguiente_nivel() {
    finaliza_nivel();
    nivel_actual++;
    inicializa_nivel(nivel_actual);
}
