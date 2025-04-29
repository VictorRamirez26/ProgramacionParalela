#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>

int T1(int i){
    sleep(1);
    return i;
}

int T2(int i){
    sleep(2);
    return i;
}

int T3(int i, int j){
    sleep(1);
    return i;
}

int T4(int i){
    sleep(1);
    return i;
}

int T5(int i){
    sleep(2);
    return i;
}


int main(int argc, char *argv[]) {
    int rank, size, a, b, c, d, e, i, j;
    char mensaje[100];  // Reservamos espacio suficiente para recibir el mensaje

    i = 1;
    j = 1;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0){
        a = T1(i);
        MPI_Send(&a, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&c, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        d = T4(a + c);
        MPI_Send(&d, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else{
        b = T2(j);
        MPI_Recv(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        c = T3(a + b, i);
        MPI_Send(&c, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        e = T5(b + c);
        MPI_Recv(&d, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int f = e + d;

        printf("%d\n", f);
        
    }     

    MPI_Finalize();

    return 0;
}
