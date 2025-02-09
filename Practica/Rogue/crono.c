#include "stdbool.h"
#include "crono.h"
#include "stddef.h"
#include "raylib.h"

void iniciar_crono(Crono *crono, float tiempo) {
    crono->tiempo=tiempo;
}

void actualizar_crono(Crono *crono) {
    if (crono != NULL && crono->tiempo > 0) crono->tiempo-=GetFrameTime(); // Restamos un segundo
}

bool crono_termina(Crono *crono) {
    if (crono != NULL) return (crono->tiempo <= 0); // Devolvemos true si crono ha terminado
}
