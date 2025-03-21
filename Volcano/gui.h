#pragma once

#include "character.h"

// TEXTURAS
#define TEXTURA_BARRA_INFERIOR "resources\\scroll.png"
#define TEXTURA_CUADRO_NIVEL "resources\\level_tag.png"

// BARRA INFERIOR
#define AJUSTE_Y_BARRA_INFERIOR -75

// ELEMENTOS BARRA INFERIOR
#define TAM_FUENTE_ESTANDAR 32
#define TAM_FUENTE_NIVEL 28
#define AJUSTE_POSICION_Y -55
#define AJUSTE_X_TEXTO 42
#define POSICION_X_RELOJ 1200

// CUADRO NIVEL
#define POSICION_X_NIVEL_UN_DIGITO 32
#define POSICION_X_NIVEL_DOBLE_DIGITO 28
#define POSICION_Y_NIVEL 30

// FUENTE DE TEXTO
#define RUTA_FUENTE_PERSONALIZADA "resources\\fonts\\font_default.ttf"

typedef enum {
    FONDO_BARRA_INFERIOR,
    BORDE_IZQUIERDO_BARRA_INFERIOR,
    BORDE_DERECHO_BARRA_INFERIOR,
    CUADRO_NIVEL,
    ICONO_ITEM,
    ICONO_MONSTRUOS_ELIMINADOS,
    ICONO_MUERTES
} EtiquetaIcono;

void inicializar_texturas_gui();
void interfaz_grafica_juego(Personaje);
