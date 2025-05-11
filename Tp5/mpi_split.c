/*
    Asignatura: Programación Paralela y Distribuida
    Integrantes:
        - Olivares, Agustín
        - Ramirez, Victor
        - Ruiz, Joaquin
    Fecha: Abril de 2025
    Comando compilación: mpicc mpi_split.c -o mpi_split
    Comando ejecución: mpirun -np 16 ./mpi_split

*/

#include <stdio.h>     // Librería estándar para entrada/salida (printf)
#include <mpi.h>       // Librería principal de MPI

int main(int argc, char *argv[]) {
    int rank, size;               // rank: identificador único de cada proceso
                                  // size: número total de procesos

    MPI_Comm new_comm;
    int new_rank;
    int color;

    MPI_Init(&argc, &argv);       // Inicializa el entorno de MPI.

    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtiene el número total de nodo (tamaño del "comunicador" global)
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtiene el "rango" (identificador) de este nodo dentro del comunicador

    printf("Ejercicio a: ");

    color = (rank / 4) % 2;

    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &new_comm);
    MPI_Comm_rank(new_comm, &new_rank);

    printf("Proceso %d (rango global en MPI_COMM_WORLD) está en el grupo %d con rango %d (dentro del grupo)\n", rank, color, new_rank);

    MPI_Comm_free(&new_comm);

    printf("Ejercicio b: ");

    color = rank % 2;

    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &new_comm);
    MPI_Comm_rank(new_comm, &new_rank);

    printf("Proceso %d (rango global en MPI_COMM_WORLD) está en el grupo %d con rango %d (dentro del grupo)\n", rank, color, new_rank);

    MPI_Comm_free(&new_comm);

    MPI_Finalize(); 

    return 0;       // Fin del programa
}

