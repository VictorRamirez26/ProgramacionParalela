#include <stdio.h>
#include <stdlib.h> //COnversión (atoi), asignación de memoria (malloc, free)
#include <unistd.h> //Para unslep, que permite pausar la ejecución de un hilo
#include <sys/wait.h>
#include <time.h>

int variableCompartida = 0;

void process_type_1(int x){
	printf("Process1 instancia %d\n", x);
	usleep(rand() % 1 * 1000000);
	variableCompartida++;
}

void process_type_2(int y) {
	printf("Process2 instancia %d\n", y);
	usleep(rand() %  1 * 1000000);
	printf("Valor de variable compartida: %d\n", variableCompartida);
}

int main(int argc, char*argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Uso: %s <N> <M>\n", argv[0]);
		return 1;
	}

	int N = atoi(argv[1]);
	int M = atoi(argv[2]);

	srand(time(NULL));

	for (int i = 0; i < N; i++) {
		pid_t pid = fork();
		if (pid == -1) {
			perror("Error en fork");
			return 1;
		}
		if (pid == 0) { // Proceso hijo
			process_type_1(i);
			return 0;
		}
	}

	for (int i = 0; i < M; i++) {
		pid_t pid = fork();
		if (pid == -1) {
			perror("Error en fork");
			return 1;
		}
		if (pid == 0) { //Proceso hijo
			process_type_2(i);
			return 0;
		}
	}

	for (int i = 0; i < N + M; i++) {
		wait(NULL);
	}

	printf("Fin del programa\n");

	return 0;
}
