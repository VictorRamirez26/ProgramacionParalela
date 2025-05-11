/*
    Asignatura: Programación Paralela y Distribuida
    Integrantes:
        - Olivares, Agustín
        - Ramirez, Victor
        - Ruiz, Joaquin
    Fecha: Abril de 2025
    Comando compilación: mpicc bingo.c -o bingo
    Comando ejecución: mpirun -np 4 ./bingo

*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <stdbool.h>

struct Carton {
    int matriz[3][9];
    int cantidadTachados;
};

struct BolsaNumeros {
    int listaNumeros[91];
};

struct Premios {
    bool ambo;
    bool terna;
    bool cuaterna;
    bool quintina;
    bool cartonLleno;
};

struct Carton formatearCarton(struct Carton carton) {
    int filas = 3;
    int columnas_invalidas = 4;
    srand(time(NULL));

    for (int i = 0; i < filas; i++) {
        while (columnas_invalidas > 0) {
            int columnaRandom = rand() % 9;
            if (carton.matriz[i][columnaRandom] != -1) {
                carton.matriz[i][columnaRandom] = -1;
                columnas_invalidas--;
            }
        }
        columnas_invalidas = 4;
    }
    return carton;
}

void imprimirCarton(struct Carton carton){

    for (int i = 0 ; i < 3 ; i++){
        for (int j = 0; j < 9; j++){
            printf("%d | " , carton.matriz[i][j]);
        }
        printf("\n");
    }
}

struct Carton llenarCarton(int rank) {
    struct Carton carton;
    srand(time(NULL) + rank);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 9; j++) {
            carton.matriz[i][j] = rand() % 91;
        }
    }
    carton.cantidadTachados = 0;
    return formatearCarton(carton);
}

void iniciarNumeros(struct BolsaNumeros *bolsaNumeros) {
    for (int i = 0; i < 91; i++) {
        bolsaNumeros->listaNumeros[i] = i;
    }
}

int sacarNumero(struct BolsaNumeros *bolsaNumeros) {
    int posicion;
    do {
        posicion = rand() % 91;
    } while (bolsaNumeros->listaNumeros[posicion] == 0);
    bolsaNumeros->listaNumeros[posicion] = 0;
    return posicion;
}

struct Carton tacharNumero(struct Carton carton, int numero) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 9; j++) {
            if (carton.matriz[i][j] == numero) {
                carton.matriz[i][j] = -2;
                carton.cantidadTachados++;
            }
        }
    }
    return carton;
}

void verificarPremios(int count, char mensaje[], struct Premios *premios) {
    if (count == 2 && premios->ambo) {
        strcpy(mensaje, "Ambo");
    } else if (count == 3 && premios->terna) {
        strcpy(mensaje, "Terna");
    } else if (count == 4 && premios->cuaterna) {
        strcpy(mensaje, "Cuaterna");
    } else if (count == 5 && premios->quintina) {
        strcpy(mensaje, "Quintina");
    } else {
        strcpy(mensaje, "Nada");
    }
}

void verificarCarton(struct Carton carton, char mensaje[], struct Premios *premios) {
    strcpy(mensaje, "Nada");
    if (carton.cantidadTachados == 15 && premios->cartonLleno) {
        strcpy(mensaje, "Carton lleno");
        return;
    }
    for (int i = 0; i < 3; i++) {
        int count = 0;
        for (int j = 0; j < 9; j++) {
            if (carton.matriz[i][j] == -2) count++;
        }
        verificarPremios(count, mensaje, premios);
        if (strcmp(mensaje, "Nada") != 0) return;
    }
}

void actualizarPremioGlobal(struct Premios *premios, char mensaje[]) {
    if (strcmp(mensaje, "Ambo") == 0) premios->ambo = false;
    else if (strcmp(mensaje, "Terna") == 0) premios->terna = false;
    else if (strcmp(mensaje, "Cuaterna") == 0) premios->cuaterna = false;
    else if (strcmp(mensaje, "Quintina") == 0) premios->quintina = false;
    else if (strcmp(mensaje, "Carton lleno") == 0) premios->cartonLleno = false;
}

int main(int argc, char *argv[]) {
    int rank, size;
    struct BolsaNumeros bolsa;
    struct Premios premios = {true, true, true, true,true};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Total de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Identificador de proceso

    int numero;
    char mensaje[100];

    if (rank == 0) {
        iniciarNumeros(&bolsa);
        for (int i = 0; i < 90; i++) { // Cantidad que saco
            numero = sacarNumero(&bolsa);
            printf("Se enviará el número %d\n", numero);
            for (int j = 1; j < size; j++) {
                MPI_Send(&numero, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
            }

            MPI_Status status;
            int premioOtorgado = 0;
            char recibido[100];
            for (int j = 1; j < size; j++) {
                MPI_Recv(recibido, 100, MPI_CHAR, j, 1, MPI_COMM_WORLD, &status);
                if (strcmp(recibido, "Nada") != 0 && premioOtorgado == 0) {
                    printf("Jugador %d ganó el premio: %s\n", status.MPI_SOURCE, recibido);
                    actualizarPremioGlobal(&premios, recibido);
                    premioOtorgado = 1;
                    
                }
            }
            
            // Broadcast para actualizar estado global de premios y sincronizo los procesos
            MPI_Bcast(&premios, sizeof(struct Premios), MPI_BYTE, 0, MPI_COMM_WORLD);
            if (!premios.cartonLleno){
                break;
            }
        }
    } else {
        struct Carton carton = llenarCarton(rank);
        for (int i = 0; i < 90; i++) {
            MPI_Recv(&numero, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            carton = tacharNumero(carton, numero);
            verificarCarton(carton, mensaje, &premios);

            // Aviso al master el resultado
            MPI_Send(mensaje, 100, MPI_CHAR, 0, 1, MPI_COMM_WORLD);

            // Esperar actualización del estado de los premios
            MPI_Bcast(&premios, sizeof(struct Premios), MPI_BYTE, 0, MPI_COMM_WORLD);
            if (!premios.cartonLleno){
                break;
            }
        }
        //imprimirCarton(carton);
        printf("Jugador %d, cantidad de tachados: %d\n", rank, carton.cantidadTachados);
    }

    MPI_Finalize();
    return 0;
}
