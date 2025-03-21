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

Texture2D *texturas_escenario;
Rectangle **tiles;

extern Personaje personaje;
extern int rango_horizontal;
extern int rango_vertical;

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

    texturas_escenario=malloc(sizeof(Texture2D));
    if (texturas_escenario == NULL) {
        printf("\nERROR al asignar memoria para las texturas del nivel actual");
        exit(-1);
    }

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

        texturas_escenario=realloc(texturas_escenario, sizeof(Texture2D) * (cantidad_tilesets+1));
        if (texturas_escenario == NULL) {
            printf("\nERROR al asignar memoria para las texturas del nivel actual");
            exit(-1);
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

                // Almacenamos en una variable aparte los botones presentes en el terreno del nivel actual.
                if (terreno[indice] == BOTON) total_botones++;

                // Almacenamos también la posición en losas donde se encuentra la reja cerrada del nivel actual.
                if (terreno[indice] == REJA_CERRADA) {
                    losa_x_reja=k;
                    losa_y_reja=j;
                }

                if (terreno[indice] == LOSA_COFRE) almacenar_cofre(indice);

                if (j == alto_sala - 1 && k == ancho_sala - 1) continue; // No queremos cambiar el cursor buscando una coma cuando llegemos al último dato leído de la capa actual. Cuando llege al último elemento de data el cursor se actualizará con el que hay al final del primer for.
                cursor=strstr(cursor, ",")+2; // Avanzamos el cursor a nuestra conveniencia.
            }
        }
        cursor=strstr(cursor, ETIQ_DATOS_SALA)+strlen(ETIQ_DATOS_SALA); // El cursor se moverá al inicio de los datos que estamos leyendo de la siguiente capa.
    }

    // Elegimos aleatoriamente el cofre que tendrá el objeto recolectable
    asignar_cofre_objeto_recolectable();

    printf("\nHOLA");

    // Cargamos texturas
    for (int i=0; i<cantidad_tilesets; i++) {
        texturas_escenario[i]=LoadTexture(nombre_tileset[i]);
    }

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

    // Almacenamos los rectángulos de cada losa o tile de las texturas que se hayan cargado.
    for (int i=0; i<cantidad_tilesets; i++) {
        for (int j=0; j<filas_tileset[i]; j++) {
            for (int k=0; k<col_tileset[i]; k++) {
                tiles[i][1 + (j * col_tileset[i] + k)]=(Rectangle){k*ancho_tile[i], j*alto_tile[i], ancho_tile[i], alto_tile[i]};
            }
        }
    }
}

void dibuja_nivel() {
    int col_min=(personaje.losa.x-rango_horizontal>0)? personaje.losa.x-rango_horizontal : 0;
    int col_max=(personaje.losa.x+rango_horizontal<ancho_sala)? personaje.losa.x+rango_horizontal : ancho_sala;
    int fila_min=(personaje.losa.y-rango_vertical>0)? personaje.losa.y-rango_vertical : 0;
    int fila_max=(personaje.losa.y+rango_vertical<alto_sala)? personaje.losa.y+rango_vertical : alto_sala;

    for (int i=0; i<CANTIDAD_CAPAS_SALA; i++) {
        for (int j=fila_min; j<fila_max; j++) {
            for (int k=col_min; k<col_max; k++) {
                int id_losa=terreno[(i * ancho_sala * alto_sala) + (j * ancho_sala) + k];
                int indice_tileset=0;

                if (id_losa == 0) continue; // No nos molestamos en dibujar una losa cuyo valor es 0. No hay nada que dibujar.

                // En el arreglo terreno los valores de las losas a dibujar son acumulativos entre texturas usadas. Al contrario, en el arreglo tiles que es donde guardamos los rectangulos de las losas, cada textura tiene su propio contador del indice empezando en 1.
                while (id_losa > losas_tileset[indice_tileset]) { // Convertimos el id_losa al valor real que tiene esa losa en su propia textura.
                    id_losa-=losas_tileset[indice_tileset];
                    indice_tileset++;
                }

                Vector2 posicion={k*ancho_losa, j*alto_losa};
                DrawTextureRec(texturas_escenario[indice_tileset], tiles[indice_tileset][id_losa], posicion, WHITE);
            }
        }
    }
}

void finaliza_nivel() {
    UnloadFileText(datos_archivo);
    for (int i=0; i<cantidad_tilesets; i++) { // Puede que no sea lo más eficiente cargar y liberar las texturas para luego volver a cargar todas o casi todas en el próximo nivel. Voy a lo seguro.
       UnloadTexture(texturas_escenario[i]);
    }
    free(terreno);
    for (int i=0; i<CANTIDAD_TILESETS; i++) {
        free(tiles[i]);
    }
    free(texturas_escenario);
    free(tiles);
    free(col_tileset);
    free(losas_tileset);
    free(alto_tile);
    free(ancho_tile);
    free(nombre_tileset);
    libera_monstruos();
    liberar_cofres();
}

void siguiente_nivel() {
    finaliza_nivel();
    nivel_actual++;
    inicializa_nivel(nivel_actual);
    posicion_inicial_nivel(&personaje);
    inicializa_monstruos();
}
