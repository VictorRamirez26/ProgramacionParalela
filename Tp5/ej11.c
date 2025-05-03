#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <time.h> 

#define ROOT 0

void func_par(double v[], int n , int rank, int np){
    double max;
    int max_local_index = 0;

    int particion = n / np;
    double recv_data[particion];
    double gathered_max[np];
    int gathered_index[np];

    MPI_Scatter(v, particion, MPI_DOUBLE, recv_data, particion, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    // Busco el máximo y su índice
    max = recv_data[0];
    for (int i = 1; i < particion; i++) {
        if (recv_data[i] > max) {
            max = recv_data[i];
            max_local_index = i;
        }
    }

    // Recolectar máximos e índices locales
    MPI_Gather(&max, 1, MPI_DOUBLE, gathered_max, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    MPI_Gather(&max_local_index, 1, MPI_INT, gathered_index, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        double max_global = gathered_max[0];
        int proceso_max = 0;
        // Busco el máximo global y el proceso que contiene al máximo
        for (int i = 1; i < np; i++) {
            if (gathered_max[i] > max_global) {
                max_global = gathered_max[i];
                proceso_max = i;
            }
        }

        int indice_global = proceso_max * particion + gathered_index[proceso_max];

        printf("Máximos locales:\n");
        for (int i = 0; i < np; i++) {
            printf("Proceso %d: max = %.1f, índice local = %d\n", i, gathered_max[i], gathered_index[i]);
        }

        printf("\nMáximo global: %.1f\n", max_global);
        printf("Índice global del máximo: %d\n", indice_global);
    }
}


int main(int argc, char *argv[]) {
    int rank, size, N;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc == 2) {
        N = atoi(argv[1]);
    }

    double data[N];
    if (rank == ROOT) {
        srand(time(NULL));

        for (int i = 0; i < N; i++) {
            data[i] = ((double)rand() / RAND_MAX) * 100.0; // entre 0.0 y 100.0
            printf("%.2f | ", data[i]);
        }
        printf("\n");
    }

    func_par(data , N , rank , size);

    MPI_Finalize(); 
    return 0;
}