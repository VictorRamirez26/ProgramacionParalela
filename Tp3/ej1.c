#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_THREADS 10

int cola_impresion[1] = {-1};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Declarar e inicializar el mutex
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  


bool cola_vacia(){
    return cola_impresion[0] == -1;
}

void* enqueue(void *arg){

    int *num = (int*)arg;
    bool flag = false;
    while(true){
        pthread_mutex_lock(&mutex);
        if (cola_vacia()){
            printf("Agregando archivo %d a la cola\n", *num);
            cola_impresion[0] = *num;
            flag = true;
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
        if (flag){
            return NULL;
        }
    }
}

void* imprimir_archivo(void *arg){

    int num_archivos = 0;
    while(true){
        pthread_mutex_lock(&mutex);
        while(cola_vacia()){
            pthread_cond_wait(&cond, &mutex);
        }
        printf("Imprimiendo archivo %d\n", cola_impresion[0]);
        cola_impresion[0] = -1;
        num_archivos++;
        if (num_archivos == MAX_THREADS){
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
        pthread_mutex_unlock(&mutex);
    }

}

int main(int argc, char *argv[]) {


    pthread_t users[MAX_THREADS];
    pthread_t printer[1];
    pthread_create(&printer[0], NULL, imprimir_archivo, NULL);

    for (int i=0; i<MAX_THREADS; i++){
        int *arg = malloc(sizeof(int));  // Reservar memoria para cada hilo
        *arg = i;  

        pthread_create(&users[i], NULL, enqueue, arg);
    }

    for (int i=0; i<MAX_THREADS; i++){
        pthread_join(users[i], NULL);
    }

    pthread_join(printer[0], NULL);
}
