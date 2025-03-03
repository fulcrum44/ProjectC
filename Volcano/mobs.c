#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "raylib.h"
#include "raymath.h"
// #include "mobsTypesData.h" Al compilar no parece pedir incluirse.
#include "mobs.h"
#include "character.h"
#include "cofres.h"


Monstruo *monstruos;
Tipo tipos[TIPOS_MONSTRUO];
int cantidad_monstruos;
Texture2D sprite_monstruos;
int orientacion_monstruo;

extern int nivel_actual;
extern char* datos_archivo; // En room.c nunca descargamos el archivo del nivel que cargamos al principio. Podemos acceder a él desde aqui.
extern Personaje personaje;
extern float delta;
extern int total_cofres;
extern Cofre *cofres;
extern int alto_losa;
extern int ancho_losa;

void inicializa_monstruos() {
    // Inicializamos la semilla de aleatoriedad
    srand(time(NULL));

    // Cargamos textura donde tenemos todos los monstruos
    sprite_monstruos=LoadTexture(TEXTURA_MONSTRUOS);

    // ANTES que nada lo más importante es inicializar los tipos de monstruos que tenemos en el juego
    for (int i=0; i<TIPOS_MONSTRUO; i++) {
        tipos[i].id=i; // Empezamos con id=0 para que coincidan con los indices del enum EtiquetasMonstruo;
        tipos[i].textura=TEXTURAS[i];
        tipos[i].fotograma=FOTOGRAMA[i];
        tipos[i].cuadricula_fotogramas=CUADRICULA_FOTOGRAMAS[i];
        tipos[i].hitbox_colision=HITBOX_COLISIONES[i];
        tipos[i].hitbox_combate=HITBOX_COMBATE[i];
        tipos[i].origen_hb=ORIGEN_HB_COLISIONES[i];
        tipos[i].velocidad=VELOCIDADES[i];
        tipos[i].vida=MONSTRUO_VIDA[i];
        tipos[i].dmg=DMG[i];
    }

    // Dimensionamos el array con la cantidad de monstruos generados en el nivel actual
    cantidad_monstruos=CANTIDAD_MOBS_NIVEL[nivel_actual];
    monstruos=malloc(sizeof(Monstruo)*total_cofres-1); // ESTO DEBO RETOCARLO POR SI HAGO QUE HAYA MAS DE UN COFRE CON ESTRELLA

    printf("CANTIDAD MONSTRUOS: %d", cantidad_monstruos);

    // Inicializamos el resto de las variables.
    for (int i=0; i<cantidad_monstruos; i++) {
        monstruos[i].tipo=tipos[CENTINELA];
        monstruos[i].direccion_desplazamiento=(Vector2){0,0};
        monstruos[i].velocidad=monstruos[i].tipo.velocidad;
        monstruos[i].activo=false; // ESTO DEPENDERÁ. LOS CENTINELAS COMIENZAN DESACTIVADOS
        monstruos[i].tiempo=0;
        monstruos[i].fotograma_actual=0;
        monstruos[i].fotograma=(Rectangle){monstruos[i].tipo.textura.x, monstruos[i].tipo.textura.y, monstruos[i].tipo.fotograma.ancho, monstruos[i].tipo.fotograma.alto};
        monstruos[i].estado=M_PARADO;
        monstruos[i].vida=monstruos[i].tipo.vida;

        // Inicializamos posicion centinela
        printf("\nTOTAL COFRES: %d", total_cofres);
        for (int j=0; j<total_cofres; j++) {
            if (!cofres[j].item_recolectable && !cofres[j].monstruo) {
                monstruos[i].posicion.x=cofres[j].losa.x * ancho_losa;
                monstruos[i].posicion.y=cofres[j].losa.y * alto_losa - 10;
                cofres[j].monstruo=true;
                cofres[j].indice_monstruo=i;
                break; // No salimos inmediatamente. El monstruo de la actual iteracion ya se le ha asignado un cofre disponible.
            }
        }

        // Incialiamos las posiciones de origen de los hitbox de los monstruos.
        // Este es para las colisiones con el entorno
        monstruos[i].hb_posicion.x=monstruos[i].posicion.x+monstruos[i].tipo.origen_hb.x;
        monstruos[i].hb_posicion.y=monstruos[i].posicion.y+monstruos[i].tipo.origen_hb.y;

        // Hitbox combate
        monstruos[i].hitbox_combate.x=monstruos[i].posicion.x;
        monstruos[i].hitbox_combate.y=monstruos[i].posicion.y;
        monstruos[i].hitbox_combate.width=monstruos[i].tipo.hitbox_combate.ancho;
        monstruos[i].hitbox_combate.height=monstruos[i].tipo.hitbox_combate.alto;
    }

    for (int i=0; i<cantidad_monstruos; i++) {
        printf("\n\nTipo monstruo: %d\n\n", monstruos[i].tipo.id);
    }
}

// VERSION ANTERIOR
/*void inicializa_monstruos() {
    // Inicializamos la semilla de aleatoriedad
    srand(time(NULL));

    // Cargamos textura donde tenemos todos los monstruos
    sprite_monstruos=LoadTexture(TEXTURA_MONSTRUOS);

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
    monstruos=malloc(sizeof(Monstruo)*total_cofres-1); // ESTO DEBO RETOCARLO POR SI HAGO QUE HAYA MAS DE UN COFRE CON ESTRELLA

    printf("%d", cantidad_monstruos);

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
        monstruos[i].activo=true; // ESTO DEPENDERÁ. LOS CENTINELAS COMIENZAN DESACTIVADOS
        if (monstruos[i].tipo.id == CABEZON || monstruos[i].tipo.id == SETA_MAGMA || monstruos[i].tipo.id == CENTINELA) {
            monstruos[i].estado=M_PARADO;
            monstruos[i].textura_activa=M_PARADO;
        } else {
            monstruos[i].estado=M_DEAMBULANDO;
            monstruos[i].textura_activa=M_DEAMBULANDO;
        }
        monstruos[i].tiempo=0;
        monstruos[i].fotograma_actual=0;
        monstruos[i].fotograma=(Rectangle){monstruos[i].tipo.textura.x, monstruos[i].tipo.textura.y, monstruos[i].tipo.fotograma.ancho, monstruos[i].tipo.fotograma.alto};

        // Inicializamos posicion centinela
        for (int i=0; i<total_cofres; i++) {
            if (!cofres[i].item_recolectable) {
                monstruos[i].posicion.x=cofres[i].losa.x * ancho_sala;
                monstruos[i].posicion.Y=cofres[i].losa.y * alto_sala;
            }
        }

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
}*/

void actualizar_monstruos() {
    // Actualizamos los monstruos uno por uno
    for (int i=0; i<cantidad_monstruos; i++) {
        if (monstruos[i].activo) {
            if (monstruos[i].tipo.id==CENTINELA) actualizar_centinela(&monstruos[i]);
        }

        //printf("%f - %f", monstruos[i].direccion_desplazamiento.x, monstruos[i].direccion_desplazamiento.y);
    }
}

void actualizar_centinela(Monstruo *m) {
    // Calculamos la distancia del monstruo con respecto al personaje. Lo guardamos primero en una variable aparte.
    Vector2 diferencia=Vector2Subtract(personaje.posicion, m->posicion);

    // Cambiamos la orientación que está mirando el monstruo según cómo se esté moviendo.
    if (diferencia.y < 0 && fabs(diferencia.y) > fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_ARRIBA;
    if (diferencia.y > 0 && fabs(diferencia.y) > fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_ABAJO;
    if (diferencia.x < 0 && fabs(diferencia.y) < fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_IZQ;
    if (diferencia.x > 0 && fabs(diferencia.y) < fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_DER;

    // Sabida la orientación en la que está mirando el monstruo ajustamos la fila de fotogramas a dibujar.
    m->fotograma.y=m->tipo.textura.y + (m->tipo.fotograma.alto * orientacion_monstruo);

    m->direccion_desplazamiento=Vector2Normalize(diferencia);
    // Desplazamiento
    Vector2 destino = Vector2Add(m->posicion, Vector2Scale(m->direccion_desplazamiento, m->velocidad * delta));
    Vector2 destino_hitbox = Vector2Add(m->hb_posicion, Vector2Scale(m->direccion_desplazamiento, m->velocidad * delta));

    // Actualizamos datos dependientes de la posicion del monstruo
    m->posicion=destino;
    m->hb_posicion=destino_hitbox;
    m->hitbox_combate.x=destino.x;
    m->hitbox_combate.y=destino.y;

    // Animacion
    actualizar_fotogramas_monstruo(m);
}

void actualizar_fotogramas_monstruo(Monstruo *m) {
    m->tiempo+=delta;


    // No se anima correctamente no sé por qué
    if (m->estado == M_ELIMNADO) {
        if (m->tiempo >= TIEMPO_FOTOGRAMA) {
            if (m->fotograma_actual == m->tipo.cuadricula_fotogramas.ancho-1) {
                m->activo=false;
            }
            m->fotograma_actual++;
            m->tiempo=0;
            m->fotograma.x=m->fotograma_actual * m->tipo.fotograma.ancho; // Avanzamos al siguiente fotograma en la textura
        }
    }

    // Animacion general
    if (m->tiempo >= TIEMPO_FOTOGRAMA) {
        m->fotograma_actual++;

        m->fotograma_actual%=m->tipo.cuadricula_fotogramas.ancho; // Ajustamos el fotograma actual al rango de fotogramas por fila en textura.
        m->tiempo=0; // Reiniciamos la variable
        m->fotograma.x=m->fotograma_actual * m->tipo.fotograma.ancho; // Avanzamos al siguiente fotograma en la textura
    }
}

void dibujar_monstruos() {
    for (int i=0; i<cantidad_monstruos; i++) {
        if (!monstruos[i].activo) continue;
        DrawTextureRec(sprite_monstruos, monstruos[i].fotograma, monstruos[i].posicion, WHITE);
        //DrawRectangle(monstruos[i].hitbox_combate.x, monstruos[i].hitbox_combate.y, monstruos[i].hitbox_combate.width, monstruos[i].hitbox_combate.height, YELLOW);
        //DrawRectangle(monstruos[i].hb_posicion.x,monstruos[i].hb_posicion.y,monstruos[i].tipo.hitbox_colision.ancho,monstruos[i].tipo.hitbox_colision.alto,WHITE);
    }
}

void libera_monstruos() {
    free(monstruos);
}

void muerte_monstruo(Monstruo *m) {
    if (m->vida <= 0) {
        m->estado=M_ELIMNADO; // Cambiamos el estado del monstruo para que sea detectado en la animación.
        m->fotograma_actual=0;
        m->fotograma.y=m->tipo.textura.y + (m->tipo.fotograma.alto * m->tipo.cuadricula_fotogramas.alto); // Antes de desactivar al monstruo por eliminación animaremos su muerte. Asignamos la fila correspondiente de fotogramas.
        return;
    }
    else return;
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
