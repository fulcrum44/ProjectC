#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raylib.h"
#include "room.h"
#include "character.h"
#include "mobs.h"
#include "cofres.h"

int nivel_actual;
char nombre_archivo[20];
int cantidad_niveles=0;
char *datos_archivo;
int *terreno;
int ancho_sala;
int alto_sala;
int *col_tileset;
int *losas_tileset;
int *ancho_tile;
int *alto_tile;
int ancho_losa; // A diferencia de ancho_tile y alto_tile, estas dos siguientes corresponden al tileheight y tilewidth del mapa configurado en TILED y no a los de cada tileset.
int alto_losa;
char (*nombre_tileset)[50];
int cantidad_tilesets;
int losa_x_reja;
int losa_y_reja;
int total_botones;

Texture2D volcan;
Texture2D objetos;
Texture2D plantas;
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

    return 0;
}

void inicializa_nivel(int nivel) {
    terreno=NULL;
    datos_archivo=NULL;
    col_tileset=NULL;
    losas_tileset=NULL;
    ancho_tile=NULL;
    alto_tile=NULL;
    nombre_tileset=NULL;
    tiles=NULL;
    char *cursor=NULL;
    nivel_actual=nivel;

    cantidad_tilesets=0; // Inicializamos el contador de tilesets que se leeran en el .tmj del nivel actual. Reiniciamos en caso de no ser el primer nivel.
    total_botones=0; // Inicializamos la cantidad de botones en el nivel. Reiniciamos en caso de que hayamos vuelto al menu principal y empezado de nuevo la partida

    // Construimos nombre del archivo a leer
    sprintf(nombre_archivo, "%s%d%s", NOMBRE_BASE_ARCHIVO, nivel_actual, EXTENSION_ARCHIVO);

    // Leemos los datos del archivo y almacenamos
    datos_archivo=LoadFileText(nombre_archivo);

    // Buscamos los datos comunes referentes a las salas construidas y el tileset usado.
    sscanf(strstr(datos_archivo, ETIQ_ALTO_LOSA)+strlen(ETIQ_ALTO_LOSA), "%d", &alto_losa);
    sscanf(strstr(datos_archivo, ETIQ_ANCHO_SALA)+strlen(ETIQ_ANCHO_SALA), "%d", &ancho_sala); // TODAS LAS CAPAS TIENEN LAS MISMAS DIMENSIONES
    sscanf(strstr(datos_archivo, ETIQ_ALTO_SALA)+strlen(ETIQ_ALTO_SALA), "%d", &alto_sala);
    cursor=strstr(datos_archivo, ETIQ_COL_TILESET); // Nos colocomas antes de la primera ocurrencia de la etiqueta. Tenemos dos tilesets, debemos recoger los datos de cada uno.

    col_tileset=malloc(sizeof(int) /* * CANTIDAD_TILESETS*/);
    if (col_tileset == NULL) {
        printf("\nERROR al reservar memoria para col_tileset");
        exit(-1);
    }

    losas_tileset=malloc(sizeof(int) /* * CANTIDAD_TILESETS*/);
    if (losas_tileset == NULL) {
        printf("\nERROR al reservar memoria para losas_tileset");
        exit(-1);
    }

    ancho_tile=malloc(sizeof(int));
    if (ancho_tile == NULL) {
        printf("\nERROR al reservar memoria para ancho_tile");
        exit(-1);
    }

    alto_tile=malloc(sizeof(int));
    if (alto_tile == NULL) {
        printf("\nERROR al reservar memoria para alto_tile");
        exit(-1);
    }

    nombre_tileset=malloc(sizeof(*nombre_tileset));
    if (nombre_tileset == NULL) {
        printf("\nERROR al reservar memoria para nombre_tileset");
        exit(-1);
    }

    /*for (int i=0; i<CANTIDAD_TILESETS; i++) {
        sscanf(strstr(cursor, ETIQ_COL_TILESET)+strlen(ETIQ_COL_TILESET), "%d", &col_tileset[i]);
        cursor=strstr(cursor, ETIQ_LOSAS_TILESET); // Avanzamos al siguiente "tilecount:"
        sscanf(strstr(cursor, ETIQ_LOSAS_TILESET)+strlen(ETIQ_LOSAS_TILESET), "%d", &losas_tileset[i]);
        cursor=strstr(cursor, ETIQ_COL_TILESET); // Avanzamos al siguiente "columns:"
    }*/
    //sscanf(strstr(datos_archivo, ETIQ_ANCHO_LOSA)+strlen(ETIQ_ANCHO_LOSA), "%d", &ancho_losa);
    //sscanf(strstr(cursor, ETIQ_ALTO_LOSA)+strlen(ETIQ_ALTO_LOSA), "%d", &alto_losa);
    //sscanf(strstr(datos_archivo, ETIQ_NOMBRE_TILESET)+strlen(ETIQ_NOMBRE_TILESET), "%[^\"]", nombre_tileset); // No lo estoy usando?

    while (true) {
        sscanf(strstr(cursor, ETIQ_COL_TILESET)+strlen(ETIQ_COL_TILESET), "%d", &col_tileset[cantidad_tilesets]);
        cursor=strstr(cursor, ETIQ_NOMBRE_TILESET);
        sscanf(strstr(cursor, ETIQ_NOMBRE_TILESET)+strlen(ETIQ_NOMBRE_TILESET), "%[^\"]", nombre_tileset[cantidad_tilesets]);
        cursor=strstr(cursor, ETIQ_LOSAS_TILESET); // Avanzamos al siguiente "tilecount:"
        sscanf(strstr(cursor, ETIQ_LOSAS_TILESET)+strlen(ETIQ_LOSAS_TILESET), "%d", &losas_tileset[cantidad_tilesets]);
        cursor=strstr(cursor, ETIQ_ALTO_LOSA); // Avanzamos al siguiente "tileheight:"
        sscanf(strstr(cursor, ETIQ_ALTO_LOSA)+strlen(ETIQ_ALTO_LOSA), "%d", &alto_tile[cantidad_tilesets]);
        cursor=strstr(cursor, ETIQ_ANCHO_LOSA); // Avanzamos al siguiente "tilewidth:"
        sscanf(strstr(cursor, ETIQ_ANCHO_LOSA)+strlen(ETIQ_ANCHO_LOSA), "%d", &ancho_tile[cantidad_tilesets]);

        cantidad_tilesets++;

        if (strstr(cursor, ETIQ_COL_TILESET) != NULL) cursor=strstr(cursor, ETIQ_COL_TILESET); // Avanzamos al siguiente "columns:"
        else { // No hay más tilesets que leer. Leeremos el tilewidth del mapa que está almacenado después de los datos de los tilesets y nos salimos del bucle.
            cursor=strstr(cursor, "}],")+3;
            cursor=strstr(cursor, ETIQ_ANCHO_LOSA);
            sscanf(strstr(cursor, ETIQ_ANCHO_LOSA)+strlen(ETIQ_ANCHO_LOSA), "%d", &ancho_losa);
            break;
        }

        col_tileset=realloc(col_tileset, sizeof(int) * (cantidad_tilesets+1));
        if (col_tileset == NULL) {
            printf("\nERROR al reservar memoria para col_tileset");
            exit(-1);
        }

        losas_tileset=realloc(losas_tileset, sizeof(int) * (cantidad_tilesets+1));
        if (losas_tileset == NULL) {
            printf("\nERROR al reservar memoria para losas_tileset");
            exit(-1);
        }

        ancho_tile=realloc(ancho_tile, sizeof(int) * (cantidad_tilesets+1));
        if (ancho_tile == NULL) {
            printf("\nERROR al reservar memoria para ancho_tile");
            exit(-1);
        }

        alto_tile=realloc(alto_tile, sizeof(int) * (cantidad_tilesets+1));
        if (alto_tile == NULL) {
            printf("\nERROR al reservar memoria para alto_tile");
            exit(-1);
        }

        nombre_tileset=realloc(nombre_tileset, sizeof((*nombre_tileset)) * (cantidad_tilesets+1));
        if (nombre_tileset == NULL) {
            printf("\nERROR al reservar memoria para nombre_tileset");
            exit(-1);
        }

    }

    printf("\nCANTIDAD TILESETS: %d\n", cantidad_tilesets);

    // DEBUG
    printf("\n%d", ancho_sala);
    printf("\n%d", alto_sala);
    for (int i=0; i<cantidad_tilesets; i++) {
        printf("\nColumnas tileset %d: %d", i+1, col_tileset[i]);
        printf("\nNombre del tileset %d: %s", i+1, nombre_tileset[i]);
        printf("\nTotal losas tileset %d: %d", i+1, losas_tileset[i]);
        printf("\nAlto tile tileset %d: %d", i+1, alto_tile[i]);
        printf("\nAncho tile tileset %d: %d", i+1, ancho_tile[i]);
    }
    printf("\n%d", ancho_losa);
    printf("\n%d\n", alto_losa);

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
                //int indice=(terreno + (i * ancho_sala * alto_sala) + (j * ancho_sala) + k);
                int indice=(i * ancho_sala * alto_sala) + (j * ancho_sala) + k;
                sscanf(cursor, "%d", &terreno[indice]);

                //sscanf(cursor, "%d", (terreno + (i * ancho_sala * alto_sala) + (j * ancho_sala) + k));

                // Almacenamos en una variable aparte los botones presentes en el terreno del nivel actual.
                if (terreno[indice] == BOTON) total_botones++;

                // Almacenamos también la posición en losas donde se encuentra la reja cerrada del nivel actual.
                if (terreno[indice] == REJA_CERRADA) {
                    losa_x_reja=k;
                    losa_y_reja=j;
                }

                if (terreno[indice] == 1348) almacenar_cofre(indice);

                if (j == alto_sala - 1 && k == ancho_sala - 1) continue; // No queremos cambiar aún el cursor cuando llegemos al último dato leído de la capa actual.
                cursor=strstr(cursor, ",")+2; // Avanzamos el cursor a nuestra conveniencia.
            }
        }
        cursor=strstr(cursor, ETIQ_DATOS_SALA)+strlen(ETIQ_DATOS_SALA); // El cursor se moverá al inicio de los datos que estamos leyendo de la siguiente capa.
    }

    // Elegimos aleatoriamente el cofre que tendrá el objeto recolectable
    asignar_cofre_objeto_recolectable();

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
    if (cantidad_tilesets >= 1) volcan=LoadTexture(nombre_tileset[0]);
    if (cantidad_tilesets >= 2) objetos=LoadTexture(nombre_tileset[1]);
    if (cantidad_tilesets >= 3) plantas=LoadTexture(nombre_tileset[2]);

    // Array con las texturas de la sala
    int filas_tileset[cantidad_tilesets];

    for (int i=0; i<cantidad_tilesets; i++) filas_tileset[i]=losas_tileset[i]/col_tileset[i];

    tiles = (Rectangle**)malloc(sizeof(Rectangle*) * cantidad_tilesets);
    if (tiles == NULL) {
        printf("\nERROR al reservar memoria para los tiles %d", nivel_actual);
            exit(-1);
    }

    for (int i=0; i<cantidad_tilesets; i++) {
        tiles[i]=(Rectangle*)malloc(sizeof(Rectangle)* (losas_tileset[i] + 1));

        if (tiles[i] == NULL) {
            printf("\nERROR al reservar memoria para los tiles %d", nivel_actual);
            exit(-1);
        }
    }

    for (int i=0; i<cantidad_tilesets; i++) tiles[i][0]=(Rectangle){0,0,0,0}; // Los indices de un tileset empiezan en 1. Inutilizamos la primera posicion del array.


    for (int i=0; i<cantidad_tilesets; i++) {
        for (int j=0; j<filas_tileset[i]; j++) {
            for (int k=0; k<col_tileset[i]; k++) {
                tiles[i][1 + (j * col_tileset[i] + k)]=(Rectangle){k*ancho_tile[i], j*alto_tile[i], ancho_tile[i], alto_tile[i]}; // AQUI ES DONDE HABRIA QUE PONER LOS ALTO Y ANCHO RESPECTIVOS DE CADA TILESET
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

                //printf("\nID_LOSA: %d", id_losa);
                while (id_losa > losas_tileset[indice_tileset]) {
                    id_losa-=losas_tileset[indice_tileset];
                    indice_tileset++;
                }
                /*printf("\nID_LOSA CONVERTIDO: %d", id_losa);
                printf("\nINDICE TILESET: %d", indice_tileset);*/

                Vector2 posicion={k*ancho_losa, j*alto_losa}; // AQUI PUEDE QUE TAMBIEN HABRIA QUE PONER LOS ALTO Y ANCHO RESPECTIVOS DE CADA TILESET
                DrawTextureRec((indice_tileset == 0)? volcan : (indice_tileset == 1)? objetos : plantas , tiles[indice_tileset][id_losa], posicion, WHITE);
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
    free(alto_tile);
    free(ancho_tile);
    free(nombre_tileset);
    libera_monstruos();
    liberar_cofres();
    UnloadTexture(volcan); // Realmente no haría falta ya que en todos los niveles uso las mismas texturas
    UnloadTexture(objetos);
    UnloadTexture(plantas);
}

void siguiente_nivel() {
    finaliza_nivel();
    nivel_actual++;
    inicializa_nivel(nivel_actual);
    posicion_inicial_nivel(&personaje);
    inicializa_monstruos();
}
