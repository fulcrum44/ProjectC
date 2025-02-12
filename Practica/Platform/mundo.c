#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "mundo.h"
#include "raylib.h"

int cantidad_niveles=0;
int longitud_archivo;
char nombre_archivo[50];
int nivel_actual=0;
char* cadena;
char* cursor;
int alto_nivel;
int ancho_nivel;
int* escenario;
Texture2D atlas;
int col_atlas;
int losas_atlas;
int ancho_losa;
int alto_losa;
Rectangle* areas;

int preparar_juego() {
    int sufijo=0;
    while (true) {
        sufijo++;
        strcpy(nombre_archivo, TextFormat("%s%d%s", NOMBRE_BASE_ARCHIVO, sufijo, EXTENSION_ARCHIVO));
        if (FileExists(nombre_archivo)) cantidad_niveles++;
        else break;
    }

    if (cantidad_niveles > 0) inicializa_nivel(1);

    return cantidad_niveles;
}

void inicializa_nivel(int numero) {
    strcpy(nombre_archivo, TextFormat("%s%d%s", NOMBRE_BASE_ARCHIVO, numero, EXTENSION_ARCHIVO));
    longitud_archivo=GetFileLength(nombre_archivo)/sizeof(char);
    char tipo_objeto[50];
    nivel_actual=numero;
    // Cargar el contenido del archivo en una cadena
    cadena=LoadFileText(nombre_archivo);
    // Localizo altura
    cursor=strstr(cadena, ETIQ_ALTO_ESCENARIO);
    cursor+=strlen(ETIQ_ALTO_ESCENARIO);
    sscanf(cursor, "%d", &alto_nivel);
    // Localizo ancho
    cursor=strstr(cadena, ETIQ_ANCHO_ESCENARIO);
    cursor+=strlen(ETIQ_ANCHO_ESCENARIO);
    sscanf(cursor, "%d", &ancho_nivel);

    // printf("\n %d filas x %d columnas", alto_nivel, ancho_nivel);

    // Hay que dimensionar el array dinámico y cargar los datos en él
    escenario=malloc(sizeof(int) * ancho_nivel * alto_nivel);
    if (escenario == NULL) {
        printf("\nERROR al cargar los datos del nivel %d", numero);
        exit(-1);
    }

    cursor=strstr(cadena, ETIQ_DATOS_ESCENARIO);
    cursor+=strlen(ETIQ_DATOS_ESCENARIO);
    for (int i=0; i<alto_nivel; i++) {
        for (int j=0; j<ancho_nivel; j++) {
            sscanf(cursor, "%d", escenario+(i*ancho_nivel)+j);
            cursor=strstr(cursor, ",")+2;
        }
    }

    //Recupero la información del atlas
    sscanf(strstr(cadena,ETIQ_ANCHO_ESCENARIO)+strlen(ETIQ_ANCHO_ESCENARIO),"%d",&ancho_nivel);
    sscanf(strstr(cadena,ETIQ_ALTO_ESCENARIO)+strlen(ETIQ_ALTO_ESCENARIO),"%d",&alto_nivel);
    sscanf(strstr(cadena,ETIQ_COL_ATLAS)+strlen(ETIQ_COL_ATLAS),"%d",&col_atlas);
    sscanf(strstr(cadena,ETIQ_LOSAS_ATLAS)+strlen(ETIQ_LOSAS_ATLAS),"%d",&losas_atlas);
    sscanf(strstr(cadena,ETIQ_ANCHO_LOSA)+strlen(ETIQ_ANCHO_LOSA),"%d",&ancho_losa);
    sscanf(strstr(cadena,ETIQ_ALTO_LOSA)+strlen(ETIQ_ALTO_LOSA),"%d",&alto_losa);
    char nombre_atlas[50];
    sscanf(strstr(cadena,ETIQ_ATLAS_ESCENARIO)+strlen(ETIQ_ATLAS_ESCENARIO),
           "%[^\"]",nombre_atlas);

    //Cargar la textura
    atlas=LoadTexture(nombre_atlas);
    //Construir el array con las áreas del atlas
    areas=malloc(sizeof(Rectangle)*(losas_atlas+1));
    if(areas==NULL){
        printf("\n---ERROR al reservar espacio para las áreas %d", numero);
        exit(-1);
    }
    int filas_atlas=losas_atlas/col_atlas;
    areas[0]=(Rectangle){0,0,0,0};//Dejo la primera inservible porque los índices de las losas del atalas empiezan en 1

    for(int i=0;i<filas_atlas;i++){
        for(int j=0;j<col_atlas;j++){
            areas[1+i*col_atlas+j]=(Rectangle){j*ancho_losa,i*alto_losa,ancho_losa,alto_losa};
        }
    }

    //Recupero la información de los objetos
    cursor=cadena;
    for(int i=0;i<10;i++){
        cursor=strstr(cursor,ETIQ_TIPO);
        if(cursor==NULL) break;
        cursor+=strlen(ETIQ_TIPO);
        sscanf(cursor,"%[^\"]",tipo_objeto);
        if(strcmp(tipo_objeto,ETIQ_ENEMIGO)==0){
            printf("\n enemigo encontrado");
        }else if(strcmp(tipo_objeto,ETIQ_PROTA)==0){
            printf("\n prota encontrado");
        }
    }
}

void dibuja_nivel() {
    for (int i=0; i<alto_nivel; i++) {
        for (int j=0; j<ancho_nivel; j++) {
            if (escenario[i*ancho_nivel+j] == 0) continue;
            Vector2 posicion={j*ancho_losa, i*alto_losa};
            DrawTextureRec(atlas, areas[escenario[i*ancho_nivel+j]], posicion, WHITE);
        }
    }
}

void finaliza_nivel() {
  free(escenario);
  free(areas);
  UnloadTexture(atlas);
}

void siguiente_nivel() {
    if (nivel_actual == cantidad_niveles) {
        printf("\n ¡¡ HAS VENCIDO !!");
        return;
    }

    finaliza_nivel();
    nivel_actual++;
    inicializa_nivel(nivel_actual);
}
