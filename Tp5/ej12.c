/*
    Asignatura: Programación Paralela y Distribuida
    Integrantes:
        - Olivares, Agustín
        - Ramirez, Victor
        - Ruiz, Joaquin
    Fecha: Abril de 2025
    Comando compilación: mpicc ej12.c -o ej12
    Comando ejecución: mpirun -np 4 ./ej12 num (entre 0 y 99)

*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <time.h> 

#define ROOT 0
#define N 100

void leer(int v[N][N], int x){
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    int num_filas = N / size;
    int recv_data[num_filas][N];
    int gathered_count[size];

    MPI_Scatter(v, num_filas * N, MPI_INT, recv_data, num_filas * N, MPI_INT, ROOT, MPI_COMM_WORLD);

    int count = 0;
    for (int i = 0; i < num_filas; i++) {
        for (int j = 0; j < N; j++) {
            if (recv_data[i][j] == x)
                count++;
        }
    }

    MPI_Gather(&count, 1, MPI_INT, gathered_count, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        int total = 0;
        for (int i = 0; i < size; i++)
            total += gathered_count[i];
        printf("Total ocurrencias de %d: %d\n", x, total);
    }
}


int main(int argc, char *argv[]) {
    int rank, x;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    if (argc == 2) {
        x = atoi(argv[1]);
    }

    int data[N][N];

    if (rank == ROOT) {
        srand(time(NULL));
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                data[i][j] = rand() % 100;
    }

    // Preguntar sobre la llamada del proceso 0
    leer(data, x);

    MPI_Finalize(); 
    return 0;
}
