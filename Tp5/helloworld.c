#include <stdio.h>     // Librería estándar para entrada/salida (printf)
#include <mpi.h>       // Librería principal de MPI
#include <unistd.h>    // Necesaria para la función gethostname()

int main(int argc, char *argv[]) {
    int rank, size;               // rank: identificador único de cada proceso
                                  // size: número total de procesos
    char hostname[256];           // Almacena el nombre del host donde corre el proceso

    MPI_Init(&argc, &argv);       // Inicializa el entorno de MPI.

    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtiene el número total de procesos (tamaño del "comunicador" global)
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtiene el "rango" (identificador) de este proceso dentro del comunicador

    gethostname(hostname, sizeof(hostname)); // Obtiene el nombre del host donde se ejecuta este proceso

    // Imprime un mensaje indicando qué proceso está hablando, cuántos hay en total, y en qué máquina se ejecuta
    printf("Hola desde el nodo %d de %d en la máquina %s\n", rank, size, hostname);

    MPI_Finalize(); // Finaliza el entorno MPI. ¡Siempre debe llamarse al final!

    return 0;       // Fin del programa
}

