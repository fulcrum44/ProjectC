#pragma once

#define NOMBRE_BASE_ARCHIVO "nivel_"
#define EXTENSION_ARCHIVO ".tmj"

//Etiquetas para extraer la información del escenario
#define ETIQ_ANCHO_ESCENARIO "\"width\":"
#define ETIQ_ALTO_ESCENARIO "\"height\":"
#define ETIQ_DATOS_ESCENARIO "\"data\":["
#define ETIQ_ATLAS_ESCENARIO "\"image\":\""
#define ETIQ_COL_ATLAS "\"columns\":"
#define ETIQ_LOSAS_ATLAS "\"tilecount\":"
#define ETIQ_ANCHO_LOSA "\"tilewidth\":"
#define ETIQ_ALTO_LOSA "\"tileheight\":"
//Etiquetas para extraer la información de los personajes
#define ETIQ_TIPO "\"type\":\""
#define ETIQ_ENEMIGO "enemigo"
#define ETIQ_PROTA "prota"
#define ETIQ_X "\"x\":"
#define ETIQ_Y "\"y\":"

int preparar_juego();
void inicializa_nivel(int);
void dibuja_nivel();
void siguiente_nivel();
void finaliza_nivel();
