#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "menu.h"
#include "room.h"
#include "character.h"
#include "mobs.h"
#include "cofres.h"
#include "crono.h"
#include "audio.h"

const int BOTONES_MENU_PRINCIPAL[]= {
    EMPEZAR_PARTIDA, // PANTALLA JUEGO
    SALIR // SALIDA
};

const int BOTONES_MENU_PAUSA[]= {
    CONTINUAR, // MODIFICARÁ LA VARIABLE PAUSA -> 0 = FALSE
    MENU_PRINCIPAL, // MODIFICARÁ LA VARIABLE PANTALLA -> 0 = MENU PRINCIPAL
    AJUSTES,
    SALIR // SALIDA
};

const int BOTONES_AJUSTES[]= {
    ON, // ACTIVA EL VOLUMEN DE LA MUSICA
    OFF, // SILENCIA EL VOLUMEN DE TODO EN EL JUEGO
    VOLVER // VUELVE A LA PANTALLA PRINCIPAL DEL MENU PAUSA
};

int x_centro_pantalla;
int y_centro_pantalla;
int pantalla_menu_pausa;

Texture2D fondo;
Texture2D titulo;
Texture2D botones_menu;
Texture2D menu_pausa;
Texture2D botones_menu_pausa;
Texture2D ajustes;

Boton *botones;
Boton *botones_pausa;
Boton *botones_ajustes;

extern int pantalla;
extern bool pausa;
extern Personaje personaje;
extern Camera2D camara;
extern Font texto;

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
    ajustes=LoadTexture(TEXTURA_BOTONES_AJUSTES);

    // Preparamos los botones de la pantalla principal del menu de pausa
    botones_pausa=malloc(sizeof(Boton) * CANTIDAD_BOTONES_MENU_PAUSA);
    if (botones_pausa == NULL) {
        printf("\nERROR al reservar memoria para los botones del menu de pausa");
        exit(-1);
    }

    for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
        botones_pausa[i].area=(Rectangle){0, i*ALTO_BOTON_PAUSA, ANCHO_BOTON_PAUSA, ALTO_BOTON_PAUSA};
        botones_pausa[i].tipo=BOTONES_MENU_PAUSA[i];
    }

    // Preparamos los botones del apartado ajustes del menu pausa
    botones_ajustes=malloc(sizeof(Boton) * CANTIDAD_BOTONES_AJUSTES);
    if (botones_ajustes == NULL) {
        printf("\nERROR al reservar memoria para los botones de los ajustes");
        exit(-1);
    }

    for (int i=0; i<CANTIDAD_BOTONES_AJUSTES; i++) {
        botones_ajustes[i].area=(Rectangle){0, i*ALTO_BOTON_AJUSTE, ANCHO_BOTON_AJUSTE, ALTO_BOTON_AJUSTE};
        botones_ajustes[i].tipo=BOTONES_AJUSTES[i];
    }

    pantalla_menu_pausa=PRINCIPAL;
}

void dibujar_menu_principal() {
    // He configurado las dimensiones y coordenadas de los elementos del menú usando una resolucion de pantalla de 1440x1080 como referencia.
    // El tamaño de la pantalla se ajusta al tamaño del monitor desde el que se ejecuta. Si la resolución máxima de un equipo es menor que la de referencia, los elementos del menú empiezan a comprimirse y solaparse.
    // Cuando las dimensiones del monitor sean menores que la resolución de referencia, escalaremos los elementos del menú para que su tamaño se adapte al espacio disponible de pantalla.
    // toDO

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
            if (botones[i].tipo==EMPEZAR_PARTIDA) {
                iniciar_partida();
                iniciar_musica(0.25f);
            }

            return true; // Devolvemos true para que en mouse.c no se compruebe más a qué se ha pulsado con el ratón
        }
    }
    return false; // Devolvemos false para que se siga comprobando en mouse.c que se ha pulsado con el ratón
}

void dibujar_menu_pausa() {
    EndMode2D(); // Nos salimos momentaneamente del Mode2D para dibujar el menu de pausa teniendo de referencia las coordenadas de la pantalla.

    // Ajustamos el fondo del menu de pausa al centro de la pantalla.
    float menu_pausa_x=x_centro_pantalla - (menu_pausa.width * ESCALADO_MENU_PAUSA)/2;
    float menu_pausa_y=y_centro_pantalla - (menu_pausa.height * ESCALADO_MENU_PAUSA)/2;

    // Alineamos texto que dibujemos. Ahora solo hay un texto y está hecho pensando solo en ese texto
    float posicion_x_texto=menu_pausa_x+AJUSTE_X_TEXTO_AJUSTES;
    float posicion_y_texto=menu_pausa_y+AJUSTE_Y_TEXTO_AJUSTES;

    // Ajustamos posicion de los botones de la pantalla principal
    for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
        botones_pausa[i].posicion.x=x_centro_pantalla -(botones_menu_pausa.width/2);
        botones_pausa[i].posicion.y=y_centro_pantalla - (menu_pausa.height * ESCALADO_MENU_PAUSA / 2) + AJUSTE_Y_BOTON_PAUSA + i * (ALTO_BOTON_PAUSA + ESPACIO_ENTRE_BOTONES_PAUSA);

        // Establecemos el hitbox de los botones sabida ya su posicion
        botones_pausa[i].hitbox=(Rectangle){botones_pausa[i].posicion.x, botones_pausa[i].posicion.y, ANCHO_BOTON_PAUSA, ALTO_BOTON_PAUSA};
    }

    // Ajustamos posicion de los botones dentro de los ajustes
    for (int i=0; i<CANTIDAD_BOTONES_AJUSTES; i++) {
        // Está bien pero no me convence. Podría guardar las posiciones en un array de Vector2 en esta misma funcion.
        if (i == OFF) {
            botones_ajustes[i].posicion.x=x_centro_pantalla - (botones_menu_pausa.width) + AJUSTE_X_BOTON_AJUSTES + ESPACIO_ENTRE_BOTONES_AJUSTES_X;
            botones_ajustes[i].posicion.y=botones_ajustes[ON].posicion.y;
        }
        else {
            botones_ajustes[i].posicion.x=x_centro_pantalla - (botones_menu_pausa.width) + AJUSTE_X_BOTON_AJUSTES;
            botones_ajustes[i].posicion.y=y_centro_pantalla - (menu_pausa.height/2 * ESCALADO_MENU_PAUSA) + AJUSTE_Y_BOTON_AJUSTES + i * (ALTO_BOTON_AJUSTE + ESPACIO_ENTRE_BOTONES_AJUSTES_Y);
        }

        // Establecemos el hitbox de los botones de los ajustes
        botones_ajustes[i].hitbox=(Rectangle){botones_ajustes[i].posicion.x, botones_ajustes[i].posicion.y, ANCHO_BOTON_AJUSTE, ALTO_BOTON_AJUSTE};
    }

    // Dibujamos
    if (pantalla_menu_pausa == PRINCIPAL) {
        DrawTextureEx(menu_pausa, (Vector2){menu_pausa_x, menu_pausa_y}, 0.0f, ESCALADO_MENU_PAUSA, WHITE);
        for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
            DrawTextureRec(botones_menu_pausa, botones_pausa[i].area, botones_pausa[i].posicion, WHITE);
        }
    } else if (pantalla_menu_pausa == PANTALLA_AJUSTES) {
        DrawTextureEx(menu_pausa, (Vector2){menu_pausa_x, menu_pausa_y}, 0.0f, ESCALADO_MENU_PAUSA, WHITE);
        DrawTextEx(texto, "Musica", (Vector2){posicion_x_texto, posicion_y_texto} , TAMANIO_FUENTE_AJUSTES, 0, BLACK);
        for (int i=0; i<CANTIDAD_BOTONES_AJUSTES; i++) {
            DrawTextureRec(ajustes, botones_ajustes[i].area, botones_ajustes[i].posicion, WHITE);
        }
    }
}

void dibujar_menu_fin_partida() {
    // ESTO ES AUN UNA VERSION DE PRUEBA. LA IDEA ES HACER UN RECOPILATORIO DE CIERTAS ESTADÍSTICAS ADEMÁS DE MOSTRAR LOS BOTONES YA IMPLEMENTADOS.


    // Ajustamos el fondo del menu de pausa al centro de la pantalla.
    float menu_pausa_x=x_centro_pantalla - (menu_pausa.width * ESCALADO_MENU_PAUSA)/2;
    float menu_pausa_y=y_centro_pantalla - (menu_pausa.height * ESCALADO_MENU_PAUSA)/2;

    // Alineamos texto que dibujemos. Ahora solo hay un texto y está hecho pensando solo en ese texto
    float posicion_x_texto=menu_pausa_x+AJUSTE_X_TEXTO_AJUSTES;
    float posicion_y_texto=menu_pausa_y+AJUSTE_Y_TEXTO_AJUSTES;

    DrawTextureEx(menu_pausa, (Vector2){menu_pausa_x, menu_pausa_y}, 0.0f, ESCALADO_MENU_PAUSA, WHITE);
    DrawTextEx(texto, "FIN PARTIDA", (Vector2){posicion_x_texto+25, posicion_y_texto} , 80, 0, BLACK);
    DrawTextureRec(botones_menu_pausa, botones_pausa[1].area, botones_pausa[1].posicion, WHITE);
    DrawTextureRec(botones_menu_pausa, botones_pausa[3].area, botones_pausa[3].posicion, WHITE);
}

bool boton_menu_pausa_pulsado(Vector2 posicion_raton) {
    if (pantalla_menu_pausa != PRINCIPAL) return false;

    for (int i=0; i<CANTIDAD_BOTONES_MENU_PAUSA; i++) {
        if (CheckCollisionPointRec(posicion_raton, botones_pausa[i].hitbox)) {
            switch(i) {
                case 0: // CONTINUAR
                    pausa=botones_pausa[i].tipo;
                    break;

                case 1: // MENU PRINCIPAL
                    pantalla=botones_pausa[i].tipo; // Pantalla = 0
                    pausa=botones_pausa[i].tipo; // Pausa = 0 = false
                    detener_musica();
                    liberar_menu_pausa();
                    finaliza_nivel();
                    break;

                case 2: // AJUSTES
                    pantalla_menu_pausa=PANTALLA_AJUSTES; // Con el cambio de pantalla del menu pausa deben aparecer otros botones
                    break;

                case 3: // SALIR
                    pantalla=botones_pausa[i].tipo; // Pantalla = -1
                    finaliza_nivel();
                    break;
            }

            return true; // Un botón ha sido pulsado
        }
    }

    return false; // No estamos pulsando ningún botón
}

bool boton_ajustes_pulsado(Vector2 posicion_raton) {
    if (pantalla_menu_pausa != PANTALLA_AJUSTES) return false;

    for (int i=0; i<CANTIDAD_BOTONES_AJUSTES; i++) {
        if (CheckCollisionPointRec(posicion_raton, botones_ajustes[i].hitbox)) {
            switch(i) {
                case ON: // ACTIVAR VOLUMEN
                    des_silenciar_musica();
                    break;

                case OFF: // SILENCIAR
                    silenciar_musica();
                    break;

                case VOLVER: // VOLVER ATRÁS
                    pantalla_menu_pausa=PRINCIPAL;
                    break;
            }

            return true; // Un botón ha sido pulsado
        }
    }

    return false; // No estamos pulsando ningún botón
}

void liberar_menu() {
    free(botones);
}

void liberar_menu_pausa() {
    free(botones_pausa);
}

void iniciar_partida() {
    // Inicializamos lo necesario para empezar la partida. Lo hacemos ordenadamente.

    inicializa_nivel(2);
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
