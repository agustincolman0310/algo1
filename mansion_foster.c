#include <stdio.h>
#include "la_sombra_de_mac.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#define MAX_CARACTERES 100
#define MAX_NOMBRE 100
#define MAX_PARTIDAS 100
#define LEER "r"
#define ESCRIBIR "w"
#define MENSAJE_DE_ERROR "Hubo un error"
#define ARCHIVO_AUX "partidas_aux.csv"
#define ARCHIVO_CONFIG "config.txt"
#define F_ESCRITURA "%s;%i;%i;%i;%s\n"
#define F_LECTURA "%[^;];%i;%i;%i;%[^\n]\n"
#define F_LECTURA_CONFIG "%[^\n]\n"
#define GANO "Si"
#define PERDIO "No"
const int ERROR = -1;
const int CANTIDAD_LEIDOS = 5;
const int CANTIDAD_LEIDOS_CONFIG = 1;
const int NIVEL_INICIAL= 0;
const int NIVEL_INTERMEDIO = 1;
const int NIVEL_FINAL = 2;


typedef struct partida{
    char jugador[MAX_NOMBRE];
    int nivel_llegado;
    int puntos;
    int vidas_restantes;
    bool gano;
} partida_t;

/*
*  Procedimiento que recibe el juego por parámetro. Se encarga de hacer
* que el imprimir el mostrar el terreno actual a medida que el usuario
* realiza las diferentes jugadas.
*/
void jugar(juego_t juego){
    imprimir_terreno(juego);
    while (estado_juego(juego) != 1 && estado_juego(juego) != -1)
    {
        printf("Mac tiene %i vidas\n", juego.personaje.vida);
        printf("Mac tiene %i puntos\n", juego.personaje.puntos);
        realizar_jugada(&juego);
        imprimir_terreno(juego);
    }
}

/*
*  Procedimiento que recibe dos archivos: uno para leer y otro para
* escribir. Se encarga de insertar alfabeticamente(por nombre) los
* datos de una nueva partida.
*/
void agregar_partida(FILE *f_partidas, FILE *f_partidas_aux){
    partida_t nueva_partida;
    char ganado[MAX_CARACTERES];
    printf("Ingrese su nombre:\n");
    scanf(" %s", nueva_partida.jugador);
    printf("Ingrese el número de nivel hasta el que llegó:\n");
    scanf("%i", &(nueva_partida.nivel_llegado));
    printf("Ingrese el número de puntos obtenidos:\n");
    scanf("%i", &(nueva_partida.puntos));
    printf("Ingrese el número de vidas restantes:\n");
    scanf("%i", &(nueva_partida.vidas_restantes));
    printf("¿Logró ganar el juego?\n");
    scanf("%s", ganado);
    nueva_partida.gano = strcmp(ganado, PERDIO);

    bool insertado = false;
    partida_t partidas;
    int leidos = fscanf(f_partidas, F_LECTURA, partidas.jugador, &(partidas.nivel_llegado), &(partidas.puntos), &(partidas.vidas_restantes), ganado);

    while (leidos == CANTIDAD_LEIDOS){
        if (strcmp(partidas.jugador, nueva_partida.jugador) > 0 && !insertado){
            if (nueva_partida.gano == true){
                fprintf(f_partidas_aux, F_ESCRITURA, nueva_partida.jugador, nueva_partida.nivel_llegado, nueva_partida.puntos, nueva_partida.vidas_restantes, GANO);
                insertado = true;
            }
            if (nueva_partida.gano == false){
                fprintf(f_partidas_aux, F_ESCRITURA, nueva_partida.jugador, nueva_partida.nivel_llegado, nueva_partida.puntos, nueva_partida.vidas_restantes, PERDIO);
                insertado = true;
            }
        }
        fprintf(f_partidas_aux, F_ESCRITURA, partidas.jugador, partidas.nivel_llegado, partidas.puntos, partidas.vidas_restantes, ganado);
        leidos = fscanf(f_partidas, F_LECTURA, partidas.jugador, &(partidas.nivel_llegado), &(partidas.puntos), &(partidas.vidas_restantes), ganado);
    }
    if (!insertado){
        if (nueva_partida.gano == true){
            fprintf(f_partidas_aux, F_ESCRITURA, nueva_partida.jugador, nueva_partida.nivel_llegado, nueva_partida.puntos, nueva_partida.vidas_restantes, GANO);
            insertado = true;
        }
        else if (nueva_partida.gano == false){
            fprintf(f_partidas_aux, F_ESCRITURA, nueva_partida.jugador, nueva_partida.nivel_llegado, nueva_partida.puntos, nueva_partida.vidas_restantes, PERDIO);
            insertado = true;
        }
    }
}

/*
*  Procedimiento que recibe dos archivos(uno para leer y otro para
* escribir) y el nombre de un usuario. Se encarga de eliminar la partida
* del primer primer usuario que coincide con el nombre pasado por parámetro.
*/
void eliminar_partida(FILE *f_partidas, FILE *f_partidas_aux, char jugador[MAX_NOMBRE]){
    bool eliminado = false;
    partida_t partidas;
    char ganado[MAX_CARACTERES];
    int leidos = fscanf(f_partidas, F_LECTURA, partidas.jugador, &(partidas.nivel_llegado), &(partidas.puntos), &(partidas.vidas_restantes), ganado);

    while (leidos == CANTIDAD_LEIDOS){
        if (strcmp(jugador, partidas.jugador) != 0 || (strcmp(jugador, partidas.jugador) == 0 && eliminado)){
            fprintf(f_partidas_aux, F_ESCRITURA, partidas.jugador, partidas.nivel_llegado, partidas.puntos, partidas.vidas_restantes, ganado);
        }
        else{
            eliminado = true;
        }
        leidos = fscanf(f_partidas, F_LECTURA, partidas.jugador, &(partidas.nivel_llegado), &(partidas.puntos), &(partidas.vidas_restantes), ganado);
    }
}

/*
*  Procedimiento que recibe dos archivos(uno para leer y otro para
* escribir). Se encarga de ordenar las partidas que hay en el archivo que lee
* e imprimirlo ordenadamente en el archivo que escribe.
*/
void ordenar_partidas(FILE *f_partidas, FILE *f_partidas_aux){
    partida_t partidas[MAX_PARTIDAS];
    char ganado[MAX_CARACTERES];
    int tope = 0;
    int leidos = fscanf(f_partidas, F_LECTURA, partidas[tope].jugador, &(partidas[tope].nivel_llegado), &(partidas[tope].puntos), &(partidas[tope].vidas_restantes), ganado);

    while (leidos == CANTIDAD_LEIDOS){
        partidas[tope].gano = strcmp(ganado, PERDIO);
        tope++;
        leidos = fscanf(f_partidas, F_LECTURA, partidas[tope].jugador, &(partidas[tope].nivel_llegado), &(partidas[tope].puntos), &(partidas[tope].vidas_restantes), ganado);
    }

    partida_t partida_aux;
    for (int i = 0; i < tope; i++){
        for (int j = 0; j < tope - 1; j++){
            if (strcmp(partidas[j].jugador, partidas[j + 1].jugador) > 0){
                partida_aux = partidas[j];
                partidas[j] = partidas[j + 1];
                partidas[j + 1] = partida_aux;
            }
        }
    }

    for (int i = 0; i < tope; i++){
        if (partidas[i].gano){
            fprintf(f_partidas_aux, F_ESCRITURA, partidas[i].jugador, partidas[i].nivel_llegado, partidas[i].puntos, partidas[i].vidas_restantes, GANO);
        }
        else{
            fprintf(f_partidas_aux, F_ESCRITURA, partidas[i].jugador, partidas[i].nivel_llegado, partidas[i].puntos, partidas[i].vidas_restantes, PERDIO);
        }
    }
}

/*
*  Procedimiento que recibe un vector del tipo obstaculos_t y otro del tipo herramientas_t.
* En caso de encontrar un archivo de configuración, inicializa el juego con los datos
* de dicho archivo. Caso contrario, el juego se carga con sus valores normales.
*/
void configurar_juego(obstaculos_t obstaculos[MAX_NIVELES], herramientas_t herramientas[MAX_NIVELES]){
    FILE* f_config = fopen(ARCHIVO_CONFIG, LEER);
    if(!f_config){
        perror(MENSAJE_DE_ERROR);
        juego_t juego;
        inicializar_juego(&juego);
        jugar(juego);
        return;
    }
    char linea[MAX_CARACTERES];
    int leidos = fscanf(f_config, F_LECTURA_CONFIG, linea);
    char* etiqueta;

    while (leidos == CANTIDAD_LEIDOS_CONFIG){
        etiqueta = strtok(linea, "=");
        if(strcmp(etiqueta, "N1_POZOS") == 0){
            obstaculos[NIVEL_INICIAL].pozos = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N2_POZOS") == NIVEL_INICIAL){
            obstaculos[NIVEL_INTERMEDIO].pozos = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N3_POZOS") == 0){
            obstaculos[NIVEL_FINAL].pozos = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N1_VELAS") == 0){
            obstaculos[NIVEL_INICIAL].velas = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N2_VELAS") == 0){
            obstaculos[NIVEL_INTERMEDIO].velas = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N3_VELAS") == 0){
            obstaculos[NIVEL_FINAL].velas = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N1_INTERRUPTORES") == 0){
            obstaculos[NIVEL_INICIAL].interruptores= atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N2_INTERRUPTORES") == 0){
            obstaculos[NIVEL_INTERMEDIO].interruptores = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N3_INTERRUPTORES") == 0){
            obstaculos[NIVEL_FINAL].interruptores = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N1_PORTALES") == 0){
            obstaculos[NIVEL_INICIAL].portales = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N2_PORTALES") == 0){
            obstaculos[NIVEL_INTERMEDIO].portales = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N3_PORTALES") == 0){
            obstaculos[NIVEL_FINAL].portales = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N1_MONEDAS") == 0){
            herramientas[NIVEL_INICIAL].monedas = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N2_MONEDAS") == 0){
            herramientas[NIVEL_INTERMEDIO].monedas = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N3_MONEDAS") == 0){
            herramientas[NIVEL_FINAL].monedas = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N1_ESCALERAS") == 0){
            herramientas[NIVEL_INICIAL].escaleras = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N2_ESCALERAS") == 0){
            herramientas[NIVEL_INTERMEDIO].escaleras = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N3_ESCALERAS") == 0){
            herramientas[NIVEL_FINAL].escaleras = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N1_LLAVES") == 0){
            herramientas[NIVEL_INICIAL].llaves = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N2_LLAVES") == 0){
            herramientas[NIVEL_INTERMEDIO].llaves = atoi(strtok(NULL, "\n"));
        }else if(strcmp(etiqueta, "N3_LLAVES") == 0){
            herramientas[NIVEL_FINAL].llaves = atoi(strtok(NULL, "\n"));
        }
        leidos = fscanf(f_config, F_LECTURA_CONFIG, linea);
    }
    juego_t juego;
    inicializar_juego_config(&juego, obstaculos, herramientas);
    jugar(juego);
    fclose(f_config);
}

int main(int argc, char* argv[]){

    srand((unsigned)time(NULL));
    if(strcmp(argv[1], "config_juego") == 0){
        obstaculos_t obstaculos[MAX_NIVELES];
        herramientas_t herramientas[MAX_NIVELES];
        configurar_juego(obstaculos, herramientas);
    }
    FILE *f_partidas = fopen(argv[2], LEER);
    if (!f_partidas){
        perror(MENSAJE_DE_ERROR);
        return ERROR;
    }
    FILE *f_partidas_aux = fopen(ARCHIVO_AUX, ESCRIBIR);
    if (!f_partidas_aux){
        fclose(f_partidas);
        perror(MENSAJE_DE_ERROR);
        return ERROR;
    }

    if (strcmp(argv[1], "agregar_partida") == 0){
        agregar_partida(f_partidas, f_partidas_aux);
    }
    if (strcmp(argv[1], "eliminar_partida") == 0){
        eliminar_partida(f_partidas, f_partidas_aux, argv[3]);
    }
    if (strcmp(argv[1], "ordenar_partidas") == 0){
        ordenar_partidas(f_partidas, f_partidas_aux);
    }
    fclose(f_partidas);
    fclose(f_partidas_aux);
    rename(ARCHIVO_AUX, argv[2]);
    return 0;
}
