#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define NUM_USUARIOS 10
#define CAPACIDAD_COLA 4
#define NUM_IMPRESORAS 2

typedef struct {
    int cola[CAPACIDAD_COLA];
    int inicio;
    int fin;
    int archivos_impresos;

    sem_t sem_ocupada;  // Espacios disponibles
    sem_t sem_vacia;    // Archivos en cola
    sem_t mutex;        // Mutex general
} ColaDeImpresion;

void usuario_proceso(int id, ColaDeImpresion* cola) {
    sem_wait(&cola->sem_ocupada);
    sem_wait(&cola->mutex);

    cola->cola[cola->fin] = id;
    cola->fin = (cola->fin + 1) % CAPACIDAD_COLA;
    printf("Usuario %d agregó archivo a la cola de impresión\n", id);

    sem_post(&cola->mutex);
    sem_post(&cola->sem_vacia);
    exit(0);  // Termina el proceso
}

void impresora_proceso(int id, ColaDeImpresion* cola) {
    while (1) {
        sem_wait(&cola->sem_vacia);
        sem_wait(&cola->mutex);

        if (cola->archivos_impresos >= NUM_USUARIOS) {
            sem_post(&cola->mutex);
            sem_post(&cola->sem_vacia); 
            break;
        }

        int archivo = cola->cola[cola->inicio];
        cola->inicio = (cola->inicio + 1) % CAPACIDAD_COLA;
        cola->archivos_impresos++;

        printf("Impresora %d: Imprimiendo archivo de usuario %d\n", id, archivo);

        sem_post(&cola->mutex);
        sem_post(&cola->sem_ocupada);
        sleep(1);  // Simula impresión
        if (cola->archivos_impresos >= NUM_USUARIOS){
            break;
        }
        
    }
    exit(0);
}

int main() {

    // Creo la memoria compartida
    int shmid = shmget(IPC_PRIVATE, sizeof(ColaDeImpresion), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    ColaDeImpresion* cola = (ColaDeImpresion*)shmat(shmid, NULL, 0);
    if (cola == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    // Inicialización de la cola
    cola->inicio = 0;
    cola->fin = 0;
    cola->archivos_impresos = 0;
    // Semáforos
    sem_init(&cola->sem_ocupada, 1, CAPACIDAD_COLA);
    sem_init(&cola->sem_vacia, 1, 0);
    sem_init(&cola->mutex, 1, 1);

    // Creo los procesos para los usuarios
    for (int i = 0; i < NUM_USUARIOS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            usuario_proceso(i + 1, cola);
        }
    }

    // Creo los procesos para las impresoras
    for (int i = 0; i < NUM_IMPRESORAS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            impresora_proceso(i + 1, cola);
        }
    }

    // Espero a que terminen los procesos hijos
    for (int i = 0; i < NUM_USUARIOS + NUM_IMPRESORAS; i++) {
        wait(NULL);
    }

    // Liberar recursos
    sem_destroy(&cola->sem_ocupada);
    sem_destroy(&cola->sem_vacia);
    sem_destroy(&cola->mutex);
    shmdt(cola);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
