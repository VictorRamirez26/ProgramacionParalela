#include <stdio.h> //Funciones IO
#include <stdlib.h> //Conversión (atoi), asignación de memoria (malloc, free)
#include <pthread.h> //Uso de hilos 
#include <unistd.h>  //Para usleep, que permite pausar la ejecución de un hilo

int variableCompartida = 0;
pthread_mutex_t mutex; //Mutex para sincronización

void* thread1_funcion(void* arg) {
	int id = *(int*)arg;
	printf("Thread1 instancia %d\n", id);
	usleep((rand() % 1000000)); //Suspensión aleatoria entre 0 y 1

	pthread_mutex_lock(&mutex);
	variableCompartida++;
	pthread_mutex_unlock(&mutex);

	free(arg);
	return NULL;
}

void* thread2_funcion(void* arg){
	int id = *(int*)arg;
	printf("Thread2 instancia %d\n", id);
	usleep((rand() % 1000000));

	pthread_mutex_lock(&mutex);
	printf("Valor de la variable compartida: %d\n", variableCompartida);
	pthread_mutex_unlock(&mutex);

	free(arg);
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Uso: %s <N> <M>\n", argv[0]);
	}

	int N = atoi(argv[1]);
	int M = atoi(argv[2]);
	pthread_t threads[N + M];
	pthread_mutex_init(&mutex, NULL);

	srand(time(NULL));

	for (int i = 0; i < N; i++) {
		int* id = malloc(sizeof(int));
		*id = i;
		pthread_create(&threads[i], NULL, thread1_funcion, id);
	}

	for (int i = 0; i < M; i++) {
		int* id = malloc(sizeof(int));
		*id = i;
		pthread_create(&threads[N + i], NULL, thread2_funcion, id);
	}

	for (int i = 0; i < N + M; i++) {
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	printf("Fin del programa\n");

	return 0;
}
