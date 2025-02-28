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

void inicializar_raton() {
    SetMousePosition(200,200);
}

void actualizar_raton() {
    if (pantalla == PANTALLA_MENU) posicion_raton=GetMousePosition();
    else posicion_raton=GetScreenToWorld2D(GetMousePosition(), camara); // Con GetScreenToWorld2D ajustamos las coordenadas del ratón al mapa y en concordancia al ajuste de la cámara.

    //printf("\nRaton x: %f - y: %f", posicion_raton.x, posicion_raton.y);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        //Vector2 losa=conversion_coordenadas_losa(posicion_raton);

        // Al entrar en las funciones, con que una devuelva true ya no se comprobará más
        if (boton_menu_pulsado(posicion_raton)) return;
        if (cofre_pulsado(posicion_raton)) return;

    }
}
