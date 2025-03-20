#include "stdio.h"
#include "raylib.h"
#include "raymath.h"
#include "crono.h"
#include "menu.h"


int horas=0;
int minutos=0;
int segundos=0;
int tiempo_juego=0;
int tiempo_pausado=0;
int tiempo_ejecucion;

extern bool pausa;
extern int pantalla;

void formato_hora() { // Es un cronometro.
    tiempo_ejecucion=(int)GetTime();

    if (pantalla == PANTALLA_JUEGO && !pausa) { // Calculamos el tiempo de juego real
        tiempo_juego=tiempo_ejecucion-tiempo_pausado;
    } else { // Mientras el juego esté en pausa contaremos el tiempo que transcurre estando en el menú de pausa.
        tiempo_pausado=tiempo_ejecucion-tiempo_juego;
        return;
    }

    segundos=tiempo_juego%60;
    minutos=(tiempo_juego/60)%60;
    horas=tiempo_juego/3600;
}
