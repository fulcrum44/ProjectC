#include "stdio.h"
#include "string.h"
#include "raylib.h"
#include "raymath.h"
#include "character.h"
#include "room.h"
#include "teclado.h"
#include "mobs.h"
#include "audio.h"

Texture2D sprite[CANTIDAD_TEXTURAS_PERSONAJE];
int orientacion;

const char *TEXTURAS_PERSONAJE[]= { // ANIMACIONES
    "resources\\character\\PNG\\Unarmed_Idle\\Unarmed_Idle_full.png", // P_PARADO
    "resources\\character\\PNG\\Unarmed_Run\\Unarmed_Run_full.png", // P_CORRIENDO
    "resources\\character\\PNG\\Sword_attack\\Sword_attack_full.png", // P_ATANCANDO
    "resources\\character\\PNG\\Unarmed_Death\\Unarmed_Death_full.png" // P_ELIMINADO
};

const Rectangle HITBOX_PERSONAJE[]= {
    (Rectangle){-8, 1, 17, 9}, // ATAQUE INFERIOR = 0
    (Rectangle){-22, -21, 17, 22}, // ATAQUE IZQUIERDO = 1
    (Rectangle){7, -21, 17, 22}, // ATAQUE DERECHO = 2
    (Rectangle){-8, -30, 17, 9}, // ATAQUE ARRIBA = 3
    (Rectangle){-6, -12, 13, 9} // TORSO = 4
};

extern float delta;
extern int *terreno;
extern int alto_sala;
extern int ancho_sala;
extern int alto_losa;
extern int ancho_losa;
extern char *datos_archivo;
extern int losa_x_reja;
extern int losa_y_reja;
extern int total_botones;
extern Monstruo *monstruos;
extern int cantidad_monstruos;
extern Sound *lista_sonidos;

void inicializa_textura_personaje() { // Ahora mismo va a parecer un poco innecesario pero luego le podemos dar uso de verdad si tenemos varios personajes.
    for (int i=0; i<CANTIDAD_TEXTURAS_PERSONAJE; i++) {
        sprite[i]=LoadTexture(TEXTURAS_PERSONAJE[i]);
    }
}

void crear_personaje(Personaje *p) {
    posicion_inicial_nivel(p); // En esta función gestionamos lo relativo a la posición del personaje como su propia posición como la de sus hitboxes. // ESTO DEBE VARIAR SEGÚN EL NIVEL EN EL QUE ENTREMOS
    p->direccion_desplazamiento=(Vector2){0,0};
    p->velocidad=110;
    p->textura_activa=P_PARADO;
    p->tiempo=0;
    p->fotograma_actual=0;
    p->fotogramas_ataque_restantes=0;
    p->fotograma=(Rectangle){0,0, ANCHO_FOTOGRAMA, ALTO_FOTOGRAMA};
    p->vida=VIDA_PERSONAJE;
    p->danyo=DANYO_ATAQUE_PERSONAJE;
    p->total_muertes=0;

    // Configuramos velocidades y volúmenes de reproduccion de los sonidos que sean propios del personaje.
    SetSoundVolume(lista_sonidos[PASOS], 0.25f);
    SetSoundPitch(lista_sonidos[PASOS], 0.65f);
    SetSoundPitch(lista_sonidos[ESPADA], 0.75f);

}

void actualizar_personaje(Personaje *p) {
    // Tanto si estamos apretando una tecla para movernos o pulsando el ratón para atacar, ya ha sido comprobado previamente a entrar en esta función

    // Comprobamos el estado del personaje y su dirección cargamos las texturas correspondientes
    if (p->estado == P_PARADO && p->textura_activa != P_PARADO) p->textura_activa=P_PARADO;
    else if (p->estado == P_CORRIENDO && p->textura_activa != P_CORRIENDO) p->textura_activa=P_CORRIENDO;
    else if (p->estado == P_ATACANDO && p->textura_activa != P_ATACANDO) p->textura_activa=P_ATACANDO;
    else if (p->estado == P_ELIMINADO && p->textura_activa != P_ELIMINADO) p->textura_activa=P_ELIMINADO;

    // Actualizamos si ha habido un intento de moverse
    if (p->estado == P_CORRIENDO) {
        p->fotograma.y=ALTO_FOTOGRAMA * orientacion; // Cambio de direccion en los fotogramas

        // Normalizamos
        p->direccion_desplazamiento=Vector2Normalize(p->direccion_desplazamiento);
        Vector2 destino=Vector2Add(p->posicion, Vector2Scale(p->direccion_desplazamiento, p->velocidad*delta));
        Vector2 destino_hitbox=Vector2Add(p->hb_posicion, Vector2Scale(p->direccion_desplazamiento, p->velocidad*delta));

        // Comprobamos si personaje se puede mover en la dirección pulsada. Usamos la hb_posicion para eso.
        if (!suelo_transitable(destino_hitbox, PERSONAJE)) {
            return;
        }

        if (!IsSoundPlaying(lista_sonidos[PASOS])) PlaySound(lista_sonidos[PASOS]);
        // Actualizamos datos dependientes de la posicion del personaje
        p->posicion=destino;
        p->hb_posicion=destino_hitbox;

        for (int i=0; i<CANTIDAD_HITBOXES; i++) {
            p->hitboxes[i].x=p->posicion.x + HITBOX_PERSONAJE[i].x;
            p->hitboxes[i].y=p->posicion.y + HITBOX_PERSONAJE[i].y;
        }

        p->losa=conversion_coordenadas_losa(p->posicion);

        // Importante reiniciar estas variables para el proximo frame. Reiniciamos después de haber validado el movimiento y no antes porque la entrada por teclado se gestiona antes de entrar en actualizar_personaje()
        // Si llegados a un frame hemos dejado de movernos no nos interesa acumular cálculos de ejecuciones anteriores. Si seguimos corriendo tampoco queremos que el desplazamiento crezca exponencialmente.
        p->estado=P_PARADO;
        p->direccion_desplazamiento.x=0;
        p->direccion_desplazamiento.y=0;
    }

    // Animacion
    actualizar_fotogramas_personaje(p);
}

void actualizar_fotogramas_personaje(Personaje *p) {
    p->tiempo+=delta;

    // Animación ataque
    if(p->estado == P_ATACANDO) {
        if (p->tiempo >= TIEMPO_FOTOGRAMA_ATAQUE) {
                p->fotograma_actual++;
                p->fotogramas_ataque_restantes--;
                p->fotograma.x=p->fotograma_actual * ANCHO_FOTOGRAMA; // Avanzamos al siguiente fotograma en la textura
                p->tiempo=0; // Reiniciamos la variable

                if (p->fotogramas_ataque_restantes <= 0) {
                    p->estado=P_PARADO;
                }
        }
        return;
    }

    // Animacion muerte
    if (p->estado == P_ELIMINADO) {
        if (p->tiempo >= TIEMPO_FOTOGRAMA) { // SI USO TIEMPO_FOTOGRAMA_MUERTE POR ALGUNA RAZÓN LA ANIMACIÓN ENTRA EN UN BUCLE INFINITO
            // Si en el frame anterior ya se ha llegado al último fotograma de la textura no avanzamos más y reiniciamos el personaje
            if (p->fotograma_actual == FOTOGRAMAS_MUERTE) { // Cuando el personaje muerte solo queremos que la animación de su muerte se reproduzca un único ciclo completo antes de reaparecer.
                reinicio_personaje(p);
                return;
            }

            p->fotograma_actual++;
            p->fotograma.x=p->fotograma_actual * ANCHO_FOTOGRAMA; // Avanzamos al siguiente fotograma en la textura
            p->tiempo=0; // Reiniciamos la variable
        }
    }

    // Animacion general
    if (p->tiempo >= TIEMPO_FOTOGRAMA) {
        p->fotograma_actual++;
        p->fotograma_actual%=FOTOGRAMAS; // Ajustamos el fotograma actual al rango de fotogramas por fila en textura.
        p->tiempo=0; // Reiniciamos la variable
        p->fotograma.x=p->fotograma_actual * ANCHO_FOTOGRAMA; // Avanzamos al siguiente fotograma en la textura
    }
}

void dibujar_personaje(Personaje *p) {
    DrawTexturePro(sprite[p->textura_activa], p->fotograma, (Rectangle){p->posicion.x, p->posicion.y, ANCHO_FOTOGRAMA, ALTO_FOTOGRAMA}, (Vector2){31, 43}, 0.0f, WHITE);
}

bool suelo_transitable(Vector2 destino, int sujeto) {
    // Comprobamos que las esquinas del hitbox del personaje no estén colisionando con suelo no transitable
    if (!hb_esquina((Vector2){destino.x, destino.y}, sujeto)) return false;
    if (!hb_esquina((Vector2){destino.x+HB_LONG_HORIZONTAL, destino.y}, sujeto)) return false;
    if (!hb_esquina((Vector2){destino.x+HB_LONG_HORIZONTAL, destino.y+HB_LONG_VERTICAL}, sujeto)) return false;
    if (!hb_esquina((Vector2){destino.x+HB_LONG_HORIZONTAL, destino.y+HB_LONG_VERTICAL}, sujeto)) return false;

    return true; // Devolvemos true si el suelo donde pretendemos movernos es transitable
}

bool hb_esquina(Vector2 esquina, int sujeto) {
    int losa_esquina;

    Vector2 losa=conversion_coordenadas_losa(esquina);

    losa_esquina=*(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + ((int)losa.y * ancho_sala) + (int)losa.x);

    if (losa_esquina == SUELO_NO_TRANSITABLE) return false;

    if (sujeto == MONSTRUO) return true; // El resto de la función corresponde a mecánica del personaje.


    // Comprobamos a partir de aqui si el personaje ha pisado algun elemento interactivo del mapa.
    if (losa_esquina == PUERTA_SIGUIENTE_NIVEL) siguiente_nivel();

    if (losa_esquina == BOTON) {
        PlaySound(lista_sonidos[BOTON_PISADO]);
        *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + ((int)losa.y * ancho_sala) + (int)losa.x)=BOTON_PULSADO;
        total_botones--;

        if (total_botones <= 0) { // Todos los botones tienen que haber sido pulsados para que la reja se abra
            *(terreno + (CAPA_COLISION * ancho_sala * alto_sala) + (losa_y_reja * ancho_sala) + losa_x_reja)=0;
            *(terreno + (CAPA_SUELO * ancho_sala * alto_sala) + (losa_y_reja * ancho_sala) + losa_x_reja)=REJA_ABIERTA;
        }
    }

    return true;
}

Vector2 conversion_coordenadas_losa(Vector2 coordenada) {
    Vector2 losa;

    losa.x=floor(coordenada.x/ancho_losa);
    losa.y=floor(coordenada.y/alto_losa);

    return losa;
}

void posicion_inicial_nivel(Personaje *p) {
    char *cursor;

    // Buscamos la información del punto de aparición del personaje en el archivo del mapa del nivel actual para determinar su posición inicial
    cursor=strstr(datos_archivo, ETIQ_NOMBRE_PERSONAJE)+strlen(ETIQ_NOMBRE_PERSONAJE);
    sscanf(strstr(cursor, ETIQ_X_OBJETO)+strlen(ETIQ_X_OBJETO), "%f", &p->posicion.x);
    sscanf(strstr(cursor, ETIQ_Y_OBJETO)+strlen(ETIQ_Y_OBJETO), "%f", &p->posicion.y);

    // Colocamos el hitbox de movimiento en posicion
    p->hb_posicion.x=p->posicion.x+HB_X_ORIGEN;
    p->hb_posicion.y=p->posicion.y+HB_Y_ORIGEN;

    // Colocamos el resto de hitboxes del personaje
    for (int i=0; i<CANTIDAD_HITBOXES; i++) {
        p->hitboxes[i].x=p->posicion.x + HITBOX_PERSONAJE[i].x;
        p->hitboxes[i].y=p->posicion.y + HITBOX_PERSONAJE[i].y;
        p->hitboxes[i].width=HITBOX_PERSONAJE[i].width;
        p->hitboxes[i].height=HITBOX_PERSONAJE[i].height;
    }
}

void movimiento_personaje(Personaje *p, Vector2 desplazamiento, int orientacion_final) {
    // Por poder podemos estar apretando una tecla de movimiento mientras estamos atacando, pero no volverá a moverse hasta el movimiento del ataque termine.
    // Por otra parte, no tiene sentido comprobar intentos de moverse si el personaje ha sido recientemente eliminado.
    if (p->estado == P_ATACANDO || p->estado == P_ELIMINADO) return;

    // Primero vamos a proceder como si el destino al que queremos movernos fuese transitable y normalizamos el desplazamiento para evitar imprecisiones.
    desplazamiento=Vector2Normalize(desplazamiento);
    Vector2 destino=Vector2Add(p->hb_posicion, Vector2Scale(desplazamiento, p->velocidad*delta));

    // Comprobamos si de verdad nos podemos mover en la direccion pretendida. Comprobamos en los dos ejes.
    bool transitable_x=suelo_transitable((Vector2){destino.x, p->hb_posicion.y}, PERSONAJE);
    bool transitable_y=suelo_transitable((Vector2){p->hb_posicion.x, destino.y}, PERSONAJE);

    // Creamos un Vector2 que podamos manipular localmente
    Vector2 desplazamiento_definitivo=(Vector2){desplazamiento.x, desplazamiento.y};

    // Anulamos desplazamiento en el eje donde por el que no podamos transitar si fuera el caso.
    if (!transitable_x) desplazamiento_definitivo.x=0;
    if (!transitable_y) desplazamiento_definitivo.y=0;

    // Llegados aquí ya tenemos el desplazamiento correcto.

    p->estado=P_CORRIENDO;
    p->direccion_desplazamiento=desplazamiento_definitivo;
    orientacion=orientacion_final;
}

void ataque_personaje(Personaje *p) {
    // Reproducimos el sonido de la espada independientemente si su hitbox está colisionando con un enemigo
    PlaySound(lista_sonidos[ESPADA]);

    if (p->estado != P_ATACANDO) { // Si ocurriese que hemos pulsado al ratón antes de que haya acabado una animación de ataque no hacemos nada.
        p->estado=P_ATACANDO;
        p->fotograma_actual=0; // Nos aseguramos que empezaremos por el primer fotograma de la animación
        p->fotograma.x=0; // Con fotograma_actual=0 no sería necesario reiniciar esta variable. Lo hacemos igualmente para evitar inconsistencias.
        p->fotogramas_ataque_restantes=FOTOGRAMAS; // Esto es lo que durará la animación de ataque en fotogramas.

        for (int i=0; i<cantidad_monstruos; i++) {
            if (!monstruos[i].activo) continue; // No comprobamos colisiones de ataque con monstruos inactivos

            if (CheckCollisionRecs(p->hitboxes[orientacion], monstruos[i].hitbox_combate)) {
                monstruos[i].vida-=p->danyo;

                if (monstruos[i].tipo.id == CENTINELA) PlaySound(lista_sonidos[ARMADURA_ENEMIGO]);

                if (monstruos[i].tipo.id == SETA_MAGMA) {
                    PlaySound(lista_sonidos[ENEMIGO_ATACADO]);
                    if (orientacion == ORIENTACION_ABAJO) {
                        monstruos[i].posicion.y+=20;
                    }
                    if (orientacion == ORIENTACION_ARRIBA) {
                        monstruos[i].posicion.y-=20;
                    }
                    if (orientacion == ORIENTACION_DER) {
                        monstruos[i].posicion.x+=20;
                    }
                    if (orientacion == ORIENTACION_IZQ) {
                        monstruos[i].posicion.x-=20;
                    }

                }

                muerte_monstruo(&monstruos[i]);

                printf("\nVida monstruo %d: %d", i, monstruos[i].vida);
                //printf("\n\nPersonaje ataca a monstruo\n\n");
            }
        }
    }
}

void muerte_personaje(Personaje *p) {
    if (p->estado != P_ELIMINADO) { // Procuramos que se ejecute solamente una vez por muerte.
        if (p->vida <= 0) {
            p->estado=P_ELIMINADO;
            p->fotograma_actual=0;
            p->total_muertes++;
        }
    }
}

void reinicio_personaje(Personaje *p) {
    p->estado=P_PARADO;
    p->vida=VIDA_PERSONAJE; // Restablecemos vida.

    posicion_inicial_nivel(p); // Reinciamos el posicionamiento del personaje y recolocamos sus hitboxes.
}
