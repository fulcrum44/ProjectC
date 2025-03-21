#include "stdio.h"
#include "raylib.h"
#include "math.h"
#include "mouse.h"
#include "room.h"
#include "character.h"
#include "cofres.h"
#include "menu.h"

Vector2 posicion_raton;

extern Camera2D camara;
extern int pantalla;
extern bool pausa;
extern Personaje personaje;

void inicializar_raton() {
    Vector2 centro_pantalla;

    centro_pantalla.x=GetScreenWidth()/2;
    centro_pantalla.y=GetScreenHeight()/2;

    SetMousePosition(centro_pantalla.x,centro_pantalla.y);
}

void actualizar_raton() {
    if (pantalla == PANTALLA_MENU || pausa) posicion_raton=GetMousePosition();
    else posicion_raton=GetScreenToWorld2D(GetMousePosition(), camara); // Con GetScreenToWorld2D ajustamos las coordenadas del ratón al mapa y en concordancia al ajuste de la cámara.

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Al entrar en las funciones, con que una devuelva true ya no se comprobará más
        if (pantalla == PANTALLA_MENU && boton_menu_principal_pulsado(posicion_raton)) return;
        if (pantalla == PANTALLA_JUEGO) {
            if (boton_menu_pausa_pulsado(posicion_raton)) return;
            if (boton_ajustes_pulsado(posicion_raton)) return;
            if (!pausa) ataque_personaje(&personaje);
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        if (cofre_pulsado(posicion_raton)) return;
    }
}
