#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int siguiente = (rank + 1) % size;
    int anterior = (rank - 1 + size) % size;
    int numero_vueltas = 500;
    double start_time , end_time ;

    for (int i = 0 ; i < numero_vueltas ; i++){

        if (rank == 0) {

            if (i==0) start_time = MPI_Wtime();

            MPI_Send(NULL, 0, MPI_CHAR, siguiente, 0, MPI_COMM_WORLD);
            MPI_Recv(NULL, 0, MPI_CHAR, anterior, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (i==numero_vueltas-1) end_time = MPI_Wtime();
        } else {
            MPI_Recv(NULL, 0, MPI_CHAR, anterior, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(NULL, 0, MPI_CHAR, siguiente, 0, MPI_COMM_WORLD);
        }

    }

    if (rank == 0) {
        double total_time = end_time - start_time;
        double tiempo_promedio = total_time / numero_vueltas;
        printf("Numero de vueltas: %d \n", numero_vueltas);
        printf("Tiempo total: %f segundos\n", total_time);
        printf("Tiempo promedio por vuelta: %f segundos\n", tiempo_promedio);
    }

    MPI_Finalize();
    return 0;
}
