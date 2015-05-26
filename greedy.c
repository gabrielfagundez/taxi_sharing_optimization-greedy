#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#define PORCENTAJE_0 0.0
#define PORCENTAJE_1 0.05
#define PORCENTAJE_2 0.1
#define PORCENTAJE_3 0.2

#define GREEDY_TIEMPO_SOL "/greedy_tiempo_sol.txt"
#define GREEDY_TIEMPO_FIT "/greedy_tiempo_fit.txt"

#define POSICION_DESOCUPADA -1

using namespace std;

int lectura_pasajeros(FILE * file);
double lectura_bandera(FILE * file);
double lectura_precio_greedy(FILE * file);
int lectura_solucion_greedy(FILE * file, int cant_pasajeros);
int * lectura_apurados(FILE * file, int cant_pasajeros);
int * lectura_taxis_disponibles(FILE * file);
double ** lectura_costos(FILE * file, int cant_pasajeros);
double ** lectura_tiempos(FILE * file, int cant_pasajeros);

double * calculo_tiempo_tolerado(double ** tiempos, int * apurados, int cant_pasajeros);

int * inicializar_solucion(int cant_pasajeros);
bool * inicializar_asignados(int cant_pasajeros);

int capacidad_maxima(int * taxis);
int guardar_en_archivo(int cant_pasajeros, char * path, double fitness_total, int * solucion);
int imprimir_taxis(int ** taxis, int cant_taxis);
int buscar_taxi_mas_cercano(int ** taxis, int cant_taxis, int pasajero_objetivo, double ** tiempos, double * tiempo_tolerado, int * taxis_disponibles);
int ultimo_pasajero_taxi(int * taxi);
int ultima_posicion_taxi(int * taxi);
double tiempo_total_taxi(int * taxi, double ** tiempos);
double fitness_pasajero(double ** tiempos, int pasajero, int * taxi, double * tiempo_tolerado);
int largo_taxi(int * taxi);
double fitness_taxi(int * taxi, double ** tiempos, double * tiempo_tolerado);

int main(int argc, char* argv[]) {

	// Mensaje previo
	if (argc < 2){
		cout << "Ingrese la ruta del archivo 'instancia.txt'. \n";
		return 0;
	}
	
	// Leemos el archivo de entrada
	char * path = new char [256];
	strcpy(path, argv[1]);
	FILE* file = fopen(strcat(path, "/instancia.txt"), "r");
	
	// Cantidad de pasajeros
	int cant_pasajeros = lectura_pasajeros(file);
	lectura_bandera(file);
	//lectura_precio_greedy(file);
	//lectura_solucion_greedy(file, cant_pasajeros);
	int * apurados = lectura_apurados(file, cant_pasajeros);
	int * taxis_disponibles = lectura_taxis_disponibles(file);
	lectura_costos(file, cant_pasajeros);
	double ** tiempos = lectura_tiempos(file, cant_pasajeros);

	// Cerramos el archivo
	fclose(file);

	double * tiempo_tolerado = calculo_tiempo_tolerado(tiempos, apurados, cant_pasajeros);

	// Generamos la solución
	int * solucion = inicializar_solucion(cant_pasajeros);
	bool * asignados = inicializar_asignados(cant_pasajeros);
	int cant_asignados = 0;
	int cant_taxis = 0;
	int ** taxis = new int *[cant_pasajeros];
	bool * taxis_cerrados = new bool [cant_pasajeros];
	for(int i = 0; i < cant_pasajeros; i++){
		taxis_cerrados[i] = false;
	} 
	double fitness_total;

	// Manejos los pasajeros con apuro nivel 0
	for (int i = 0; i < cant_pasajeros; i++){
		if(!asignados[i] && apurados[i] == 0) {
			cout << "El pasajero " << i+1 << " esta apurado y no asignado por lo que lo pongo en el taxi " << cant_taxis+1 << endl;
			
			// Creo un nuevo taxi
			taxis[cant_taxis] = new int[10];
			for(int i = 0; i < 10; i++){
				taxis[cant_taxis][i] = POSICION_DESOCUPADA;
			}
			cant_taxis++;

			// Asigno al pasajeros
			taxis[cant_taxis-1][0] = i+1;
			asignados[i] = true;
			cant_asignados++;
		}
	}

	if(cant_taxis < cant_pasajeros / 4) {
		for(int i = cant_taxis; i < (int)(cant_pasajeros / 4) + 1; i++) {

			cout << "La cantidad de taxis es " << cant_taxis << endl;

			// Creo un nuevo taxi
			taxis[cant_taxis] = new int[10];
			for(int i = 0; i < 10; i++){
				taxis[cant_taxis][i] = POSICION_DESOCUPADA;
			}
			
			bool encontre = false;

			for(int j = 0; j < cant_pasajeros; j++){
				if(!asignados[j] && apurados[j] == 1 && !encontre) {
					encontre = true;
					taxis[cant_taxis][0] = j+1;
					asignados[j] = true;
					cant_asignados++;
					cout << "Agrego pasajero de nivel 1: " << j+1 << endl; 
				}
			}
			for(int j = 0; j < cant_pasajeros; j++){
				if(!asignados[j] && apurados[j] == 2 && !encontre) {
					encontre = true;
					taxis[cant_taxis][0] = j+1;
					asignados[j] = true;
					cant_asignados++;
					cout << "Agrego pasajero de nivel 2: " << j+1 << endl; 
				}
			}
			for(int j = 0; j < cant_pasajeros; j++){
				if(!asignados[j] && apurados[j] == 3 && !encontre) {
					encontre = true;
					taxis[cant_taxis][0] = j+1;
					asignados[j] = true;
					cant_asignados++;
					cout << "Agrego pasajero de nivel 3: " << j+1 << endl; 
				}
			}
			cant_taxis++;
		}
	}

	bool continuar = true;
	while (cant_asignados < cant_pasajeros) {
		continuar = true;
		// Manejos los pasajeros con apuro nivel 1
		for (int i = 0; i < cant_pasajeros; i++) {
			if(!asignados[i] && apurados[i] == 1) { 	
				cout << "Agregando pasajero de nivel 1 > " << i+1 << endl; 

				int taxi_mas_cercano = buscar_taxi_mas_cercano(taxis, cant_taxis, i, tiempos, tiempo_tolerado, taxis_disponibles);
				taxis[taxi_mas_cercano][ultima_posicion_taxi(taxis[taxi_mas_cercano]) + 1] = i + 1;
				asignados[i] = true;
				cant_asignados++;

				// Funcion correctiva
				if(largo_taxi(taxis[taxi_mas_cercano]) == capacidad_maxima(taxis_disponibles)){
					taxis_disponibles[largo_taxi(taxis[taxi_mas_cercano]) - 1] += -1;
					taxis_cerrados[taxi_mas_cercano] = true;
					for(int i = 0; i < cant_taxis; i++) {
						if(!taxis_cerrados[i] && largo_taxi(taxis[i]) > capacidad_maxima(taxis_disponibles)){
							cout << "Se cago todo." << endl;

							// Saco la asignacion de los pasajeros en el taxi
							for(int pos = 2; pos < 10; pos++){
								if(taxis[taxi_mas_cercano][pos] != POSICION_DESOCUPADA){
									asignados[taxis[taxi_mas_cercano][pos]] = false;
									taxis[taxi_mas_cercano][pos] = POSICION_DESOCUPADA;
									cant_asignados--;
								}
							}

							// Creo un nuevo taxi
							taxis[cant_taxis] = new int[10];
							taxis[cant_taxis][0] = taxis[taxi_mas_cercano][1];
							taxis[taxi_mas_cercano][1] = POSICION_DESOCUPADA;
							for(int j = 1; j < 10; j++){
								taxis[cant_taxis][j] = POSICION_DESOCUPADA;
							}
							cant_taxis++;
							continuar = false;
						}
					}
				}
			}
		}

		// Manejos los pasajeros con apuro nivel 2
		if(continuar){	
			for (int i = 0; i < cant_pasajeros; i++) {
				if(!asignados[i] && apurados[i] == 2) { 	
					cout << "Agregando pasajero de nivel 2 > " << i+1 << endl; 

					int taxi_mas_cercano = buscar_taxi_mas_cercano(taxis, cant_taxis, i, tiempos, tiempo_tolerado, taxis_disponibles);
					taxis[taxi_mas_cercano][ultima_posicion_taxi(taxis[taxi_mas_cercano]) + 1] = i + 1;
					asignados[i] = true;
					cant_asignados++;

					// Funcion correctiva
					if(largo_taxi(taxis[taxi_mas_cercano]) == capacidad_maxima(taxis_disponibles)){
						taxis_disponibles[largo_taxi(taxis[taxi_mas_cercano]) - 1] += -1;
						taxis_cerrados[taxi_mas_cercano] = true;
						for(int i = 0; i < cant_taxis; i++) {
							if(!taxis_cerrados[i] && largo_taxi(taxis[i]) > capacidad_maxima(taxis_disponibles)){
								cout << "Se cago todo." << endl;

								// Saco la asignacion de los pasajeros en el taxi
								for(int pos = 2; pos < 10; pos++){
									if(taxis[taxi_mas_cercano][pos] != POSICION_DESOCUPADA){
										asignados[taxis[taxi_mas_cercano][pos]] = false;
										taxis[taxi_mas_cercano][pos] = POSICION_DESOCUPADA;
										cant_asignados--;
									}
								}

								// Creo un nuevo taxi
								taxis[cant_taxis] = new int[10];
								taxis[cant_taxis][0] = taxis[taxi_mas_cercano][1];
								taxis[taxi_mas_cercano][1] = POSICION_DESOCUPADA;
								for(int j = 1; j < 10; j++){
									taxis[cant_taxis][j] = POSICION_DESOCUPADA;
								}
								cant_taxis++;
								continuar = false;
							}
						}
					}
				}
			}
		}

		// Manejos los pasajeros con apuro nivel 3
		if(continuar){
			for (int i = 0; i < cant_pasajeros; i++) {
				if(!asignados[i] && apurados[i] == 3) { 
					cout << "Agregando pasajero de nivel 3 > " << i+1 << endl; 
					
					int taxi_mas_cercano = buscar_taxi_mas_cercano(taxis, cant_taxis, i, tiempos, tiempo_tolerado, taxis_disponibles);
					taxis[taxi_mas_cercano][ultima_posicion_taxi(taxis[taxi_mas_cercano]) + 1] = i + 1;
					asignados[i] = true;
					cant_asignados++;

					// Funcion correctiva
					if(largo_taxi(taxis[taxi_mas_cercano]) == capacidad_maxima(taxis_disponibles)){
						taxis_disponibles[largo_taxi(taxis[taxi_mas_cercano]) - 1] += -1;
						taxis_cerrados[taxi_mas_cercano] = true;
						for(int i = 0; i < cant_taxis; i++) {
							if(!taxis_cerrados[i] && largo_taxi(taxis[i]) > capacidad_maxima(taxis_disponibles)){
								cout << "Se cago todo." << endl;
								
								// Saco la asignacion de los pasajeros en el taxi
								for(int pos = 2; pos < 10; pos++){
									if(taxis[taxi_mas_cercano][pos] != POSICION_DESOCUPADA){
										asignados[taxis[taxi_mas_cercano][pos]] = false;
										taxis[taxi_mas_cercano][pos] = POSICION_DESOCUPADA;
										cant_asignados--;
									}
								}

								// Creo un nuevo taxi
								taxis[cant_taxis] = new int[10];
								taxis[cant_taxis][0] = taxis[taxi_mas_cercano][1];
								taxis[taxi_mas_cercano][1] = POSICION_DESOCUPADA;
								for(int j = 1; j < 10; j++){
									taxis[cant_taxis][j] = POSICION_DESOCUPADA;
								}
								cant_taxis++;
								continuar = false;
							}
						}
					}
				}
			}
		}
	}

	imprimir_taxis(taxis, cant_taxis);

	int posicion = 0;
	double fitness = 0.0;
	for(int i = 0; i < cant_taxis; i++) {
		for(int j = 0; j < 10; j++) {
			if(taxis[i][j] != POSICION_DESOCUPADA) {
				solucion[posicion] = taxis[i][j];
				posicion++;
			}
		}
		fitness += fitness_taxi(taxis[i], tiempos, tiempo_tolerado);
		solucion[posicion] = 0;
		posicion++;
	}

	guardar_en_archivo(cant_pasajeros, argv[1], fitness, solucion);

	// Imprimimos la solucion y el fitness
	cout << "Solucion: " <<  endl;
	for (int i = 0; i < (cant_pasajeros*2) - 1; i++){
		cout << solucion[i] << " ";
	}
	cout << endl;
	cout << "El fitness total es: " << endl;
	cout << fitness << endl << endl;

	return 0;
}



// Funciones auxiliares

int largo_taxi(int * taxi) {
	int iter = 0;
	while(taxi[iter] != POSICION_DESOCUPADA) {
		iter++;
	}
	return iter;
}

int buscar_taxi_mas_cercano(int ** taxis, int cant_taxis, int pasajero_objetivo, double ** tiempos, double * tiempo_tolerado, int * taxis_disponibles) {
	double fitness = 999999999;
	int taxi_optimo = 0;
	for(int taxi = 0; taxi < cant_taxis; taxi++){
		if(largo_taxi(taxis[taxi]) < capacidad_maxima(taxis_disponibles)){
			double fitness_taxi_actual = fitness_pasajero(tiempos, pasajero_objetivo, taxis[taxi], tiempo_tolerado);
			if(fitness_taxi_actual < fitness){
				fitness = fitness_taxi_actual;
				taxi_optimo = taxi;
			}
		}
	}
	return taxi_optimo;
}

int ultimo_pasajero_taxi(int * taxi) {
	int ultimo_pasajero = taxi[0];
	for(int pasajero = 0; pasajero < 10; pasajero++) {
		if(taxi[pasajero] != POSICION_DESOCUPADA) {
			ultimo_pasajero = taxi[pasajero];
		}
	}
	return ultimo_pasajero;
}

int ultima_posicion_taxi(int * taxi) {
	int ultima_posicion = 0;
	for(int pasajero = 0; pasajero < 10; pasajero++) {
		if(taxi[pasajero] != POSICION_DESOCUPADA) {
			ultima_posicion = pasajero;
		}
	}
	return ultima_posicion;
}

double tiempo_total_taxi(int * taxi, double ** tiempos) {
	double tiempo_taxi = tiempos[0][taxi[0]];
	for(int i = 1; i < 10; i++) {
		if(taxi[i] != POSICION_DESOCUPADA) {
			tiempo_taxi += tiempos[taxi[i-1]][taxi[i]];
		}
	}
	return tiempo_taxi;
}

double fitness_pasajero(double ** tiempos, int pasajero, int * taxi, double * tiempo_tolerado) {
	int ultimo_pasajero = ultimo_pasajero_taxi(taxi);
	int tiempo_taxi = tiempo_total_taxi(taxi, tiempos) + tiempos[ultimo_pasajero][pasajero];
    
    double fitness_tiempo = tiempo_taxi - tiempo_tolerado[pasajero];
    return fitness_tiempo;
}

double fitness_taxi(int * taxi, double ** tiempos, double * tiempo_tolerado) {
	double tiempo_taxi_actual = tiempos[0][taxi[0]];
	double fitness = tiempo_taxi_actual - tiempo_tolerado[taxi[0] - 1];

	for(int i = 1; i < 10; i++) {
		if(taxi[i] != POSICION_DESOCUPADA) {
			cout << "SUMANDO TIEMPO DE " << taxi[i-1] << " a " << taxi[i] << endl;
			tiempo_taxi_actual += tiempos[taxi[i-1]][taxi[i]]; 
			double tiempo_solo = tiempos[0][taxi[i]];
			fitness += tiempo_taxi_actual - tiempo_tolerado[taxi[i] - 1];
		}
	}	

	return fitness;
}

int imprimir_taxis(int ** taxis, int cant_taxis) { 
	cout << "*** *** *** TAXIS *** *** ***" << endl;
	for(int i = 0; i < cant_taxis; i++){
		cout << "TAXI " << i+1 << " -> ";
		for(int j = 0; j < 10; j++) {
			if(taxis[i][j] != -1) {
				cout << taxis[i][j] << " ";
			}
		}
		cout << endl;
	}
	cout << "*** *** *** TAXIS *** *** ***" << endl;
	return 0;
}

double ** lectura_costos(FILE * file, int cant_pasajeros) {
	double ** costos = new double * [cant_pasajeros + 1];
	for (int i = 0; i < cant_pasajeros + 1; i++){
		costos[i] = new double [cant_pasajeros + 1];
	}
	double costo;
	for (int i = 0; i < cant_pasajeros + 1; ++i) {
		for (int j = 0; j < cant_pasajeros + 1; j++){
			fscanf(file, "%lf", &costo);
			costos[i][j] = costo;
		};
	};

	return costos;
}

double ** lectura_tiempos(FILE * file, int cant_pasajeros) {
	double ** tiempos = new double * [cant_pasajeros + 1];
	for (int i = 0; i < cant_pasajeros + 1; i++){
		tiempos[i] = new double [cant_pasajeros + 1];
	}
	double tiempo;
	for (int i = 0; i < cant_pasajeros + 1; ++i) {
		for (int j = 0; j < cant_pasajeros + 1; j++){
			fscanf(file, "%lf", &tiempo);
			tiempos[i][j] = tiempo;
		};
	};

	return tiempos;
}

int lectura_pasajeros(FILE * file) {
	int i;
	fscanf (file, "%d", &i);
	int cant_pasajeros = (i + 1) / 2;

	return cant_pasajeros;
}

double lectura_bandera(FILE * file) {
	double bandera;
	fscanf(file, "%lf", &bandera);
	return bandera;
}

double lectura_precio_greedy(FILE * file) {
	double precio_greedy;
	fscanf(file, "%lf", &precio_greedy);
	return precio_greedy;
}

int lectura_solucion_greedy(FILE * file, int cant_pasajeros) {
	int solucion_greedy;
	for (int i = 0; i < cant_pasajeros * 2 - 1; i++){
		fscanf(file, "%d", &solucion_greedy);
	}
	return 0;
}

int * lectura_apurados(FILE * file, int cant_pasajeros) {
	int * apurados = new int [cant_pasajeros];
	int valor_apurado;
	for (int i = 0; i < cant_pasajeros; i++){
		fscanf(file, "%d", &valor_apurado);
		apurados[i] = valor_apurado;
	}

	// cout << "*** *** APURADOS *** ***" << endl;
	// for (int i = 0; i < cant_pasajeros; i++){
	// 	cout << apurados[i] << " ";
	// }
	// cout << "*** *** *** ***" << endl;

	return apurados;
}

int * lectura_taxis_disponibles(FILE * file) {
	int * taxis = new int [10];
	int taxi;
	for (int i = 0; i < 10; i++){
		fscanf(file, "%d", &taxi);
		taxis[i] = taxi;
	}
	return taxis;
}

double * calculo_tiempo_tolerado(double ** tiempos, int * apurados, int cant_pasajeros) {
	double * tiempo_tolerado = new double [cant_pasajeros];
	for(int i = 0; i < cant_pasajeros; i++){
		if (apurados[i] == 0){
			tiempo_tolerado[i] = tiempos[0][i+1] * (1 + PORCENTAJE_0);
		}
		if (apurados[i] == 1){
			tiempo_tolerado[i] = tiempos[0][i+1] * (1 + PORCENTAJE_1);
		}
		if (apurados[i] == 2){
			tiempo_tolerado[i] = tiempos[0][i+1] * (1 + PORCENTAJE_2);
		}
		if (apurados[i] == 3){
			tiempo_tolerado[i] = tiempos[0][i+1] * (1 + PORCENTAJE_3);
		}
	}

	return tiempo_tolerado;
} 

int * inicializar_solucion(int cant_pasajeros) {
	int * solucion = new int[(cant_pasajeros * 2) - 1];
	for (int i = 0; i < (cant_pasajeros * 2) - 1; i++){
		solucion[i] = 0;
	}
	return solucion;
}

bool * inicializar_asignados(int cant_pasajeros) {
	bool * asignados = new bool[cant_pasajeros];
	for (int i = 0; i < cant_pasajeros; i++){
		asignados[i] = false;
	}

	return asignados;
}

int capacidad_maxima(int * taxis) {
	int maximo;
	int iter = 9;
	while(iter >= 0){
		if(taxis[iter] != 0){
			taxis[iter] = taxis[iter] - 1;
			maximo = iter + 1;
			iter = -1;
		}
		iter--;
	}

	return maximo;
}

int guardar_en_archivo(int cant_pasajeros, char * path, double fitness_total, int * solucion) {

	cout << "GUARDANDO EN ARCHIVO" << endl;
	cout << "Path: " << path << endl;	

	char * greedy_tiempo_sol = new char [256];
	strcpy(greedy_tiempo_sol, path);
	strcat(greedy_tiempo_sol, GREEDY_TIEMPO_SOL);
	FILE* greedy_tiempo_sol_file = fopen (greedy_tiempo_sol, "w");
	for (int j = 0; j < (cant_pasajeros * 2) - 1; j++)
		fprintf(greedy_tiempo_sol_file,"%d ", solucion[j]);
	fclose(greedy_tiempo_sol_file);
 
	char * greedy_tiempo_fit = new char [256];
	strcpy(greedy_tiempo_fit, path);
	strcat(greedy_tiempo_fit, GREEDY_TIEMPO_FIT);
	FILE * greedy_tiempo_fit_file = fopen(greedy_tiempo_fit, "w");
	fprintf(greedy_tiempo_fit_file, "%lf FITNESS", fitness_total);
	fclose(greedy_tiempo_fit_file);

	return 0;
}
