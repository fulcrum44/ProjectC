#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "raylib.h"
#include "audio.h"
#include "menu.h"

const char *PLAYLIST[]= {
    "resources\\audio\\lava.wav",
    "resources\\audio\\03. Volcano Mines (Molten Jelly).mp3",
    "resources\\audio\\04. Volcano Mines (Forgotten World).mp3",
    "resources\\audio\\05. Mystery of the Caldera.mp3",
};

const char *SONIDOS[]= {
    "resources\\audio\\sounds\\thudStep.wav",
    "resources\\audio\\sounds\\openChest.wav",
    "resources\\audio\\sounds\\openBox.wav",
    "resources\\audio\\sounds\\ow.wav",
    "resources\\audio\\sounds\\parry.wav",
    "resources\\audio\\sounds\\hitEnemy.wav",
    "resources\\audio\\sounds\\brush.wav",
    NULL // Si ponemos NULL al final de un array constante, podemos iterar por él hasta encontrar un NULL como señal para terminar esa iteración. Viene bien, por ejemplo, para recorrer las rutas y cargar los archivos respectivos sin tener conocimiento de la cantidad que hay.
};

int musica_actual=-1;
int reproducciones=0;

Sound *lista_sonidos;
int cantidad_sonidos=0; // Inicializamos a 0 el contador de sonidos
float volumen_musica_ultimo; // Guardamos el volumen actual de la musica como ultimo volumen configurado para poder usarlo cuando dejemos de silenciar la musica

extern Music musica;
extern int pantalla;
extern int nivel_actual;
extern int cantidad_niveles;

// MUSICA

void iniciar_musica(float volumen) {
    // Comprobamos si se estaba reproduciendo previamente otra cosa
    if (IsMusicStreamPlaying(musica)) {
        StopMusicStream(musica);
        //UnloadMusicStream(musica); // Esta función es un poco peculiar. Usarla resulta más contraproducente que no usarla.
    }

    if (nivel_actual != cantidad_niveles) musica_actual=musica_aleatoria();

    volumen_musica_ultimo=volumen;

    musica=LoadMusicStream(PLAYLIST[musica_actual]); // El Load pone por defecto como true el loopind del Music cuando se usan ciertos formatos a de audio. Decidimos manualmente después cuando será true y cuando false
    SetMusicVolume(musica, volumen);
    if (musica_actual == AMBIENTE_LAVA) musica.looping=true;
    else musica.looping=false;
    PlayMusicStream(musica);
}

void actualizar_musica() {
    UpdateMusicStream(musica);

    float tiempo_maximo=rand()%(MAX_REPRODUCCION_LAVA-MIN_REPRODUCCION_LAVA)+MIN_REPRODUCCION_LAVA;
    if (musica_actual == AMBIENTE_LAVA) {
        if (GetMusicTimePlayed(musica) >= GetMusicTimeLength(musica)-0.02f) {
            reproducciones++;
        }
        if (reproducciones > (tiempo_maximo/GetMusicTimeLength(musica))) {
            iniciar_musica(0.25f);
            reproducciones=0;
        }
    }

    if (pantalla == PANTALLA_JUEGO && !IsMusicStreamPlaying(musica)) iniciar_musica(0.25f);

}

void detener_musica() {
    StopMusicStream(musica);
    //UnloadMusicStream(musica); // Esta función es un poco peculiar. Usarla resulta más contraproducente que no usarla.
}

void des_silenciar_musica() {
    SetMusicVolume(musica, volumen_musica_ultimo);
}

void silenciar_musica() {
    SetMusicVolume(musica, 0);
}

int musica_aleatoria() {
    if (musica_actual == AMBIENTE_LAVA) return rand()%2+1; // Si la musica anterior era el sonido ambiental, evitamos que el proximo sea este mismo

    return rand()%3;
}


// SONIDO

void cargar_sonidos() {
    while (SONIDOS[cantidad_sonidos] != NULL) {
        // Comprobamos si hay alguna ruta de sonido almacenada o si hay más.

        // Vamos asignando memoria a nuestro array de lista_sonidos según vayamos leyendo sonidos que almacenar.
        if (lista_sonidos == NULL) {
            lista_sonidos=malloc(sizeof(Sound));
            if (lista_sonidos == NULL) {
                printf("\nERROR al asignar memoria para sonidos");
                exit(-1);
            }
        } else {
            lista_sonidos=realloc(lista_sonidos, sizeof(Sound) * (cantidad_sonidos +1));
            if (lista_sonidos == NULL) {
                printf("\nERROR al asignar memoria para sonidos");
                exit(-1);
            }
        }

        // Almacenamos el próximo sonido de la lista
        lista_sonidos[cantidad_sonidos]=LoadSound(SONIDOS[cantidad_sonidos]);

        // Aumentamos cantidad de sonidos almacenados
        cantidad_sonidos++;
    }
}

void liberar_sonidos() { // Se llama en el main
    for (int i=0; i<cantidad_sonidos; i++) {
        UnloadSound(lista_sonidos[i]); // Descargamos los sonidos cargados al principio
    }

    free(lista_sonidos); // Liberamos la memoria del puntero.
}


