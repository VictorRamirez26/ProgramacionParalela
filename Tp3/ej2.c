#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_USUARIOS 10
#define CAPACIDAD_COLA 4
#define NUM_IMPRESORAS 2

// Estructura de la cola de impresión
typedef struct {
    int cola[CAPACIDAD_COLA];  // Cola de impresión con capacidad para 4 documentos
    int inicio;
    int fin;
    int archivos_impresos;
    sem_t sem_ocupada;         // Semáforo para verificar si la cola tiene espacio
    sem_t sem_vacia;           // Semáforo para verificar si la cola está vacía
    pthread_mutex_t mutex;     // Mutex para proteger el acceso a la cola
} ColaDeImpresion;

// Estructura de los usuarios
typedef struct {
    int id_usuario;
    ColaDeImpresion *cola;
} Usuario;

// Estructura de las impresoras
typedef struct {
    int id_impresora;
    ColaDeImpresion *cola;
} Impresora;

// Función para simular la acción de un usuario
void* usuario_func(void* arg) {
    Usuario* usuario = (Usuario*)arg;
    ColaDeImpresion* cola = usuario->cola;
    
    // Espera si la cola está llena
    sem_wait(&cola->sem_ocupada);

    // Bloquea el acceso a la cola para agregar un documento
    pthread_mutex_lock(&cola->mutex);
    
    // Agrega un archivo a la cola
    cola->cola[cola->fin] = usuario->id_usuario;
    cola->fin = (cola->fin + 1) % CAPACIDAD_COLA;

    // Muestra mensaje de usuario agregando archivo
    printf("Usuario %d agregó archivo a la cola de impresión\n", usuario->id_usuario);

    // Desbloquea el acceso a la cola
    pthread_mutex_unlock(&cola->mutex);

    // Señaliza que la cola tiene un archivo disponible
    sem_post(&cola->sem_vacia);

    return NULL;
}

// Función para simular la acción de una impresora
void* impresora_func(void* arg) {
    Impresora* impresora = (Impresora*)arg;
    ColaDeImpresion* cola = impresora->cola;
    
    int archivo;
    while (cola->archivos_impresos < 9) {

        // Espera si la cola está vacía
        sem_wait(&cola->sem_vacia);

        // Bloquea el acceso a la cola para procesar un archivo
        pthread_mutex_lock(&cola->mutex);

        // Si hay un archivo, procesa uno
        archivo = cola->cola[cola->inicio];
        cola->inicio = (cola->inicio + 1) % CAPACIDAD_COLA;

        // Muestra mensaje de la impresora procesando archivo
        printf("Impresora %d: Imprimiendo archivo de usuario %d\n", impresora->id_impresora, archivo);

        // Desbloquea el acceso a la cola
        pthread_mutex_unlock(&cola->mutex);

        // Señaliza que la cola tiene espacio para otro archivo
        sem_post(&cola->sem_ocupada);

        // Espera simulando el tiempo de impresión
        sleep(1);

        // Agregamos archivo
        cola->archivos_impresos++;
    }

    return NULL;
}

int main() {
    // Inicializar la cola de impresión
    ColaDeImpresion cola;
    cola.inicio = 0;
    cola.fin = 0;
    cola.archivos_impresos = 0;
    sem_init(&cola.sem_ocupada, 0, CAPACIDAD_COLA); // La cola empieza llena
    sem_init(&cola.sem_vacia, 0, 0);                // La cola empieza vacía
    pthread_mutex_init(&cola.mutex, NULL);

    // Crear hilos de usuarios
    pthread_t usuarios[NUM_USUARIOS];
    Usuario user_data[NUM_USUARIOS];
    for (int i = 0; i < NUM_USUARIOS; i++) {
        user_data[i].id_usuario = i + 1;
        user_data[i].cola = &cola;
        pthread_create(&usuarios[i], NULL, usuario_func, &user_data[i]);
    }

    // Crear hilos de impresoras
    pthread_t impresoras[NUM_IMPRESORAS];
    Impresora impresora_data[NUM_IMPRESORAS];
    for (int i = 0; i < NUM_IMPRESORAS; i++) {
        impresora_data[i].id_impresora = i + 1;
        impresora_data[i].cola = &cola;
        pthread_create(&impresoras[i], NULL, impresora_func, &impresora_data[i]);
    }

    // Esperar a que todos los hilos de usuarios terminen
    for (int i = 0; i < NUM_USUARIOS; i++) {
        pthread_join(usuarios[i], NULL);
    }

    // Finalizar hilos de impresoras (en este caso son infinitos)
    for (int i = 0; i < NUM_IMPRESORAS; i++) {
        pthread_join(impresoras[i], NULL);
    }

    // Liberar recursos
    sem_destroy(&cola.sem_ocupada);
    sem_destroy(&cola.sem_vacia);
    pthread_mutex_destroy(&cola.mutex);

    return 0;
}