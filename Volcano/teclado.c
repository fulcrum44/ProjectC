#include "raylib.h"
#include "raymath.h"
#include "teclado.h"
#include "menu.h"
#include "character.h"

Vector2 direccion;
int orientacion_final;
int teclas_pulsadas;

extern Personaje personaje;
extern bool pausa;

MovimientoPersonaje controles[]= {
    {KEY_W, (Vector2){0, -1}, ORIENTACION_ARRIBA},
    {KEY_A, (Vector2){-1, 0}, ORIENTACION_IZQ},
    {KEY_S, (Vector2){0,1}, ORIENTACION_ABAJO},
    {KEY_D, (Vector2){1,0}, ORIENTACION_DER}
};

bool tecla_pulsada() {
    // Reiniciamos variables
    direccion.x=0;
    direccion.y=0;
    orientacion_final=-1;
    teclas_pulsadas=0;

    for (int i=0; i<TECLAS_MOVIMIENTO; i++) {
        if (IsKeyDown(controles[i].tecla)) {
            direccion=Vector2Add(direccion, controles[i].direccion);
            orientacion_final=controles[i].orientacion;
            teclas_pulsadas++;
        }
    }

    if (teclas_pulsadas > 0) movimiento_personaje(&personaje, direccion, orientacion_final);

    if (IsKeyPressed(KEY_P)) pausa=(!pausa)? true : false;


}
