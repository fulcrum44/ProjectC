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
#include "audio.h"


Monstruo *monstruos;
Tipo tipos[TIPOS_MONSTRUO];
int cantidad_monstruos;
Texture2D sprite_monstruos;
int orientacion_monstruo;
int monstruos_eliminados;

extern int nivel_actual;
extern char* datos_archivo; // En room.c nunca descargamos el archivo del nivel que cargamos al principio mientras se está ejecutando un nivel. Podemos acceder a él desde aqui.
extern Personaje personaje;
extern float delta;
extern int total_cofres;
extern Cofre *cofres;
extern int alto_losa;
extern int ancho_losa;

extern int total_items;
extern Sound *lista_sonidos;

void inicializa_monstruos() {
    monstruos=NULL; // Para reinicializaciones en niveles avanzados, por seguridad, iniciamos primero con NULL el punteros.
    monstruos_eliminados=0; // Iniciamos o reiniciamos según contexto.

    // Cargamos textura donde tenemos todos los monstruos
    sprite_monstruos=LoadTexture(TEXTURA_MONSTRUOS);

    // ANTES que nada lo más importante es inicializar los tipos de monstruos que tenemos en el juego
    for (int i=0; i<TIPOS_MONSTRUO; i++) {
        tipos[i].id=i; // Empezamos con id=0 para que coincidan con los indices del enum EtiquetasMonstruo;
        tipos[i].textura=TEXTURAS_MONSTRUOS[i];
        tipos[i].fotograma=FOTOGRAMA[i];
        tipos[i].cuadricula_fotogramas=CUADRICULA_FOTOGRAMAS[i];
        tipos[i].hitbox_colision=HITBOX_COLISIONES[i];
        tipos[i].hitbox_combate=HITBOX_COMBATE[i];
        tipos[i].origen_hb_colision=ORIGEN_HB_COLISIONES[i];
        tipos[i].origen_hb_combate=ORIGEN_HB_COMBATE[i];
        tipos[i].velocidad=VELOCIDADES[i];
        tipos[i].vida=MONSTRUO_VIDA[i];
        tipos[i].dmg=DMG[i];
    }

    // Dimensionamos el array con la cantidad de monstruos generados en el nivel actual
    cantidad_monstruos=total_cofres-total_items;
    monstruos=malloc(sizeof(Monstruo)*cantidad_monstruos);
    if (monstruos == NULL) {
        printf("\nERROR al reservar memoria para los monstruos");
        exit(-1);
    }

    // Inicializamos el resto de las variables.
    for (int i=0; i<cantidad_monstruos; i++) {
        monstruos[i].tipo=tipos[CENTINELA];
        monstruos[i].direccion_desplazamiento=(Vector2){0,0};
        monstruos[i].velocidad=monstruos[i].tipo.velocidad;
        monstruos[i].activo=false;
        monstruos[i].tiempo=0;
        monstruos[i].fotograma_actual=0;
        monstruos[i].fotograma=(Rectangle){monstruos[i].tipo.textura.x, monstruos[i].tipo.textura.y, monstruos[i].tipo.fotograma.ancho, monstruos[i].tipo.fotograma.alto};
        monstruos[i].estado=M_PARADO;
        monstruos[i].vida=monstruos[i].tipo.vida;
        monstruos[i].danyo=monstruos[i].tipo.dmg;
        monstruos[i].duracion_ataque=DELAY_ATAQUE;

        // Inicializamos posicion centinela
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
        monstruos[i].hb_posicion.x=monstruos[i].posicion.x+monstruos[i].tipo.origen_hb_colision.x;
        monstruos[i].hb_posicion.y=monstruos[i].posicion.y+monstruos[i].tipo.origen_hb_colision.y;

        // Hitbox combate
        monstruos[i].hitbox_combate.x=monstruos[i].posicion.x;
        monstruos[i].hitbox_combate.y=monstruos[i].posicion.y;
        monstruos[i].hitbox_combate.width=monstruos[i].tipo.hitbox_combate.ancho;
        monstruos[i].hitbox_combate.height=monstruos[i].tipo.hitbox_combate.alto;
    }

    // El nivel puede tener otro tipo de monstruos que no hayan sido generados dependiendo de los cofres que no tengan un item.
    // Estos habrán sido configurados desde TILED como objetos y definiendo allí sus posiciones de aparici´n
    // Leemos aquí los datos correspondientes a las posicione de estos almacenados en el .tmj del nivel actual
    char *cursor=NULL;

    cursor=strstr(datos_archivo, ETIQ_NAME_MOBS_SALA);
    // Si al buscar alguna ocurrencia de la etiqueta indicada no se encuentra ninguna, significa que no hay ninguna capa de objetos con monstruos configurados. NO hay monstruos que leer del .tmj por lo que terminamos la inicialización de los monstruos del nivel.
    if (cursor == NULL) return;

    while (true) {
        // Cada repetición que hace el bucle es porque ya se ha podido determinar que hay, al menos, un monstruo más que leer.
        // Redimensionamos el array de los monstruos para poder almacenar uno más de los que ya hay.
        monstruos=realloc(monstruos, sizeof(Monstruo) * (cantidad_monstruos+1));
        if (monstruos == NULL) {
            printf("\nERROR al reservar memoria para los monstruos");
            exit(-1);
        }

        char tipo_monstruo[30];
        EtiquetaMonstruo etiqueta;

        // Escaneamos localmente el nombre del tipo de monstruo que se haya leído
        sscanf(strstr(cursor, ETIQ_TIPO_OBJETO)+strlen(ETIQ_TIPO_OBJETO), "%[^\"]", tipo_monstruo);

        if (strcmp(tipo_monstruo, "aparicion_personaje") == 0) break; // No hay más monstruos que leer del archivo.

        // tipo_monstruo debe coincidir con alguna constante del enum EtiquetaMonstruo que usamos como equivalentes a los indices numéricos para el array tipos
        etiqueta=conversion_char_enum(tipo_monstruo); // tipo_monstruo es de tipo char. Nos interesa antes convertirlo a tipo EtiquetaMonstruo

        // Inicializamos las variables cuyos datos son leídos del archivo de nivel actual.
        monstruos[cantidad_monstruos].tipo=tipos[etiqueta]; // AQUÍ YA ALMACENAMOS DEFINITIVAMENTE DE QUE TIPO ES EL MONSTRUO QUE SE ESTÁ CREANDO. A partir de aquí ya podremos inicializar ciertas variables en base a esto.
        cursor=strstr(cursor, ETIQ_X_OBJETO);
        sscanf(strstr(cursor, ETIQ_X_OBJETO)+strlen(ETIQ_X_OBJETO), "%f", &monstruos[cantidad_monstruos].posicion.x);
        cursor=strstr(cursor, ETIQ_Y_OBJETO);
        sscanf(strstr(cursor, ETIQ_Y_OBJETO)+strlen(ETIQ_Y_OBJETO), "%f", &monstruos[cantidad_monstruos].posicion.y);
        cursor=strstr(cursor, "},")+2;

        cursor=strstr(cursor, ETIQ_TIPO_OBJETO); // Nos movemos al siguiente tipo de objeto, si lo hay. Puede que sea un monstruo más o el punto de aparición del personaje. Esto último es el último objeto configurado en todos los niveles.

        // Inicializamos el resto de las variables.
        monstruos[cantidad_monstruos].direccion_desplazamiento=(Vector2){0,0};
        monstruos[cantidad_monstruos].velocidad=monstruos[cantidad_monstruos].tipo.velocidad;
        monstruos[cantidad_monstruos].activo=true;
        if (monstruos[cantidad_monstruos].tipo.id == SETA_MAGMA) {
            monstruos[cantidad_monstruos].estado=M_PARADO;
        } else { // EN DESUSO. ESTÁ PENSADO PARA MONSTRUOS NO IMPLEMENTADOS.
            monstruos[cantidad_monstruos].estado=M_DEAMBULANDO;
        }
        monstruos[cantidad_monstruos].tiempo=0;
        monstruos[cantidad_monstruos].fotograma_actual=0;
        monstruos[cantidad_monstruos].fotograma=(Rectangle){monstruos[cantidad_monstruos].tipo.textura.x, monstruos[cantidad_monstruos].tipo.textura.y, monstruos[cantidad_monstruos].tipo.fotograma.ancho, monstruos[cantidad_monstruos].tipo.fotograma.alto};
        monstruos[cantidad_monstruos].vida=monstruos[cantidad_monstruos].tipo.vida;
        monstruos[cantidad_monstruos].danyo=monstruos[cantidad_monstruos].tipo.dmg;
        monstruos[cantidad_monstruos].duracion_ataque=DELAY_ATAQUE;

        // Incialiamos las posiciones de origen de los hitbox de los monstruos. Hay ciertos monstruos a los que se inicializa diferente a los demás
        // Este es para las colisiones con el entorno
        if (monstruos[cantidad_monstruos].tipo.id == DUENDE_MAGMA || monstruos[cantidad_monstruos].tipo.id == CHISPA_MAGMA) { // EN DESUSO. ESTÁ PENSADO PARA MONSTRUOS NO IMPLEMENTADOS. EL ELSE ES EL QUE SE ACTIVA SIEMPRE POR AHORA.
            monstruos[cantidad_monstruos].hb_posicion.x=monstruos[cantidad_monstruos].posicion.x;
            monstruos[cantidad_monstruos].hb_posicion.y=monstruos[cantidad_monstruos].posicion.y;
        } else {
            monstruos[cantidad_monstruos].hb_posicion.x=monstruos[cantidad_monstruos].posicion.x+monstruos[cantidad_monstruos].tipo.origen_hb_colision.x;
            monstruos[cantidad_monstruos].hb_posicion.y=monstruos[cantidad_monstruos].posicion.y+monstruos[cantidad_monstruos].tipo.origen_hb_colision.y;
        }

        // Hitbox combate
        if (monstruos[cantidad_monstruos].tipo.id == SETA_MAGMA) {
            monstruos[cantidad_monstruos].hitbox_combate.x=monstruos[cantidad_monstruos].posicion.x+monstruos[cantidad_monstruos].tipo.origen_hb_combate.x;
            monstruos[cantidad_monstruos].hitbox_combate.y=monstruos[cantidad_monstruos].posicion.y+monstruos[cantidad_monstruos].tipo.origen_hb_combate.y;
            monstruos[cantidad_monstruos].hitbox_combate.width=monstruos[cantidad_monstruos].tipo.hitbox_combate.ancho;
            monstruos[cantidad_monstruos].hitbox_combate.height=monstruos[cantidad_monstruos].tipo.hitbox_combate.alto;

        } else {
            monstruos[cantidad_monstruos].hitbox_combate.x=monstruos[cantidad_monstruos].posicion.x;
            monstruos[cantidad_monstruos].hitbox_combate.y=monstruos[cantidad_monstruos].posicion.y;
            monstruos[cantidad_monstruos].hitbox_combate.width=monstruos[cantidad_monstruos].tipo.hitbox_combate.ancho;
            monstruos[cantidad_monstruos].hitbox_combate.height=monstruos[cantidad_monstruos].tipo.hitbox_combate.alto;
        }

        cantidad_monstruos++;
    }
}

void actualizar_monstruos() {
    // Actualizamos los monstruos uno por uno
    for (int i=0; i<cantidad_monstruos; i++) {
        if (monstruos[i].activo) {
            if (monstruos[i].tipo.id==CENTINELA) actualizar_centinela(&monstruos[i]);
            if (monstruos[i].tipo.id==SETA_MAGMA) actualizar_seta_magma(&monstruos[i]);
        }
    }
}

void actualizar_centinela(Monstruo *m) {
    if (m->estado != M_ELIMINADO) { // Solo controlaremos las acciones del monstruo como el movimiento y el ataque si el monstruo no está eliminado aparte de estar también activo.
        // Calculamos la distancia del monstruo con respecto al personaje. Lo guardamos primero en una variable aparte.
        Vector2 diferencia=Vector2Subtract((Vector2){personaje.hitboxes[TORSO].x, personaje.hitboxes[TORSO].y}, m->posicion);

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

        // Comprobamos si el hitbox del combate del monstruo está colisionando con del personaje
        ataque_centinela(m);
    }

    // Animacion
    actualizar_fotogramas_monstruo(m);
}

void actualizar_seta_magma(Monstruo *m) {
    // Calculamos la distancia del monstruo con respecto al personaje. Lo guardamos primero en una variable aparte.
    Vector2 diferencia=Vector2Subtract((Vector2){personaje.hitboxes[TORSO].x, personaje.hitboxes[TORSO].y}, m->posicion); // UN BUSCADOR DE CAMINOS SERÍA BENEFICIOSO PARA PERFECCIONAR.

    if (m->estado != M_ELIMINADO) {
        if (fabs(diferencia.x) < RANGO_PERSECUCION && fabs(diferencia.y) < RANGO_PERSECUCION) {
            if (m->estado != M_ATACANDO) m->estado=M_PERSIGUIENDO;
        }
        else m->estado=M_PARADO;
    }

    float margen=5.5f;
    if (fabs(m->posicion.x - personaje.posicion.x) < margen || fabs(m->posicion.y - personaje.posicion.y) < margen) m->direccion_desplazamiento=(Vector2){0, 0};

    if (m->estado != M_ELIMINADO && m->estado != M_PARADO) { // Solo controlaremos las acciones del monstruo como el movimiento y el ataque si el monstruo no está eliminado aparte de estar también activo.
        // Cambiamos la orientación que está mirando el monstruo según cómo se esté moviendo.
        if (diferencia.y < 0 && fabs(diferencia.y) > fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_DER;
        if (diferencia.y > 0 && fabs(diferencia.y) > fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_DER;
        if (diferencia.x < 0 && fabs(diferencia.y) < fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_ABAJO;
        if (diferencia.x > 0 && fabs(diferencia.y) < fabs(diferencia.x)) orientacion_monstruo=ORIENTACION_MONSTRUO_ABAJO;

        // Sabida la orientación en la que está mirando el monstruo ajustamos la fila de fotogramas a dibujar.
        m->fotograma.y=m->tipo.textura.y + (m->tipo.fotograma.alto * orientacion_monstruo);

        //m->direccion_desplazamiento=Vector2Normalize(diferencia);
        if (m->direccion_desplazamiento.x == 0 && m->direccion_desplazamiento.y == 0) {
            desplazamiento_en_cruz(m, diferencia);
        }

        // Desplazamiento
        Vector2 destino = Vector2Add(m->posicion, Vector2Scale(m->direccion_desplazamiento, m->velocidad * delta));
        Vector2 destino_hitbox = Vector2Add(m->hb_posicion, Vector2Scale(m->direccion_desplazamiento, m->velocidad * delta));

        if (!suelo_transitable(destino_hitbox, MONSTRUO)) {
            m->direccion_desplazamiento.x=0;
            m->direccion_desplazamiento.y=0;
            return;
        }

        // Actualizamos datos dependientes de la posicion del monstruo
        m->posicion=destino;
        m->hb_posicion=destino_hitbox;
        m->hitbox_combate.x=destino.x;
        m->hitbox_combate.y=destino.y;

    }

    if (m->estado == M_PARADO) { // Este monstruo al estar parado no tiene animación. Solo se dibuja su fotograma de estar parado que es el primero en su rectangulo de fotogramas en la textura.
        m->fotograma.x=m->tipo.textura.x;
        m->fotograma.y=m->tipo.textura.y;
    } else {
        // Comprobamos si el hitbox del combate del monstruo está colisionando con del personaje
        ataque_seta_magma(m);

        // Animacion
        actualizar_fotogramas_monstruo(m);
    }

    // Nota mental: un bug saluda de ven cuando.
}

void actualizar_fotogramas_monstruo(Monstruo *m) {
    m->tiempo+=delta;

    // No se anima correctamente no sé por qué
    if (m->estado == M_ELIMINADO) {
        if (m->tiempo >= 0.3f) { // Hace falta ajustarlo mejor quizás
            if (m->fotograma_actual == m->tipo.cuadricula_fotogramas.ancho-1) {
                m->activo=false;
            }
            m->fotograma_actual++;
            printf("\nFotograma actual monstruo: %d", m->fotograma_actual);
            m->tiempo=0;
            m->fotograma.x=m->fotograma_actual * m->tipo.fotograma.ancho + m->tipo.textura.x; // Avanzamos al siguiente fotograma en la textura. Sumo m->tipo.textura.x porque los calculos previos hay que hacerlos con la posicion de origen del eje X donde está el cuadro de fotogramas del monstruo.
        }
    } else {
        // Animacion general
        if (m->tiempo >= TIEMPO_FOTOGRAMA) {
            m->fotograma_actual++;

            m->fotograma_actual%=m->tipo.cuadricula_fotogramas.ancho; // Ajustamos el fotograma actual al rango de fotogramas por fila en textura.
            m->tiempo=0; // Reiniciamos la variable
            m->fotograma.x=m->fotograma_actual * m->tipo.fotograma.ancho + m->tipo.textura.x; // Avanzamos al siguiente fotograma en la textura
        }
    }
}

void dibujar_monstruos() {
    for (int i=0; i<cantidad_monstruos; i++) {
        if (!monstruos[i].activo) continue;
        DrawTextureRec(sprite_monstruos, monstruos[i].fotograma, monstruos[i].posicion, WHITE);
    }
}

void libera_monstruos() {
    free(monstruos);
}

void muerte_monstruo(Monstruo *m) {
    if (m->estado != M_ELIMINADO) {
        if (m->vida <= 0) {
            m->estado=M_ELIMINADO; // Cambiamos el estado del monstruo para que sea detectado en la animación.
            m->fotograma_actual=0; // Nos aseguramos que empezaremos por el primer fotograma de la animación
            m->fotograma.x=m->tipo.textura.x; // Con fotograma_actual=0 no sería necesario reiniciar esta variable. Lo hacemos igualmente para evitar inconsistencias.
            m->fotograma.y=m->tipo.textura.y + (m->tipo.fotograma.alto * (m->tipo.cuadricula_fotogramas.alto-1)); // Antes de desactivar al monstruo por eliminación animaremos su muerte. Asignamos la fila correspondiente de fotogramas.
            monstruos_eliminados++;
        return;
        }
        else return;
    }
}

void ataque_centinela(Monstruo *m) {
    if (m->estado == M_ATACANDO) {
        m->duracion_ataque--;

        if (m->duracion_ataque <= 0) {
            m->estado=M_PERSIGUIENDO;
            m->duracion_ataque=DELAY_ATAQUE;
        }

        return;
    } else {
        if (CheckCollisionRecs(m->hitbox_combate, personaje.hitboxes[TORSO])) {
            PlaySound(lista_sonidos[DANYO_RECIBIDO]); // Sonido de personaje ha recibido daño
            m->estado=M_ATACANDO;
            m->duracion_ataque--;
            personaje.vida-=m->danyo;

            muerte_personaje(&personaje);
        }
    }
}

void ataque_seta_magma(Monstruo *m) {
    if (m->estado == M_ATACANDO) {
        m->duracion_ataque--;

        if (m->duracion_ataque <= 0) {
            m->estado=M_PERSIGUIENDO;
            m->duracion_ataque=DELAY_ATAQUE;
        }

        return;
    } else {
        if (CheckCollisionRecs(m->hitbox_combate, personaje.hitboxes[TORSO])) {
            PlaySound(lista_sonidos[DANYO_RECIBIDO]); // Sonido de personaje ha recibido daño
            m->estado=M_ATACANDO;
            m->duracion_ataque--;
            personaje.vida-=m->danyo;

            muerte_personaje(&personaje);
        }
    }
}

void desplazamiento_en_cruz(Monstruo *m, Vector2 diferencia) {
    if (diferencia.y < 0 && fabs(diferencia.y) > fabs(diferencia.x)) {
        m->direccion_desplazamiento.y=-1;
        return;
    }
    if (diferencia.y > 0 && fabs(diferencia.y) > fabs(diferencia.x)) {
        m->direccion_desplazamiento.y=1;
        return;
    }
    if (diferencia.x < 0 && fabs(diferencia.y) < fabs(diferencia.x)) {
        m->direccion_desplazamiento.x=-1;
        return;
    }
    if (diferencia.x > 0 && fabs(diferencia.y) < fabs(diferencia.x)) {
        m->direccion_desplazamiento.x=1;
        return;
    }
}

void retroceso_monstruo(Monstruo *m, int orientacion_personaje) {
    if (orientacion_personaje == ORIENTACION_ABAJO) {
        m->posicion.y+=RETROCESO;
    }
    if (orientacion_personaje == ORIENTACION_ARRIBA) {
        m->posicion.y-=RETROCESO;
    }
    if (orientacion_personaje == ORIENTACION_DER) {
        m->posicion.x+=RETROCESO;
    }
    if (orientacion_personaje == ORIENTACION_IZQ) {
        m->posicion.x-=RETROCESO;
    }
}

EtiquetaMonstruo conversion_char_enum(char* tipo) { // EN DESUSO EN ESTA VERSION
    if (strcmp(tipo, "DUENDE_MAGMA") == 0) return DUENDE_MAGMA;
    else if (strcmp(tipo, "CHISPA_MAGMA") == 0) return CHISPA_MAGMA;
    else if (strcmp(tipo, "SLIME") == 0) return SLIME;
    else if (strcmp(tipo, "CENTINELA") == 0) return CENTINELA;
    else if (strcmp(tipo, "SETA_MAGMA") == 0) return SETA_MAGMA;
    else if (strcmp(tipo, "CABEZON") == 0) return CABEZON;

    // Llegados aquí es que algo ha ido muy mal
    exit(-1);
}
