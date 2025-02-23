#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "raylib.h"
// #include "mobsTypesData.h" Al compilar no parece pedir incluirse.
#include "mobs.h"


Monstruo *monstruos;
Tipo tipos[TIPOS_MONSTRUO];
int cantidad_monstruos;
Texture2D sprite_monstruos;

extern int nivel_actual;
extern char* datos_archivo; // En room.c nunca descargamos el archivo del nivel que cargamos al principio. Podemos acceder a él desde aqui.

void inicializa_monstruos() {
    sprite_monstruos=LoadTexture("resources\\mobs.png");

    // ANTES que nada lo más importante es inicializar los tipos de monstruos que tenemos en el juego
    for (int i=0; i<TIPOS_MONSTRUO; i++) {
        tipos[i].id=i; // Empezamos con id=0 para que coincidan con los indices del enum EtiquetasMonstruo;
        tipos[i].textura=TEXTURAS[i];
        tipos[i].fotograma=FOTOGRAMA[i];
        tipos[i].cuadricula_fotogramas=CUADRICULA_FOTOGRAMAS[i];
        tipos[i].hitbox_colision=HITBOX_COLISIONES[i];
        tipos[i].origen_hb=ORIGEN_HB[i];
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
        monstruos[i].estado=M_DEAMBULANDO;
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

void dibujar_monstruos() {
    for (int i=0; i<cantidad_monstruos; i++) {
        if (!monstruos[i].activo) continue;
        DrawTextureRec(sprite_monstruos, monstruos[i].fotograma, monstruos[i].posicion, WHITE);
    }
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
