#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <time.h> 
#define N 10
#define fila 2
#define columna 3

void printMatriz(int matriz[N][N]){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (j == N-1){
                printf("%d |\n",matriz[i][j]);
            }else{
                printf("%d | ",matriz[i][j]);
            }
        }
    }
}
int main(int argc, char *argv[]) {
    int rank , size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_rank(MPI_COMM_WORLD, &size); 

    int matriz[N][N];
    if (rank == 0){
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                matriz[i][j] = i * j;
            }
        }
        printMatriz(matriz);

        MPI_Send(matriz[fila] , N , MPI_INT, 1 , 0, MPI_COMM_WORLD );
        
        MPI_Datatype columna_type;
        MPI_Type_vector(N, 1, N, MPI_INT, &columna_type); 
        MPI_Type_commit(&columna_type);
        MPI_Send(&matriz[0][columna], 1, columna_type, 1, 0, MPI_COMM_WORLD);


    }else{
        int fila_recibida[N];
        int columna_recibida[N];
        MPI_Recv(fila_recibida , N , MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Se recibio la fila %d \n",fila);
        for (int i = 0; i < N; i++){
            printf("%d |",fila_recibida[i]);
        }
        printf("\n");
        
        MPI_Recv(columna_recibida, N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Se recibio la columna %d \n",columna);
        for (int i = 0; i < N; i++){
            printf("%d |",columna_recibida[i]);
        }
        printf("\n");
    }

    MPI_Finalize(); 
    return 0;
}
