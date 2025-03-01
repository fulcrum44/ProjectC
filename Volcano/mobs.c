#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "raylib.h"
#include "raymath.h"
// #include "mobsTypesData.h" Al compilar no parece pedir incluirse.
#include "mobs.h"
#include "character.h"


Monstruo *monstruos;
Tipo tipos[TIPOS_MONSTRUO];
int cantidad_monstruos;
Texture2D sprite_monstruos;

extern int nivel_actual;
extern char* datos_archivo; // En room.c nunca descargamos el archivo del nivel que cargamos al principio. Podemos acceder a él desde aqui.
extern Personaje personaje;
extern float delta;

void inicializa_monstruos() {
    // Inicializamos la semilla de aleatoriedad
    srand(time(NULL));

    // Cargamos textura donde tenemos todos los monstruos
    sprite_monstruos=LoadTexture("resources\\mobs.png");

    // ANTES que nada lo más importante es inicializar los tipos de monstruos que tenemos en el juego
    for (int i=0; i<TIPOS_MONSTRUO; i++) {
        tipos[i].id=i; // Empezamos con id=0 para que coincidan con los indices del enum EtiquetasMonstruo;
        tipos[i].textura=TEXTURAS[i];
        tipos[i].fotograma=FOTOGRAMA[i];
        tipos[i].cuadricula_fotogramas=CUADRICULA_FOTOGRAMAS[i];
        tipos[i].hitbox_colision=HITBOX_COLISIONES[i];
        tipos[i].origen_hb=ORIGEN_HB[i];
        tipos[i].velocidad=VELOCIDADES[i];
        tipos[i].vida=MONSTRUO_VIDA[i];
        tipos[i].dmg=DMG[i];
    }

    // Dimensionamos el array con la cantidad de monstruos generados en el nivel actual
    cantidad_monstruos=CANTIDAD_MOBS_NIVEL[nivel_actual];
    monstruos=malloc(sizeof(Monstruo)*cantidad_monstruos); // ESTO DEBO RETOCARLO PARA QUE SEA DINÁMICO SEGÚN EL NIVEL QUE NOS ENCONTREMOS

    printf("%d", cantidad_monstruos);

    //

    // Tenemos las posiciones de aparición de los monstruos almacenados en el archivo de cada nivel. Leemos aquí los datos correspondientes.
    char *cursor=NULL;

    cursor=strstr(datos_archivo, ETIQ_OBJETOS_SALA)+strlen(ETIQ_OBJETOS_SALA); // Primero nos colocamos en el apartado de objetos del archivo para posteriormente poder acceder a "type" "x" e "y" sin confusiones con las capas de patrones.
    for (int i=0; i<cantidad_monstruos; i++) {
        char tipo_monstruo[15];
        EtiquetaMonstruo etiqueta;

        // Escaneamos localmente el nombre del tipo de monstruo que se haya leído
        sscanf(strstr(cursor, ETIQ_TIPO_OBJETO)+strlen(ETIQ_TIPO_OBJETO), "%[^\"]", tipo_monstruo);

        // tipo_monstruo debe coincidir con alguna constante del enum EtiquetaMonstruo que usamos como equivalentes a los indices numéricos para el array tipos
        //DEBUG //printf("\n%s", tipo_monstruo);
        etiqueta=conversion_char_enum(tipo_monstruo); // tipo_monstruo es de tipo char. Nos interesa antes convertirlo a tipo EtiquetaMonstruo

        // Inicializamos las variables cuyos datos son leídos del archivo de nivel actual.
        monstruos[i].tipo=tipos[etiqueta];
        sscanf(strstr(cursor, ETIQ_X_OBJETO)+strlen(ETIQ_X_OBJETO), "%f", &monstruos[i].posicion.x);
        sscanf(strstr(cursor, ETIQ_Y_OBJETO)+strlen(ETIQ_Y_OBJETO), "%f", &monstruos[i].posicion.y);
        cursor=strstr(cursor, "},")+2;

        // DEBUG // printf("\nx = %f - y = %f ", monstruos[i].posicion.x, monstruos[i].posicion.y);


        // Inicializamos el resto de las variables.
        monstruos[i].direccion_desplazamiento=(Vector2){0,0};
        monstruos[i].velocidad=monstruos[i].tipo.velocidad;
        monstruos[i].activo=true;
        if (monstruos[i].tipo.id == CABEZON || monstruos[i].tipo.id == SETA_MAGMA) monstruos[i].estado=M_PARADO;
        else monstruos[i].estado=M_DEAMBULANDO;
        monstruos[i].tiempo=0;
        monstruos[i].fotograma_actual=0;
        monstruos[i].fotograma=(Rectangle){monstruos[i].tipo.textura.x, monstruos[i].tipo.textura.y, monstruos[i].tipo.fotograma.ancho, monstruos[i].tipo.fotograma.alto};
        monstruos[i].textura_activa=M_DEAMBULANDO;

        // Incialiamos las posiciones de origen de los hitbox de los monstruos. Hay ciertos monstruos a los que se inicializa diferente a los demás
        if (monstruos[i].tipo.id == DUENDE_MAGMA || monstruos[i].tipo.id == CHISPA_MAGMA) {
            monstruos[i].hb_posicion.x=monstruos[i].posicion.x;
            monstruos[i].hb_posicion.y=monstruos[i].posicion.y;
        } else {
            monstruos[i].hb_posicion.x=monstruos[i].posicion.x+monstruos[i].tipo.origen_hb.x;
            monstruos[i].hb_posicion.y=monstruos[i].posicion.y+monstruos[i].tipo.origen_hb.y;
        }


        // Inicializamos el area del hitbox del combate de los monstruos.
    }


}

void actualizar_monstruos() {
    // Actualizamos los monstruos uno por uno
    for (int i=0; i<cantidad_monstruos; i++) {
        if (monstruos[i].activo) {
            actualizar_monstruo(&monstruos[i]);
        }

        //printf("%f - %f", monstruos[i].direccion_desplazamiento.x, monstruos[i].direccion_desplazamiento.y);
    }
}

void actualizar_monstruo(Monstruo* m) {
    //toDO // actualizar_fotogramas_monstruo(m);

    // Calculamos la distancia del monstruo con respecto al personaje. Lo guardamos primero en una variable aparte.
    Vector2 diferencia=Vector2Subtract(personaje.posicion, m->posicion);
    float distancia=Vector2Distance(m->posicion, personaje.posicion);

    //printf("%f - %f", diferencia.x, diferencia.y);
    // Comprobamos lo cerca que está el monstruo del personaje y modificamos su estado acorde a la situacion
    if (distancia <= RANGO_ATAQUE) m->estado=M_ATACANDO;
    else if (distancia <= RANGO_VISION) m->estado=M_PERSIGUIENDO;
    else {
        if (m->tipo.id == CABEZON || m->tipo.id == SETA_MAGMA) m->estado=M_PARADO;
        else m->estado=M_DEAMBULANDO;
    }

    //printf("\n%d", m->estado);

    // Calculada la distancia entre monstruo y personaje hacemos que el monstruo se mueva de una manera u otra.
    if (m->estado == M_PERSIGUIENDO) {
            m->direccion_desplazamiento=Vector2Normalize(diferencia);
            //printf("Persiguiendo: %f - %f", m->direccion_desplazamiento.x, m->direccion_desplazamiento.y);
    }
    else if (m->estado == M_DEAMBULANDO) {
        if (rand()%101 < PROB_CAMBIO_DIRECCION) { // No en todos los FPS va a estar cambiando de direccion
            m->direccion_desplazamiento = (Vector2){rand()%3-1, rand()%3-1};
            //printf("Deambulando: %f - %f", m->direccion_desplazamiento.x, m->direccion_desplazamiento.y);
        }
    }

    //printf("%f - %f", diferencia.x, diferencia.y);

    // Desplazamiento
    Vector2 destino = Vector2Add(m->posicion, Vector2Scale(m->direccion_desplazamiento, m->velocidad * delta));
    Vector2 destino_hitbox = Vector2Add(m->hb_posicion, Vector2Scale(m->direccion_desplazamiento, m->velocidad * delta));
    if (!suelo_transitable(destino_hitbox, MONSTRUO)) {
        return;
    }

    m->posicion=destino;
    m->hb_posicion=destino_hitbox;
}

void dibujar_monstruos() {
    for (int i=0; i<cantidad_monstruos; i++) {
        if (!monstruos[i].activo) continue;
        DrawTextureRec(sprite_monstruos, monstruos[i].fotograma, monstruos[i].posicion, WHITE);
        //DrawRectangle(monstruos[i].hb_posicion.x,monstruos[i].hb_posicion.y,monstruos[i].tipo.hitbox_colision.ancho,monstruos[i].tipo.hitbox_colision.alto,WHITE);
    }
}

void libera_monstruos() {
    free(monstruos);
}

EtiquetaMonstruo conversion_char_enum(char* tipo) {
    if (strcmp(tipo, "DUENDE_MAGMA") == 0) return DUENDE_MAGMA;
    else if (strcmp(tipo, "CHISPA_MAGMA") == 0) return CHISPA_MAGMA;
    else if (strcmp(tipo, "SLIME") == 0) return SLIME;
    else if (strcmp(tipo, "CENTINELA") == 0) return CENTINELA;
    else if (strcmp(tipo, "SETA_MAGMA") == 0) return SETA_MAGMA;
    else if (strcmp(tipo, "CABEZON") == 0) return CABEZON;

    // Llegados aquí es que algo ha ido muy mal
    exit(-1);
}
