#include "stdio.h"
#include "raylib.h"
#include "gui.h"
#include "mobs.h"
#include "character.h"
#include "cofres.h"

Texture2D barra_inferior;
Texture2D cuadro_nivel;
int ancho_pantalla=0;
int alto_pantalla=0;

const Rectangle AREA_TEXTURA_ICONOS[]= {
    (Rectangle){0,0, 172, 32}, // FONDO_BARRA_INFERIOR
    (Rectangle){0, 32, 14, 32}, // BORDE_IZQUIERDO_BARRA_INFERIOR
    (Rectangle){158, 32, 14, 32}, // BORDE_DERECHO_BARRA_INFERIOR
    (Rectangle){0, 0, 28, 28}, // CUADRO_NIVEL
    (Rectangle){TEXTURA_X_ITEM, TEXTURA_Y_ITEM, ANCHO_ITEM, ALTO_ITEM}, // ICONO_ITEM
    (Rectangle){0, 1389, 16, 16}, // ICONO_MONSTRUOS_ELIMINADOS
    (Rectangle){96, 576, ANCHO_ITEM, ALTO_ITEM} // ICONO_MUERTES
};

extern Texture2D items;
extern Texture2D sprite_monstruos;
extern Tipo *tipos;
extern int nivel_actual;
extern int total_items;
extern int items_recogidos;
extern int cantidad_monstruos;
extern int monstruos_eliminados;

extern int horas;
extern int minutos;
extern int segundos;

extern Personaje personaje;

extern int screenWidth;

extern Font texto;

void inicializar_texturas_gui() {
    barra_inferior=LoadTexture(TEXTURA_BARRA_INFERIOR);
    cuadro_nivel=LoadTexture(TEXTURA_CUADRO_NIVEL);
}

void interfaz_grafica_juego(Personaje p) {
    ancho_pantalla=GetScreenWidth();
    alto_pantalla=GetScreenHeight();

    /*Igual que tanto en el menú principal como en el menu de pausa hemos usado un escalado para redimensionar y recolocar los elementos de estos acorde al tamaño de la pantalla
    en comparación a los parámetros por defecto (basados en una resolución de referencia), hacemos los mismo aqui con los elementos de la barra inferior en la pantalla de juego*/
    // Calculamos la posición en el eje X de los elementos proporcionalmente al tamaño real de la pantalla en comparación a la referencia.
    float ajuste_horizontal=(float)ancho_pantalla/(float)screenWidth; // Ej: ajuste_horizontal=1.78 -> El ancho de la pantalla es un 178% mayor que el de la referencia.

    Rectangle AREA_DIBUJADO_ICONOS[]= {
        (Rectangle){0, alto_pantalla + AJUSTE_Y_BARRA_INFERIOR, ancho_pantalla, 75}, // FONDO_BARRA_INFERIOR
        (Rectangle){0, alto_pantalla + AJUSTE_Y_BARRA_INFERIOR, AREA_TEXTURA_ICONOS[BORDE_IZQUIERDO_BARRA_INFERIOR].width, 75}, // BORDE_IZQUIERDO_BARRA_INFERIOR
        (Rectangle){ancho_pantalla - AREA_TEXTURA_ICONOS[BORDE_DERECHO_BARRA_INFERIOR].width, alto_pantalla + AJUSTE_Y_BARRA_INFERIOR, AREA_TEXTURA_ICONOS[BORDE_IZQUIERDO_BARRA_INFERIOR].width, 75}, // BORDE_DERECHO_BARRA_INFERIOR
        (Rectangle){18, 22, 42, 42}, // CUADRO_NIVEL
        (Rectangle){300*ajuste_horizontal, alto_pantalla + AJUSTE_POSICION_Y, 32, 32}, // ICONO_ITEM
        (Rectangle){600*ajuste_horizontal, alto_pantalla + AJUSTE_POSICION_Y, 32, 32}, // ICONO_MONSTRUOS_ELIMINADOS
        (Rectangle){900*ajuste_horizontal, alto_pantalla + AJUSTE_POSICION_Y, 32, 32} // ICONO_MUERTES
    };

    DrawTexturePro(barra_inferior, AREA_TEXTURA_ICONOS[FONDO_BARRA_INFERIOR], AREA_DIBUJADO_ICONOS[FONDO_BARRA_INFERIOR], (Vector2){0,0}, 0.0f, WHITE);
    DrawTexturePro(barra_inferior, AREA_TEXTURA_ICONOS[BORDE_IZQUIERDO_BARRA_INFERIOR], AREA_DIBUJADO_ICONOS[BORDE_IZQUIERDO_BARRA_INFERIOR], (Vector2){0,0}, 0.0f, WHITE);
    DrawTexturePro(barra_inferior, AREA_TEXTURA_ICONOS[BORDE_DERECHO_BARRA_INFERIOR], AREA_DIBUJADO_ICONOS[BORDE_DERECHO_BARRA_INFERIOR], (Vector2){0,0}, 0.0f, WHITE);
    DrawTexturePro(cuadro_nivel, AREA_TEXTURA_ICONOS[CUADRO_NIVEL], AREA_DIBUJADO_ICONOS[CUADRO_NIVEL], (Vector2){0,0}, 0.0f, WHITE);
    DrawTexturePro(items, AREA_TEXTURA_ICONOS[ICONO_ITEM], AREA_DIBUJADO_ICONOS[ICONO_ITEM], (Vector2){0,0}, 0.0f, WHITE);
    DrawTexturePro(sprite_monstruos, AREA_TEXTURA_ICONOS[ICONO_MONSTRUOS_ELIMINADOS], AREA_DIBUJADO_ICONOS[ICONO_MONSTRUOS_ELIMINADOS], (Vector2){0,0}, 0.0f, WHITE);
    DrawTexturePro(items, AREA_TEXTURA_ICONOS[ICONO_MUERTES], AREA_DIBUJADO_ICONOS[ICONO_MUERTES], (Vector2){0,0}, 0.0f, WHITE);

    DrawTextEx(texto, TextFormat("HP: %d", personaje.vida), (Vector2){100*ajuste_horizontal, alto_pantalla + AJUSTE_POSICION_Y}, TAM_FUENTE_ESTANDAR, 0.0f, WHITE);
    DrawTextEx(texto, TextFormat("%d", nivel_actual), (Vector2){(nivel_actual<10)? POSICION_X_NIVEL_UN_DIGITO : POSICION_X_NIVEL_DOBLE_DIGITO, POSICION_Y_NIVEL}, TAM_FUENTE_NIVEL, 0.0f, WHITE);
    DrawTextEx(texto, TextFormat("%d/%d", items_recogidos, total_items), (Vector2){AREA_DIBUJADO_ICONOS[ICONO_ITEM].x + AJUSTE_X_TEXTO, alto_pantalla + AJUSTE_POSICION_Y}, TAM_FUENTE_ESTANDAR, 0.0f, WHITE);
    DrawTextEx(texto, TextFormat("%d/%d", monstruos_eliminados, cantidad_monstruos), (Vector2){AREA_DIBUJADO_ICONOS[ICONO_MONSTRUOS_ELIMINADOS].x + AJUSTE_X_TEXTO,  alto_pantalla + AJUSTE_POSICION_Y}, TAM_FUENTE_ESTANDAR, 0.0f, WHITE);
    DrawTextEx(texto, TextFormat("%d", p.total_muertes), (Vector2){AREA_DIBUJADO_ICONOS[ICONO_MUERTES].x + AJUSTE_X_TEXTO, alto_pantalla + AJUSTE_POSICION_Y}, TAM_FUENTE_ESTANDAR, 0.0f, WHITE);
    DrawTextEx(texto, TextFormat("%2d:%2d:%2d", horas, minutos, segundos), (Vector2){POSICION_X_RELOJ*ajuste_horizontal, alto_pantalla + AJUSTE_POSICION_Y}, TAM_FUENTE_ESTANDAR, 0.f, WHITE);
}
