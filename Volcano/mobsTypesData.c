#include "raylib.h"
#include "dimensiones_struct.h"

const int CANTIDAD_MOBS_NIVEL[]={
    1,
    2
};

const Rectangle TEXTURAS[]={
    (Rectangle){296, 804, 112, 80}, // DUENDE_MAGMA
    (Rectangle){418, 804, 112, 80}, // CHISPA_MAGMA
    (Rectangle){148, 0, 64, 96}, // SLIME
    (Rectangle){0, 1389, 64, 80}, // CENTINELA
    (Rectangle){222, 747, 64, 66}, // SETA_MAGMA
    (Rectangle){74, 1389, 64, 144} // CABEZON
};

const Dimensiones FOTOGRAMA[]={
    (Dimensiones){16, 16}, // DUENDE_MAGMA
    (Dimensiones){16, 16}, // CHISPA_MAGMA
    (Dimensiones){16, 16}, // SLIME
    (Dimensiones){16, 16}, // CENTINELA
    (Dimensiones){16, 22}, // SETA_MAGMA
    (Dimensiones){16, 16} // CABEZON
};

const Dimensiones CUADRICULA_FOTOGRAMAS[]={ // Importante: La ultima fila de todas las cuadriculas corresponde con el monstruo muriendo.
    (Dimensiones){5, 7}, // DUENDE_MAGMA
    (Dimensiones){5, 7}, // CHISPA_MAGMA
    (Dimensiones){6, 4}, // SLIME
    (Dimensiones){5, 4}, // CENTINELA
    (Dimensiones){3, 4}, // SETA_MAGMA
    (Dimensiones){5, 7} // CABEZON
};

const Dimensiones HITBOX_COLISIONES[]={
    (Dimensiones){16, 16}, // DUENDE_MAGMA
    (Dimensiones){16, 16}, // CHISPA_MAGMA
    (Dimensiones){16, 13}, // SLIME
    (Dimensiones){1, 16}, // CENTINELA
    (Dimensiones){4, 16}, // SETA_MAGMA
    (Dimensiones){3, 16} // CABEZON
};

const Dimensiones HITBOX_COMBATE[]={ // Incompleto
    (Dimensiones){0, 0}, // DUENDE_MAGMA
    (Dimensiones){0, 0}, // CHISPA_MAGMA
    (Dimensiones){0, 0}, // SLIME
    (Dimensiones){16, 16}, // CENTINELA
    (Dimensiones){0, 0}, // SETA_MAGMA
    (Dimensiones){0, 0} // CABEZON
};

const Vector2 ORIGEN_HB_COLISIONES[]={ // Los vectores que tienen el valor {0,0} no se usarán. El origen del hitbox es el mismo que el de la textura.
    (Vector2){0, 0}, // DUENDE_MAGMA
    (Vector2){0, 0}, // CHISPA_MAGMA
    (Vector2){0, 3}, // SLIME
    (Vector2){0, 15}, // CENTINELA
    (Vector2){0, 15}, // SETA_MAGMA
    (Vector2){0, 13} // CABEZON
};

const int VELOCIDADES[]={
    10,
    10,
    10,
    10,
    10,
    10,
};

const int MONSTRUO_VIDA[]={
    220,
    310,
    415,
    300,
    290,
    215
};

const int DMG[]={ // Daño ataque
    15,
    15,
    23,
    18,
    15,
    18
};

