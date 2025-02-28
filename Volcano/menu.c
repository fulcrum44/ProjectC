#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "menu.h"

const int CAMBIO_PANTALLA_BOTONES[]= {
    1,
    -1
};

Texture2D fondo;
Texture2D titulo;
Texture2D botones_menu;
Boton *botones;

extern int pantalla;

void inicializar_menu() {
    fondo=LoadTexture(TEXTURA_MENU);
    titulo=LoadTexture(TEXTURA_TITULO);
    botones_menu=LoadTexture(TEXTURA_BOTONES);

    botones=malloc(sizeof(Boton) * CANTIDAD_BOTONES_MENU);
    if (botones == NULL) {
        printf("\nERROR al reservar memoria para los botones del menu");
        exit(-1);
    }

    for (int i=0; i<CANTIDAD_BOTONES_MENU; i++) {
        botones[i].area=(Rectangle){0, i*ALTO_BOTON, ANCHO_BOTON, ALTO_BOTON};
        botones[i].tipo=CAMBIO_PANTALLA_BOTONES[i];
    }
}

void dibujar_menu() {
    // Calculamos el centro de la pantalla
    int x_centro_pantalla=GetScreenWidth()/2;
    int y_centro_pantalla=GetScreenHeight()/2;

    // Ajustamos las coordenadas para que la imagen de fondo se dibuje en el centro de la pantalla
    float fondo_x=x_centro_pantalla - (fondo.width * ESCALADO_IMAGEN)/2;
    float fondo_y=y_centro_pantalla - (fondo.height * ESCALADO_IMAGEN)/2;

    // Ajustamos coordenadas para centrar el título
    float titulo_x=x_centro_pantalla - (titulo.width)/2;

    // Ajustamos ahora las coordenadas para los botones
    for (int i=0; i<CANTIDAD_BOTONES_MENU; i++) {
        botones[i].posicion.x=x_centro_pantalla - (botones[i].area.width)/2;
        botones[i].posicion.y=GetScreenHeight() + (AJUSTE_Y_BOTONES + i * (ALTO_BOTON + ESPACIO_ENTRE_BOTONES));

        // Calculada la posicion que ocupará un botón en el menú, creamos su hitbox
        botones[i].hitbox=(Rectangle){botones[i].posicion.x, botones[i].posicion.y, ANCHO_BOTON, ALTO_BOTON};
    }

    // Dibujamos
    DrawTextureEx(fondo, (Vector2){fondo_x, fondo_y}, 0.0f, ESCALADO_IMAGEN, WHITE);
    DrawTexture(titulo, titulo_x, AJUSTE_Y_TITULO, WHITE);
    for (int i=0; i<CANTIDAD_BOTONES_MENU; i++) {
        DrawTextureRec(botones_menu, botones[i].area, botones[i].posicion, WHITE);
    }
}

bool boton_menu_pulsado(Vector2 posicion_raton) {
    for (int i=0; i<CANTIDAD_BOTONES_MENU; i++) {
        if (CheckCollisionPointRec(posicion_raton, botones[i].hitbox)) {
            pantalla=botones[i].tipo;
            printf("\nPantalla: %d", pantalla);
            return true;
        }
    }
    return false;
}
