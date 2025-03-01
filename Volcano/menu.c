#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "menu.h"
#include "room.h"
#include "character.h"
#include "mobs.h"
#include "cofres.h"

const int BOTONES_MENU_PRINCIPAL[]= {
    EMPEZAR_PARTIDA, // PANTALLA JUEGO
    SALIR // SALIDA
};

const int BOTONES_MENU_PAUSA[]= {
    CONTINUAR, // MODIFICARÁ LA VARIABLE PAUSA -> 0 = FALSE
    MENU_PRINCIPAL, // MODIFICARÁ LA VARIABLE PANTALLA -> 0 = MENU PRINCIPAL
    SALIR // SALIDA
};

int x_centro_pantalla;
int y_centro_pantalla;

Texture2D fondo;
Texture2D titulo;
Texture2D botones_menu;
Texture2D menu_pausa;
Texture2D botones_menu_pausa;
Boton *botones;
Boton *botones_pausa;

extern int pantalla;
extern bool pausa;
extern Personaje personaje;
extern Camera2D camara;

void inicializar_menu() {
    // Calculamos el centro de la pantalla. Lo necesitaremos tanto para el menu principal como el menu de pausa
    x_centro_pantalla=GetScreenWidth()/2;
    y_centro_pantalla=GetScreenHeight()/2;

    // Inicializamos menu principal del jugo
    fondo=LoadTexture(TEXTURA_MENU);
    titulo=LoadTexture(TEXTURA_TITULO);
    botones_menu=LoadTexture(TEXTURA_BOTONES);

    botones=malloc(sizeof(Boton) * CANTIDAD_BOTONES_MENU_PRINCIPAL);
    if (botones == NULL) {
        printf("\nERROR al reservar memoria para los botones del menu");
        exit(-1);
    }

    for (int i=0; i<CANTIDAD_BOTONES_MENU_PRINCIPAL; i++) {
        botones[i].area=(Rectangle){0, i*ALTO_BOTON, ANCHO_BOTON, ALTO_BOTON};
        botones[i].tipo=BOTONES_MENU_PRINCIPAL[i];
    }
}

void inicializar_menu_pausa() {
    // Inicializamos menu de pausa
    menu_pausa=LoadTexture(TEXTURA_MENU_PAUSA);
    botones_menu_pausa=LoadTexture(TEXTURA_BOTONES_MENU_PAUSA);

    botones_pausa=malloc(sizeof(Boton) * CANTIDAD_BOTONES_MENU_PAUSA);
    if (botones_pausa == NULL) {
        printf("\nERROR al reservar memoria para los botones del menu de pausa");
        exit(-1);
    }

    for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
        botones_pausa[i].area=(Rectangle){0, i*ALTO_BOTON_PAUSA, ANCHO_BOTON_PAUSA, ALTO_BOTON_PAUSA};
        botones_pausa[i].tipo=BOTONES_MENU_PAUSA[i];
    }
}

void dibujar_menu_principal() {
    // Ajustamos las coordenadas para que la imagen de fondo se dibuje en el centro de la pantalla
    float fondo_x=x_centro_pantalla - (fondo.width * ESCALADO_IMAGEN)/2;
    float fondo_y=y_centro_pantalla - (fondo.height * ESCALADO_IMAGEN)/2;

    // Ajustamos coordenadas para centrar el título
    float titulo_x=x_centro_pantalla - (titulo.width)/2;

    // Ajustamos ahora las coordenadas para los botones
    for (int i=0; i<CANTIDAD_BOTONES_MENU_PRINCIPAL; i++) {
        botones[i].posicion.x=x_centro_pantalla - (botones[i].area.width)/2;
        botones[i].posicion.y=GetScreenHeight() + (AJUSTE_Y_BOTONES + i * (ALTO_BOTON + ESPACIO_ENTRE_BOTONES));

        // Calculada la posicion que ocupará un botón en el menú, creamos su hitbox
        botones[i].hitbox=(Rectangle){botones[i].posicion.x, botones[i].posicion.y, ANCHO_BOTON, ALTO_BOTON};
    }

    // Dibujamos
    DrawTextureEx(fondo, (Vector2){fondo_x, fondo_y}, 0.0f, ESCALADO_IMAGEN, WHITE);
    DrawTexture(titulo, titulo_x, AJUSTE_Y_TITULO, WHITE);
    for (int i=0; i<CANTIDAD_BOTONES_MENU_PRINCIPAL; i++) {
        DrawTextureRec(botones_menu, botones[i].area, botones[i].posicion, WHITE);
    }
}

bool boton_menu_principal_pulsado(Vector2 posicion_raton) {
    for (int i=0; i<CANTIDAD_BOTONES_MENU_PRINCIPAL; i++) {
        if (CheckCollisionPointRec(posicion_raton, botones[i].hitbox)) {
            pantalla=botones[i].tipo;
            if (botones[i].tipo=EMPEZAR_PARTIDA) iniciar_partida();

            return true;
        }
    }
    return false;
}

void dibujar_menu_pausa() {
    EndMode2D(); // Nos salimos momentaneamente del Mode2D para dibujar el menu de pausa teniendo de referencia las coordenadas de la pantalla.

    // Ajustamos el fondo del menu de pausa al centro de la pantalla.
    float menu_pausa_x=x_centro_pantalla - (menu_pausa.width * ESCALADO_MENU_PAUSA)/2;
    float menu_pausa_y=y_centro_pantalla - (menu_pausa.height * ESCALADO_MENU_PAUSA)/2;

    // Ajustamos posicion de los botones
    for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
        botones_pausa[i].posicion.x=x_centro_pantalla -(botones_menu_pausa.width/2);
        botones_pausa[i].posicion.y=y_centro_pantalla - (menu_pausa.height * ESCALADO_MENU_PAUSA / 2) + AJUSTE_Y_BOTON_PAUSA + i * (ALTO_BOTON_PAUSA + ESPACIO_ENTRE_BOTONES_PAUSA);

        // Establecemos el hitox de los botones sabida ya su posicion
        botones_pausa[i].hitbox=(Rectangle){botones_pausa[i].posicion.x, botones_pausa[i].posicion.y, ANCHO_BOTON_PAUSA, ALTO_BOTON_PAUSA};
    }

    // Dibujamos
    DrawTextureEx(menu_pausa, (Vector2){menu_pausa_x, menu_pausa_y}, 0.0f, ESCALADO_MENU_PAUSA, WHITE);
    for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
        DrawTextureRec(botones_menu_pausa, botones_pausa[i].area, botones_pausa[i].posicion, WHITE);
    }
}

bool boton_menu_pausa_pulsado(Vector2 posicion_raton) {
    for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
        if (CheckCollisionPointRec(posicion_raton, botones_pausa[i].hitbox)) {
            switch(i) {
                case 0: // CONTINUAR
                    pausa=botones_pausa[i].tipo;
                    break;

                case 1: // MENU PRINCIPAL
                    pantalla=botones_pausa[i].tipo; // Pantalla = 0
                    pausa=botones_pausa[i].tipo; // Pausa = 0 = false
                    liberar_menu_pausa();
                    finaliza_nivel();
                    break;

                case 2: // SALIR
                    pantalla=botones_pausa[i].tipo; // Pantalla = -1
                    finaliza_nivel();
                    break;
            }

            return true;
        }
    }

    return false;
}

void liberar_menu() {
    free(botones);
}

void liberar_menu_pausa() {
    free(botones_pausa);
}

void iniciar_partida() {
    inicializa_nivel(NIVEL_INICIAL);
    // Personaje
    crear_personaje(&personaje);
    inicializa_textura_personaje();

    // Items
    inicializa_textura_items();

    // Monstruos
    inicializa_monstruos();

    // Menu pausa
    inicializar_menu_pausa();
}
