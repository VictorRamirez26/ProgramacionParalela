/*
    Asignatura: Programación Paralela y Distribuida
    Integrantes:
        - Olivares, Agustín
        - Ramirez, Victor
        - Ruiz, Joaquin
    Fecha: Abril de 2025
    Comando compilación: mpicc pingpong.c -o pingpong
    Comando ejecución: mpirun -np 2 ./pingpong

*/

#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int rank, size;
    char mensaje[100];  // Reservamos espacio suficiente para recibir el mensaje

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtiene el número total de nodos (tamaño del "comunicador" global)
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtiene el "rango" (identificador) de este nodo dentro del comunicador

    if(rank == 0) {
        strcpy(mensaje, "Ping");  // Asignamos el mensaje a la variable
        MPI_Send(mensaje, strlen(mensaje) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD); // Enviamos el mensaje
        printf("Proceso 0 envió '%s' a proceso 1\n", mensaje);
        MPI_Recv(mensaje, 100, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proceso 0 recibió '%s' de proceso 1\n", mensaje);
    } else if(rank == 1) {
        MPI_Recv(mensaje, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibimos el mensaje en el buffer
        printf("Proceso 1 recibió '%s' de proceso 0\n", mensaje);
        MPI_Send(mensaje, strlen(mensaje) + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        printf("Proceso 1 envió '%s' a proceso 0\n", mensaje);
    }

    MPI_Finalize();

    return 0;     
}
