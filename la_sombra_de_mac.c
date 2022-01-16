#include "la_sombra_de_mac.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utiles.h"

const char ARRIBA = 'W';
const char ABAJO = 'S';
const char IZQUIERDA = 'A';
const char DERECHA = 'D';
const char COMPRAR_VIDA = 'V';
const char MAC = 'M';
#define EMOJI_MAC "\U0001f466"
const char BLOO = 'B';
#define EMOJI_BLOO "\U0001f47b"
const char GUION = '-';
#define EMOJI_TERRENO "\U0001f532"
const char PUERTA = 'D';
#define EMOJI_PUERTA "\U0001f6aa"
const char ESCALERA = 'E';
#define EMOJI_ESCALERA "\U0001fa9c"
const char LLAVE = 'L';
#define EMOJI_LLAVE "\U0001f511"
const char MONEDA = 'C';
#define EMOJI_MONEDA "\U0001fa99"
const char POZO = 'W';
#define EMOJI_POZO "\U0001f573"
const char INTERRUPTOR = 'O';
#define EMOJI_INTERRUPTOR "\U0001f579"
const char PORTAL = 'P';
#define EMOJI_PORTAL "\U0001f300"
const char VELA = 'V';
#define EMOJI_VELA "\U0001f56f"
const char PARED = '|';
#define EMOJI_PARED "\U0001f9f1"
const int tope_velas[MAX_NIVELES] = {5, 10, 12};
const int tope_pozos[MAX_NIVELES] = {15, 20, 30};
const int tope_interruptores[MAX_NIVELES] = {1, 2, 4};
const int tope_portales[MAX_NIVELES] = {0, 2, 4};
const int tope_escaleras[MAX_NIVELES] = {10, 15, 15};
const int tope_monedas[MAX_NIVELES] = {10, 15, 15};
const int tope_llaves[MAX_NIVELES] = {0, 1, 1};
const int tope_puertas[MAX_NIVELES] = {1, 1 , 1};
#define NIVEL_1 1
#define NIVEL_2 2
#define NIVEL_3 3
#define NO_ENCONTRADO -1
#define GANADO 1
#define JUGANDO 0
#define PERDIDO -1

/*
*    Función que recibe dos coordenadas, devuelve true si las
* coordenadas son iguales, sino false.
*/
bool coordenadas_iguales(coordenada_t a, coordenada_t b){
    return (a.col == b.col && a.fila == b.fila);
}

/*
*   Función que dados una altura máxima y un ancho máximo
* devuelve una coordenada aleatoria. Una posición nunca podrá
* tener un valor negativo, ni ser mayor a sus máximos.
*/
coordenada_t coordenada_aleatoria(int max_alto, int max_ancho){
    coordenada_t posicion_aleatoria;
    posicion_aleatoria.fila = rand() % max_alto;
    posicion_aleatoria.col = rand() % max_ancho;
    return posicion_aleatoria;
}

/*
*   Procedimiento que dejará la estructura personaje_t en un
* estado válido para iniciar el juego.
* 'arranque_personaje' es un parámetro que representa la posición 
* en la arrancará el personaje.
*/
void inicializar_personaje(personaje_t *ref_personaje, coordenada_t arranque_personaje){
    (*ref_personaje).vida = 3;
    (*ref_personaje).puntos = 0;
    (*ref_personaje).tiene_llave = false;
    (*ref_personaje).interruptor_apretado = false;
    (*ref_personaje).posicion.col = arranque_personaje.col;
    (*ref_personaje).posicion.fila = arranque_personaje.fila;
}

/*
*   Procedimiento que dejará una estructura elemento_t que
* represente a la sombra, en un estado válido para iniciar el juego. 
* 'arranque_sombra' es un parámetro que representa la posición 
* en la arrancará la sombra.
*/
void inicializar_sombra(sombra_t *ref_sombra, coordenada_t arranque_sombra){
    (*ref_sombra).esta_viva = true;
    (*ref_sombra).posicion.col = arranque_sombra.col;
    (*ref_sombra).posicion.fila = arranque_sombra.fila;
}

/*
*   La función recibe un caracter y devuelve true
* si el movimiento recibido concuerda con la convención
* propuesta, sino false.
* -> W: Si el personaje debe moverse para la arriba. 
* -> A: Si el personaje debe moverse para la izquierda.
* -> S: Si el personaje debe moverse para la abajo.
* -> D: Si el personaje debe moverse para la derecha.
*/
bool es_movimiento_valido(char movimiento){
    return (movimiento == ARRIBA || movimiento == ABAJO || movimiento == IZQUIERDA || movimiento == DERECHA || movimiento == COMPRAR_VIDA);
}

/*
*   Procedimineto que se encargará de pedirle al usuario
* que ingrese un movimiento hasta que sea válido.
*/
void pedir_movimiento(char *ref_movimiento){
    printf("Ingrese un movimiento por favor.\nW(ARRIBA) S(ABAJO) D(DERECHA) A(IZQUIERDA)\n");
    scanf(" %c", ref_movimiento);
    while (!es_movimiento_valido(*ref_movimiento)){
        printf("Ingrese un movimiento válido por favor.\nW(ARRIBA) S(ABAJO) D(DERECHA) A(IZQUIERDA)\n");
        scanf(" %c", &(*ref_movimiento));
    };
}

/*
*    La función recibe una coordenada, devuelve true
* si la posición dentro de las dimensiones dadas, sino false.
* Una posición nunca podrá tener un valor negativo y no tendrá un 
* valor más alto que los máximos.
*/
bool esta_dentro_rango(coordenada_t posicion, int max_alto, int max_ancho){
    bool en_rango = false;
    if((posicion.fila < max_alto && posicion.fila >= 0) && (posicion.col < max_ancho && posicion.col >=0)){
        en_rango = true;
    }
    return en_rango;
}

/*
*    La función recibe un vector de elementos, su respectivo tope y una coordenada,
* devuelve la posición del elemento del vector, que coincida con la coordenada pasada 'posicion',
* si no se encuentra ningún elemento, se devolverá -1. 
*/
int buscar_elemento(elemento_t elementos[MAX_ELEMENTOS], int tope, coordenada_t posicion){
    int i = 0;
    int posicion_elemento = NO_ENCONTRADO;

    while (i < tope && posicion_elemento == -1){
        if (coordenadas_iguales(elementos[i].coordenada, posicion)){
            posicion_elemento = i;
        }
        i++;
    }
    return posicion_elemento;
}

/*
*  Función que recibe un vector de coordenadas, con su tope(por valor) y una posición.
* Se encarga de decir si hay o no coincidencia entre la posición de alguna de las coordenadas del vector y 
* la posición pasada por parámetro. De haber coincidencia, significa que en la posición hay una pared.
*/
bool es_pared(coordenada_t paredes[MAX_PAREDES], int tope_paredes, coordenada_t posicion){
    bool es_pared = false;

    for(int i=0; i < tope_paredes; i++){
        if(coordenadas_iguales(paredes[i], posicion)){
            es_pared = true;
        }
    }

    return es_pared;
}

/*
*   Función que recibe un vector de elementos, con su tope(por valor) y una posición.
* Se encarga de decir si hay o no coincidencia entre la posición de alguno de los elementos 
* del vector y la posición pasada por parámetro. De haber coincidencia, significa que hay 
* una escalera en esa posición.
*/
bool es_escalera(elemento_t elementos[MAX_ELEMENTOS], int tope, coordenada_t posicion){
    bool hay_escalera = false;

    int posicion_buscado = buscar_elemento(elementos, tope, posicion);

    if(posicion_buscado != NO_ENCONTRADO && elementos[posicion_buscado].tipo == ESCALERA){
        hay_escalera = true;
    }

    return hay_escalera;
}

/*
*  Procedimiento que recibe un vector de elementos, con su tope(por referencia) y el número de una posición del vector.
* Se encarga de eliminar el elemento del vector que se encuentra en la posición del número pasado por parámetro.
*/
void eliminar_elemento(elemento_t elementos[MAX_ELEMENTOS], int * tope_elementos, int posicion_buscado){
    
    if(posicion_buscado == NO_ENCONTRADO){
        return;
    }

    for(int i=posicion_buscado; i < (*tope_elementos); i++){
        elementos[i] = elementos[i+1];
    }
    (*tope_elementos)--;

}

/*
*  Procedimiento que recibe al personaje(por referencia), un movimiento, un vector
* de coordenadas y su tope y según el movimiento ingresado, analiza que ocurre con
* la posición del personaje en el terreno y ejecuta una respuesta.
*/
void mover_mac(personaje_t *ref_mac, char movimiento, coordenada_t paredes[MAX_PAREDES], int tope, juego_t juego){
    if(movimiento == ARRIBA){
        if(esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS)){
            (*ref_mac).posicion.fila -= 1;  
        }  
        if(!esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_mac).posicion)){
                (*ref_mac).posicion.fila += 1;
        }
    }
    if(movimiento == ABAJO){
        if(esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS)){
            (*ref_mac).posicion.fila += 1;
        }
        if(!esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_mac).posicion)){
            (*ref_mac).posicion.fila -= 1;
        }
    }
    if(movimiento == DERECHA){
        if(esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS)){
            (*ref_mac).posicion.col += 1;
        }
        if(!esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_mac).posicion)){
            (*ref_mac).posicion.col -= 1;
        }
    }
    if(movimiento == IZQUIERDA){
        if(esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS)){
            (*ref_mac).posicion.col -= 1;
        }
        if(!esta_dentro_rango((*ref_mac).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_mac).posicion)){
            (*ref_mac).posicion.col += 1;
        }
    }
}

/*
*  Procedimiento que recibe a la sombra(por referencia), un movimiento, un vector de
* coordenadas con su tope y según el movimiento, analiza que ocurre con la posición 
* de la sombra de forma vertical, es decir, en las filas.
*/
void mover_sombra_en_fila(sombra_t *ref_sombra, char movimiento, coordenada_t paredes[MAX_PAREDES], int tope ){
    if(movimiento == ARRIBA && (*ref_sombra).esta_viva){
        if(esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS)){
            (*ref_sombra).posicion.fila -= 1;    
        }
        if(!esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_sombra).posicion)){
                (*ref_sombra).posicion.fila += 1;
        }
    }
    if(movimiento == ABAJO && (*ref_sombra).esta_viva){
        if(esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS)){
            (*ref_sombra).posicion.fila += 1;
        }
        if(!esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_sombra).posicion)){
            (*ref_sombra).posicion.fila -= 1;
        }
    }
}

/*
*  Procedimiento que recibe a la sombra(por referencia), al personaje por referencia,
* un movimiento, un vector de coordenadas con su tope y según el movimiento, analiza 
* que ocurre con la posición de la sombra de forma horizontal, es decir, en las columnas.
*/
void mover_sombra_en_columna(sombra_t *ref_sombra, personaje_t * ref_personaje, char movimiento, coordenada_t paredes[MAX_PAREDES], int tope ){
    if(movimiento == DERECHA && (*ref_sombra).esta_viva){
        if(esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) && (*ref_personaje).interruptor_apretado == false){
            (*ref_sombra).posicion.col -= 1;
        }
        if(esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) && (*ref_personaje).interruptor_apretado == true){
            (*ref_sombra).posicion.col += 1;
        }
        if((!esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_sombra).posicion)) && (*ref_personaje).interruptor_apretado == false){
            (*ref_sombra).posicion.col += 1;
        }
        if((!esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_sombra).posicion)) && (*ref_personaje).interruptor_apretado == true){
            (*ref_sombra).posicion.col -= 1;
        }
    }
    if(movimiento == IZQUIERDA && (*ref_sombra).esta_viva){
        if(esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) && (*ref_personaje).interruptor_apretado == false){
            (*ref_sombra).posicion.col += 1;
        }
        if(esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) && (*ref_personaje).interruptor_apretado == true){
            (*ref_sombra).posicion.col -= 1;
        }
        if((!esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_sombra).posicion)) && (*ref_personaje).interruptor_apretado == false){
            (*ref_sombra).posicion.col -= 1;
        }
        if((!esta_dentro_rango((*ref_sombra).posicion, MAX_FILAS, MAX_COLUMNAS) || es_pared(paredes,tope,(*ref_sombra).posicion)) && (*ref_personaje).interruptor_apretado == true){
            (*ref_sombra).posicion.col += 1;
        }
    }
}

/*
*  Función que recibe como parámetro dos coordenadas.
* Devuelve el valor absoluto de la distacia que hay entre ambas coordenadas.
*/
int distancia_entre_coordenadas(coordenada_t primer_coordenada, coordenada_t segundo_coordenada) {
	int distancia_fila = primer_coordenada.fila - segundo_coordenada.fila;
	if(distancia_fila < 0){
		distancia_fila = distancia_fila * (-1);
	};
	int distancia_columna = primer_coordenada.col - segundo_coordenada.col;
	if(distancia_columna < 0){
		distancia_columna = distancia_columna * (-1);
	};
	return distancia_fila + distancia_columna;
}
/*
*    El procedimiento recibe el personaje y la sombra por referencia y el movimiento,
* y según este último, los mueve acordemente.
* 
* El personaje se mueve de la forma:
* -> W: Si el personaje debe moverse para la arriba. 
* -> A: Si el personaje debe moverse para la izquierda.
* -> S: Si el personaje debe moverse para la abajo.
* -> D: Si el personaje debe moverse para la derecha.
*
*   La sombra se mueve de igual forma para el eje vertical, pero
* para el eje horizontal se moverá de forma inversa:
*
* -> A: Si el personaje debe moverse para la derecha.
* -> D: Si el personaje debe moverse para la izquierda.
*/
void mover_personaje(personaje_t *ref_personaje, sombra_t *ref_sombra, char movimiento, juego_t juego){
    mover_mac(ref_personaje, movimiento, juego.niveles[juego.nivel_actual - 1].paredes, juego.niveles[juego.nivel_actual - 1].tope_paredes, juego);
    mover_sombra_en_fila(ref_sombra,movimiento, juego.niveles[juego.nivel_actual - 1].paredes, juego.niveles[juego.nivel_actual - 1].tope_paredes);
    mover_sombra_en_columna(ref_sombra,ref_personaje, movimiento, juego.niveles[juego.nivel_actual - 1].paredes, juego.niveles[juego.nivel_actual - 1].tope_paredes);
}

/*
*  Función que recibe el juego y una coordenada. Se encarga de decir si hay o no
* un elemento que coincide con la posición pasada por parámetro .
*/
bool posicion_ocupada(juego_t juego, coordenada_t coordenada){
    bool hay_elemento = false;
    if(buscar_elemento(juego.niveles[juego.nivel_actual -1].herramientas, juego.niveles[juego.nivel_actual -1].tope_herramientas, coordenada) != NO_ENCONTRADO || buscar_elemento(juego.niveles[juego.nivel_actual -1].obstaculos, juego.niveles[juego.nivel_actual -1].tope_obstaculos, coordenada) != NO_ENCONTRADO){
        hay_elemento = true;
    }
    return hay_elemento;
}

/*
*  Función que dos coordenadas como parámetro. Se encarga de decir
* si la distancia absoluta entre ambas coordenadas es menor o igual a 1.
*/
bool rango_vela(coordenada_t vela, coordenada_t personaje){
    return abs(vela.fila-personaje.fila)<= 1 && abs(vela.col-personaje.col) <= 1;
}

/*
*  Procedimiento que recibe el juego por referencia y el tope de las herramientas por 
* por valor. Se encarga de inicializar las herramientas(asignarles un tipo y una posición)
* y paredes para todos los niveles.
*/
void inicializar_herramientas(juego_t *juego, int tope_escaleras, int tope_monedas, int tope_llaves, int tope_puertas){
    cargar_mapas();
    for (int i = 0; i < MAX_NIVELES; i++){
        obtener_mapa((*juego).niveles[i].paredes, &(*juego).niveles[i].tope_paredes);
        int j = 0;
        (*juego).niveles[i].tope_herramientas = 0;
        coordenada_t posicion_auxiliar;
        posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
        for (; j < tope_escaleras; j++){
            while(!es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar) || posicion_ocupada(*juego, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].herramientas[j].tipo = ESCALERA;
            (*juego).niveles[i].herramientas[j].coordenada = posicion_auxiliar;
            ((*juego).niveles[i].tope_herramientas)++;

        }
        for (j = tope_escaleras; j < tope_monedas + tope_escaleras; j++){
            while(posicion_ocupada(*juego, posicion_auxiliar) || es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].herramientas[j].tipo = MONEDA;
            (*juego).niveles[i].herramientas[j].coordenada = posicion_auxiliar;

            ((*juego).niveles[i].tope_herramientas)++;
        }
        for (j = tope_monedas + tope_escaleras; j < tope_llaves + tope_monedas + tope_escaleras; j++){
            while(posicion_ocupada(*juego, posicion_auxiliar)|| es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].herramientas[j].tipo = LLAVE;
            (*juego).niveles[i].herramientas[j].coordenada = posicion_auxiliar;
            ((*juego).niveles[i].tope_herramientas)++;
        }
        for (j = tope_monedas + tope_escaleras+ tope_llaves  ; j < tope_llaves + tope_monedas + tope_escaleras + tope_puertas; j++){
            while(posicion_ocupada(*juego, posicion_auxiliar) || es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].herramientas[j].tipo = PUERTA;
            (*juego).niveles[i].herramientas[j].coordenada = posicion_auxiliar;
            ((*juego).niveles[i].tope_herramientas)++;
        }
    }
}

/*
*  Procedimiento que recibe el juego por referencia y el tope de los obstáculos 
* por valor.Se encarga de inicializar los obstáculos(asignarles un tipo y una 
* posición) para todos los niveles.
*/
void inicializar_obstaculos(juego_t *juego, int tope_velas, int tope_pozos, int tope_interruptores, int tope_portales){
    for (int i = 0; i < MAX_NIVELES; i++){
        int j = 0;
        (*juego).niveles[i].tope_obstaculos = 0;
        coordenada_t posicion_auxiliar;
        posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
        for (; j < tope_velas ; j++){
            while(posicion_ocupada(*juego, posicion_auxiliar)|| es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].obstaculos[j].tipo = VELA;
            (*juego).niveles[i].obstaculos[j].coordenada = posicion_auxiliar;
            ((*juego).niveles[i].tope_obstaculos)++;
        }
        for(j =  tope_velas ; j < tope_pozos + tope_velas ; j++){
            while(posicion_ocupada(*juego, posicion_auxiliar)|| es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].obstaculos[j].tipo = POZO;
            (*juego).niveles[i].obstaculos[j].coordenada = posicion_auxiliar;
            ((*juego).niveles[i].tope_obstaculos)++;
        }
        for(j = tope_pozos + tope_velas; j < tope_interruptores+ tope_pozos+ tope_velas ; j++){
            while(posicion_ocupada(*juego, posicion_auxiliar)|| es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].obstaculos[j].tipo = INTERRUPTOR;
            (*juego).niveles[i].obstaculos[j].coordenada = posicion_auxiliar;
            ((*juego).niveles[i].tope_obstaculos)++;
        }
        for(j = tope_interruptores+ tope_pozos + tope_velas; j < tope_portales+ tope_interruptores+ tope_pozos + tope_velas ; j++){
            while(posicion_ocupada(*juego, posicion_auxiliar)|| es_pared((*juego).niveles[i].paredes, (*juego).niveles[i].tope_paredes, posicion_auxiliar)){
                posicion_auxiliar = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            }
            (*juego).niveles[i].obstaculos[j].tipo = PORTAL;
            (*juego).niveles[i].obstaculos[j].coordenada = posicion_auxiliar;
            ((*juego).niveles[i].tope_obstaculos)++;
        }
    }
}

/*
*  Procedimiento que recibe el juego por referencia y se encarga de asignarle posiciones 
* (libres) a Mac y su sombra.
*/
void inicializar_personajes(juego_t* juego){
    coordenada_t coor_aux_mac = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
    coordenada_t coor_aux_sombra;
    coor_aux_sombra.fila = coor_aux_mac.fila;
    coor_aux_sombra.col= MAX_COLUMNAS - coor_aux_mac.col - 1;
    while(es_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes,(*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, coor_aux_mac) ||es_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes,(*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, coor_aux_sombra) || posicion_ocupada(*juego, coor_aux_mac) || posicion_ocupada(*juego, coor_aux_sombra)){
        coor_aux_mac = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
        coor_aux_sombra.fila = coor_aux_mac.fila;
        coor_aux_sombra.col= MAX_COLUMNAS - coor_aux_mac.col - 1;
    }
    (*juego).personaje.posicion = coor_aux_mac;
    (*juego).personaje.posicion = coor_aux_sombra;
    inicializar_personaje(&(juego)->personaje, coor_aux_mac);
    inicializar_sombra(&(juego)->sombra, coor_aux_sombra);
}

void inicializar_juego(juego_t *juego){
    (*juego).nivel_actual = NIVEL_1;
    inicializar_herramientas(juego, tope_escaleras[(*juego).nivel_actual - 1],tope_monedas[(*juego).nivel_actual - 1], tope_llaves[(*juego).nivel_actual - 1], tope_puertas[(*juego).nivel_actual - 1]);
    inicializar_obstaculos(juego,tope_velas[(*juego).nivel_actual - 1],tope_pozos[(*juego).nivel_actual - 1], tope_interruptores[(*juego).nivel_actual - 1], tope_portales[(*juego).nivel_actual - 1]);
    inicializar_personajes(juego);
}

void inicializar_juego_config(juego_t *juego, obstaculos_t obstaculos[MAX_NIVELES], herramientas_t herramientas[MAX_NIVELES]){
    (*juego).nivel_actual = NIVEL_1;
    inicializar_herramientas(juego, herramientas[(*juego).nivel_actual - 1].escaleras,herramientas[(*juego).nivel_actual - 1].monedas, herramientas[(*juego).nivel_actual - 1].llaves, tope_puertas[(*juego).nivel_actual - 1]);
    inicializar_obstaculos(juego,obstaculos[(*juego).nivel_actual - 1].velas,obstaculos[(*juego).nivel_actual - 1].pozos, obstaculos[(*juego).nivel_actual - 1].interruptores, obstaculos[(*juego).nivel_actual - 1].portales);
   inicializar_personajes(juego);
}
/*
*   Procedimineto que dada una matriz, se encargará de llenarla.
*/
void llenar_matriz(char juego_terreno[MAX_FILAS][MAX_COLUMNAS]){
    for (int i = 0; i < MAX_FILAS; i++){
        for (int j = 0; j < MAX_COLUMNAS; j++){
            juego_terreno[i][j] = GUION;
        }
    }
}

/*
*   Procedimineto que dada una matriz, se encargará de mostrarla por pantalla con emojis.
*/
void mostrar_matriz(char juego_terreno[MAX_FILAS][MAX_COLUMNAS]){
    for(int i = 0; i < MAX_FILAS; i++){
        for(int j = 0; j < MAX_COLUMNAS; j++){
            if(juego_terreno[i][j] == MAC){
                printf("%s", EMOJI_MAC);
                }
            else if(juego_terreno[i][j] == BLOO){
                printf("%s", EMOJI_BLOO);
            }
            else if(juego_terreno[i][j] == PARED){
                printf("%s", EMOJI_PARED);
            }
            else if(juego_terreno[i][j] == ESCALERA){
                printf("%s ", EMOJI_ESCALERA);
            }
            else if(juego_terreno[i][j] == VELA){
                printf("%s ", EMOJI_VELA);
            }
            else if(juego_terreno[i][j] == MONEDA){
                printf("%s ", EMOJI_MONEDA);
            }
            else if (juego_terreno[i][j] == POZO){
                printf("%s ", EMOJI_POZO);
            }
            else if(juego_terreno[i][j] == INTERRUPTOR){
                printf("%s ", EMOJI_INTERRUPTOR);
            }
            else if (juego_terreno[i][j] == PUERTA){
                printf("%s", EMOJI_PUERTA);
            }
            else if (juego_terreno[i][j] == LLAVE){
                printf("%s", EMOJI_LLAVE);
            }
            else if (juego_terreno[i][j] == PORTAL){
                printf("%s", EMOJI_PORTAL);
            }
            else{
                printf("%s", EMOJI_TERRENO);
            }
        }
        printf("\n");
    }
    printf("\n");
}

/*
*  Procedimiento que una matriz, un vector de elementos y su respectivo 
* tope(por valor). Se encarga de asignarle el tipo de elemento a las 
* posiciones del tablero de juego. 
*/
void llenar_matriz_con_elementos(char terreno[MAX_FILAS][MAX_COLUMNAS], elemento_t elementos[MAX_ELEMENTOS], int tope){
    for(int i=0; i < tope; i++){
        terreno[elementos[i].coordenada.fila][elementos[i].coordenada.col] = elementos[i].tipo;
    }
}

void imprimir_terreno(juego_t juego){
    char juego_terreno[MAX_FILAS][MAX_COLUMNAS];
    llenar_matriz(juego_terreno);

    for(int j = 0; j < juego.niveles[juego.nivel_actual - 1].tope_paredes; j++){
        juego_terreno[juego.niveles[juego.nivel_actual - 1].paredes[j].fila][juego.niveles[juego.nivel_actual - 1].paredes[j].col] = PARED;
    }
    llenar_matriz_con_elementos(juego_terreno,juego.niveles[juego.nivel_actual - 1].obstaculos,juego.niveles[juego.nivel_actual - 1].tope_obstaculos);
    llenar_matriz_con_elementos(juego_terreno,juego.niveles[juego.nivel_actual - 1].herramientas,juego.niveles[juego.nivel_actual - 1].tope_herramientas);
    juego_terreno[juego.personaje.posicion.fila][juego.personaje.posicion.col] = MAC;
    juego_terreno[juego.sombra.posicion.fila][juego.sombra.posicion.col] = BLOO;
    mostrar_matriz(juego_terreno);
}

/*
*  Procedimiento que recibe el juego por referencia. Se encarga de sumarle
* puntos al personaje cuando éste o su sombra, atrapan una. Además de eso, las
* elimina una vez que fueron atrapadas. 
*/
void recoger_monedas(juego_t* juego){
    int posicion_moneda_mac = buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion); 
    int posicion_moneda_bloo = buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).sombra.posicion); 

    if(posicion_moneda_mac != NO_ENCONTRADO && (*juego).niveles[(*juego).nivel_actual - 1].herramientas[posicion_moneda_mac].tipo == MONEDA){
        (*juego).personaje.puntos += rand() % 10 + 10;
        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), posicion_moneda_mac);
    }
    else if(posicion_moneda_bloo != NO_ENCONTRADO && (*juego).niveles[(*juego).nivel_actual - 1].herramientas[posicion_moneda_bloo].tipo == MONEDA){
        (*juego).personaje.puntos += rand() % 10 + 10;
        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), posicion_moneda_bloo);
    }
    else{
        (*juego).personaje.puntos += 0;
    }
}

/*
*  Procedimiento que recibe el juego por referencia. Se encarga de eliminar
* las velas que va pisando el personaje en el tablero de juego.
*/
void eliminar_velas(juego_t * juego){
    int posicion_vela_mac = buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).personaje.posicion); 

    if(posicion_vela_mac != NO_ENCONTRADO && (*juego).niveles[(*juego).nivel_actual - 1].obstaculos[posicion_vela_mac].tipo == VELA){
        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, &((*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos), posicion_vela_mac);
    }
}

/*
*  Procedimiento que recibe el juego por referencia. Se encarga de restarle
* vidas al personaje cuando éste, cae en uno. 
*/
void pisar_pozo(juego_t * juego){
    int posicion_pozo_mac = buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).personaje.posicion);

    if(posicion_pozo_mac != NO_ENCONTRADO && (*juego).niveles[(*juego).nivel_actual - 1].obstaculos[posicion_pozo_mac].tipo == POZO){
        (*juego).personaje.vida -= 1;
        imprimir_terreno(*juego);
        (*juego).personaje.posicion = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
        (*juego).sombra.posicion.col = MAX_COLUMNAS- (*juego).personaje.posicion.col - 1;
        (*juego).sombra.posicion.fila = (*juego).personaje.posicion.fila;
    }
}

/*
*  Procedimiento que recibe el juego por valor. Se encarga de devolver
* la coordenada donde se encuentra la puerta en el tablero de juego. 
*/
coordenada_t posicion_puerta(juego_t juego){
    coordenada_t coordenada_puerta;
        for(int i=0; i < juego.niveles[juego.nivel_actual - 1].tope_herramientas; i++) {
            if(juego.niveles[juego.nivel_actual - 1].herramientas[i].tipo == PUERTA){
                coordenada_puerta = juego.niveles[juego.nivel_actual - 1].herramientas[i].coordenada;
            }
    }
    return coordenada_puerta;
}

/*
*  Procedimiento que recibe al personaje por referencia y un movimiento.
* Se encarga de sumarle vidas al personaje cuando éste posee más de 200
* puntos, y menos de 3 vidas. 
*/
void comprar_vida(personaje_t * personaje, char movimiento){
    if(movimiento == COMPRAR_VIDA && (*personaje).puntos >= 200 && (*personaje).vida < 3){
        (*personaje).vida++;
        (*personaje).puntos-=200;
    }
}

/*
*  Procedimiento que recibe el juego por referencia. Se encarga de
* cambiar el estado del interruptor cuando el personaje lo pisa.
*/
void pisar_interruptor(juego_t * juego){
    int posicion_interruptor = buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).personaje.posicion);

    if(posicion_interruptor != NO_ENCONTRADO && (*juego).niveles[(*juego).nivel_actual - 1].obstaculos[posicion_interruptor].tipo == INTERRUPTOR){
        (*juego).personaje.interruptor_apretado = !(*juego).personaje.interruptor_apretado;
    }
}

/*
*  Procedimiento que recibe el juego por referencia. Se encarga de
* intercambiar las posiciones de los personajes cuando la sombra lo pisa.
*/
void pisar_portal(juego_t * juego){
    int posicion_portal = buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).sombra.posicion);

    if(posicion_portal != NO_ENCONTRADO && (*juego).niveles[(*juego).nivel_actual - 1].obstaculos[posicion_portal].tipo == PORTAL){
        coordenada_t posicion_auxiliar = (*juego).personaje.posicion;
        (*juego).personaje.posicion = (*juego).sombra.posicion;
        (*juego).sombra.posicion = posicion_auxiliar;
    }
}

/*
*  Procedimiento que recibe el juego por referencia. En caso de que
* el personaje se encuentre en la posición de la llave cambia a verdadero 
* el estado de tiene_llave y se encarga de eliminarla del vector de
* herramientas.
*/
void agarrar_llave(juego_t * juego){
    int posicion_llave = buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion);

    if(posicion_llave != NO_ENCONTRADO && (*juego).niveles[(*juego).nivel_actual - 1].herramientas[posicion_llave].tipo == LLAVE){
        (*juego).personaje.tiene_llave = true;
        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), posicion_llave);
    }

}

/*
*  Procedimiento que recibe el juego por referencia. En caso de que
* la sombra se encuentre en el rango 3x3 de la vela, quedará inmóvil
* hasta que la el personaje se pare sobre ella y la reviva.
*/
void pisar_vela(juego_t * juego){
    coordenada_t coordenada_sombra = (*juego).sombra.posicion;
  
    for(int i=0; i<(*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos; i++){
        if((*juego).niveles[(*juego).nivel_actual - 1].obstaculos[i].tipo == VELA && rango_vela((*juego).niveles[(*juego).nivel_actual - 1].obstaculos[i].coordenada, coordenada_sombra) && !coordenadas_iguales(coordenada_sombra, (*juego).personaje.posicion)){
            (*juego).sombra.esta_viva = false;
        }
        if(coordenadas_iguales(coordenada_sombra, (*juego).personaje.posicion) && (*juego).sombra.esta_viva == false){
            (*juego).sombra.esta_viva = true;
            (*juego).personaje.puntos -= 50;
        }
    }
}

int estado_nivel(juego_t juego){

    if(juego.nivel_actual == NIVEL_1 && distancia_entre_coordenadas(posicion_puerta(juego), juego.personaje.posicion) <= 1 && distancia_entre_coordenadas(posicion_puerta(juego), juego.sombra.posicion) <= 1 && juego.sombra.esta_viva == true){
        return GANADO;
    }
    else if(juego.nivel_actual == NIVEL_2 && distancia_entre_coordenadas(posicion_puerta(juego), juego.personaje.posicion) <= 1 && distancia_entre_coordenadas(posicion_puerta(juego), juego.sombra.posicion) <= 1 && juego.sombra.esta_viva == true && juego.personaje.tiene_llave == true){
        return GANADO;
    }
    else if(juego.nivel_actual == NIVEL_3 && distancia_entre_coordenadas(posicion_puerta(juego), juego.personaje.posicion) <= 1 && distancia_entre_coordenadas(posicion_puerta(juego), juego.sombra.posicion) <= 1 && juego.sombra.esta_viva == true && juego.personaje.tiene_llave == true){
        return GANADO;
    }
    else{
        return JUGANDO;
    } 
}

int estado_juego(juego_t juego){
    int estado = PERDIDO;

    if(juego.personaje.vida ==  0){
        estado = PERDIDO;
    }
    else if(juego.nivel_actual == NIVEL_3 && juego.personaje.vida > 0 && juego.sombra.esta_viva == true){
        estado = GANADO;
    }
    else{
        estado = JUGANDO;
    }
    return estado;
}

/*
*  Procedimiento que recibe el juego por referencia. En caso de el nivel estar
* ganado y no ser mayor a el nivel máximo, inicializa y carga lo correspondiente
* al nuevo nivel.
*/
void pasar_nivel(juego_t* juego){
    if(estado_nivel((*juego)) == GANADO && (*juego).nivel_actual <= NIVEL_3) {
        (*juego).nivel_actual++;
        inicializar_personajes(juego);
        (*juego).personaje.tiene_llave = false;
        (*juego).personaje.interruptor_apretado = false;
        printf("PASASTE AL NIVEL %i\n", (juego)->nivel_actual);
    }
}

void realizar_jugada(juego_t* juego){
    char movimiento;
    pedir_movimiento(&movimiento);
    mover_personaje(&(*juego).personaje, &(*juego).sombra, movimiento, *juego);
    recoger_monedas(juego);
    eliminar_velas(juego);
    pisar_pozo(juego);
    pisar_vela(juego);
    comprar_vida(&(*juego).personaje, movimiento);
    agarrar_llave(juego);
    pisar_portal(juego);
    pisar_interruptor(juego);
    pasar_nivel(juego);
}