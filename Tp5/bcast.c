#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>

void bcast(int source, char *msg, int tam){
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == source){
        for(int i = 0; i < size; i++){
            if(i != source){
                MPI_Send(msg, 1, MPI_CHAR, i, source, MPI_COMM_WORLD);
            }
        }
    }else{
        MPI_Recv(msg, 1, MPI_CHAR, source, source, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("%s desde nodo %d\n", msg, rank);
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    char mensaje[100];  // Reservamos espacio suficiente para recibir el mensaje

    MPI_Init(&argc, &argv);

    strcpy(mensaje, "Hola a todos desde proceso 0");

    bcast(rank, mensaje, sizeof(mensaje));    

    MPI_Finalize();

    return 0;     
}
