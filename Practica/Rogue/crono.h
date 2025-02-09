#pragma once
#include "stdbool.h"

typedef struct {
    float tiempo;
} Crono;

void iniciar_crono(Crono*, float);
void actualizar_crono(Crono*);
bool crono_termina(Crono*);
