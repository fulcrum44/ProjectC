#pragma once

#define NOMBRE_BASE_ARCHIVO "volcano_"
#define EXTENSION_ARCHIVO ".tmj"
#define CANTIDAD_CAPAS_SALA 5
#define CANTIDAD_TILESETS 2

#define NIVEL_INICIAL 0

// ETIQUETAS EXTRACCIÓN DATOS ESCENARIO
#define ETIQ_ANCHO_SALA "\"width\":"
#define ETIQ_ALTO_SALA "\"height\":"
#define ETIQ_DATOS_SALA "\"data\":["
#define ETIQ_NOMBRE_TILESET "\"image\":\""
#define ETIQ_COL_TILESET "\"columns\":"
#define ETIQ_LOSAS_TILESET "\"tilecount\":"
#define ETIQ_ANCHO_LOSA "\"tilewidth\":"
#define ETIQ_ALTO_LOSA "\"tileheight\":"

// ETIQUETAS CAPAS
#define CAPA_SUELO 1
#define CAPA_COLISION_1 3
#define CAPA_COLISION 4

int preparar_juego();
void inicializa_nivel(int);
void dibuja_nivel();
void finaliza_nivel();
void siguiente_nivel();


