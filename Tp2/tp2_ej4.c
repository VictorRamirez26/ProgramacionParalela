#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_VEHICULOS 100
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Declarar e inicializar el mutex
int num_ticket = 0;

enum vehicle_types {
	MOTOCICLETA,
	COCHE,
	UTILITARIO,
	PESADO,
	DESCONOCIDO
};

typedef struct {
	int id;
	enum vehicle_types tipo;
	float dinero;
} Vehiculo;


typedef struct {
	Vehiculo *vehiculos;  // Puntero al primer vehículo de la sublista
    int cantidad;         // Cantidad de vehículos a procesar
} DatosPeaje;

typedef struct {
	DatosPeaje datos_peaje;
	int num_hilo;
	const char *file_path;
} Parametros_hilo;


const char* vehicle_types_string(enum vehicle_types type) {
	switch (type) {
		case MOTOCICLETA: return "motocicleta";
		case COCHE: return "coche";
		case PESADO: return "pesado";
		case UTILITARIO: return "utilitario";
		default: return "desconocido";
	}
}

void escribirVehiculos(Vehiculo vehiculo, const char *file_path) {
    FILE *archivo = fopen(file_path, "a"); // Modo "a" para no sobrescribir
    if (archivo == NULL) {
        perror("Error al abrir el archivo de salida");
        return;
    }

    fprintf(archivo, "id:%d;cat:%s; din:%.2f; ticket:%d\n", 
            vehiculo.id, vehicle_types_string(vehiculo.tipo), vehiculo.dinero, num_ticket);

    fclose(archivo);
}

void incrementar(Vehiculo vehiculo, const char *file_path) {
	pthread_mutex_lock(&mutex); // Bloquear el mutex
	escribirVehiculos(vehiculo, file_path);
	num_ticket++;
	pthread_mutex_unlock(&mutex); // Desbloquear el mutex
}

void demora(int tipo, int num_hilo) {
	switch (tipo) {
		case 0: 
			printf("Hilo %d cobrando motocicleta\n", num_hilo);
			sleep(1); 
			break;
		case 1: 
			printf("Hilo %d cobrando coche\n", num_hilo);
			sleep(2); 
			break;
		case 2: 
			printf("Hilo %d cobrando pesado\n", num_hilo);
			sleep(3); 
			break;
		case 3: 
			printf("Hilo %d cobrando utilitario\n", num_hilo);
			sleep(4); 
			break;
		default: 
			printf("Hilo %d cobrando desconocido\n", num_hilo);
			sleep(5); 
			break;
	}
}

void cobrar_peaje(int num_hilo, DatosPeaje *datos, int i) {

	switch (datos->vehiculos[i].tipo) {
		case 0: 
			datos->vehiculos[i].dinero -= 1;
			demora(0,num_hilo);
			   break;
		case 1: 
			datos->vehiculos[i].dinero -= 2.5;
			demora(1,num_hilo);
			break;
		case 2: 
			datos->vehiculos[i].dinero -= 5; 
			demora(2,num_hilo);
			break;
		case 3: 
			datos->vehiculos[i].dinero -= 3.5;
			demora(3,num_hilo);
			break;
		default: 
			datos->vehiculos[i].dinero -= 2; 
			demora(4,num_hilo);
			break;
	}
}

void* cobrar(void *arg) {
	Parametros_hilo *parametros = (Parametros_hilo *)arg;
	DatosPeaje *datos = &parametros->datos_peaje;
	int num_hilo = parametros->num_hilo;

    for (int i = 0; i < datos->cantidad; i++) {
		cobrar_peaje(num_hilo, datos, i);
		incrementar(datos->vehiculos[i], parametros->file_path);
    }

    pthread_exit(NULL);
}


enum vehicle_types obtenerTipo(const char *cadena) {
	if (strcmp(cadena, "motocicleta") == 0) return MOTOCICLETA;
	if (strcmp(cadena, "coche") == 0) return COCHE;
	if (strcmp(cadena, "pesado") == 0) return PESADO;
	if (strcmp(cadena, "utilitario") == 0) return UTILITARIO;
	return DESCONOCIDO;
}

int leerVehiculos(const char *file_path, Vehiculo vehiculos[], int max_vehiculos) {
	FILE *archivo = fopen(file_path, "r");
	if (archivo == NULL) {
		perror("Error al abrir el archivo");
		return -1;
	}

	int i = 0;
	char linea[100], tipoStr[20];

	while (i < max_vehiculos && fgets(linea, sizeof(linea), archivo) != NULL) {
		if (sscanf(linea, "id:%d;cat:%19[^;];din:%f", &vehiculos[i].id, tipoStr, &vehiculos[i].dinero) == 3) {
			vehiculos[i].tipo = obtenerTipo(tipoStr);
			i++;
		}
	}
	fclose(archivo);
	return i;  
}


int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Uso: %s <archivo_vehiculos>\n", argv[0]);
		return 1;
	}

	char *file_path = argv[1];
	Vehiculo vehiculos[MAX_VEHICULOS];

	int cantidad_vehiculos = leerVehiculos(file_path, vehiculos, MAX_VEHICULOS);
	if (cantidad_vehiculos == -1) {
		return 1;
	}

	pthread_t hilos[4];
	int puestos_peaje = 4;
	int vehiculos_por_puesto = cantidad_vehiculos / puestos_peaje;
	int resto = cantidad_vehiculos % puestos_peaje;

    DatosPeaje datos[4];
	const char *file_path_out = "vehiculos_out.txt";

	for (int i = 0; i < puestos_peaje; i++) {
		int inicio = i * vehiculos_por_puesto;
		int fin = inicio + vehiculos_por_puesto;
		if (i == puestos_peaje - 1) {
			fin += resto;
		}
	
		datos[i].vehiculos = &vehiculos[inicio];  // Pasamos la dirección del inicio del bloque
		datos[i].cantidad = fin - inicio;
	
		Parametros_hilo *parametros = (Parametros_hilo*)malloc(sizeof(Parametros_hilo)); // Asignar memoria para cada hilo
		parametros->file_path = file_path_out;
		parametros->datos_peaje = datos[i];
		parametros->num_hilo = i; // Asignamos el número del hilo
	
		pthread_create(&hilos[i], NULL, cobrar, (void*)parametros); // Pasamos el puntero a la memoria del hilo
	}
	
	for (int i = 0; i < puestos_peaje; i++) {
		pthread_join(hilos[i], NULL);
	}
	
	printf("Resultados guardados en vehiculos_out.txt\n");
	return 0;
}
