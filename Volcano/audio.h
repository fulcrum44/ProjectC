#pragma once

#include "raylib.h"

#define MIN_REPRODUCCION_LAVA 30
#define MAX_REPRODUCCION_LAVA 90

typedef enum {
    AMBIENTE_LAVA,
    MUSICA_03,
    MUSICA_04,
    MUSICA_05
} Tracklist;

typedef enum {
    PASOS,
    ABRIR_COFRE,
    BOTON_PISADO,
    DANYO_RECIBIDO,
    ARMADURA_ENEMIGO,
    ENEMIGO_ATACADO,
    ESPADA
} Sonidos;

void iniciar_musica(float);
void actualizar_musica();
void detener_musica();
void des_silenciar_musica();
void silenciar_musica();
int musica_aleatoria();
void cargar_sonidos();
void liberar_sonidos();
