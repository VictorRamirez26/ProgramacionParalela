/*
    Asignatura: Programación Paralela y Distribuida
    Integrantes:
        - Olivares, Agustín
        - Ramirez, Victor
        - Ruiz, Joaquin
    Fecha: Abril de 2025
    Comando compilación: mpicc ej10.c -o ej10
    Comando ejecución: mpirun -np 4 ./bcast 0 3

*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#define N 5

void intercambiar(double x[N] , int proc1 , int proc2) {
    double buffer[N];
    MPI_Send(x,N, MPI_DOUBLE,proc2,0,MPI_COMM_WORLD);
    MPI_Recv(buffer,N,MPI_DOUBLE,proc2,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    memcpy(x, buffer, N * sizeof(double));
}
void printVector(double x[N], int rank){
    printf("Proceso %d tiene x = [", rank);
    for (int i = 0; i < N; i++) printf("%.1f ", x[i]);
    printf("]\n");
}
int main(int argc, char *argv[]) {

    // Parametros para ejecutar: mpirun -np 4 ej10 1 3
    int rank,proc1,proc2, size;
    double x[N];    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    if (argc == 3) {
        proc1 = atoi(argv[1]);
        proc2 = atoi(argv[2]);
    }

    // Inicialización del vector
    for (int i = 0; i < N; i++) {
        if (rank == proc1)
            x[i] = 1.0;
        else if (rank == proc2)
            x[i] = 2.0;
        else
            x[i] = 0.0;
    }

    if (rank == proc1){
        printVector(x,proc1);
        intercambiar(x , proc1 , proc2);
        printVector(x,proc1);
    }else if(rank == proc2){
        printVector(x,proc2);
        intercambiar(x,proc2,proc1);
        printVector(x,proc2);
    }
    
    MPI_Finalize();
    return 0;
}
