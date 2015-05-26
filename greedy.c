#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <fstream>

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
void lectura_hasta_fin_de_linea(FILE * file);
double ** lectura_costos(FILE * file, int cant_pasajeros);
double ** lectura_tiempos(FILE * file, int cant_pasajeros);

int capacidad_maxima(int * taxis);
int sacar_a_capacidad_maxima(int * taxis);

int main(int argc, char* argv[])
{
	bool debug = false;

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
	cout << "Cantidad de Pasajeros> " << cant_pasajeros << endl;

	double bandera = lectura_bandera(file);
	// lectura_precio_greedy(file);
	// lectura_solucion_greedy(file, cant_pasajeros);
	lectura_hasta_fin_de_linea(file);
	lectura_hasta_fin_de_linea(file);
	lectura_hasta_fin_de_linea(file);
	lectura_hasta_fin_de_linea(file);
	int * apurados = lectura_apurados(file, cant_pasajeros);
	cout << "Vector de Apurados> ";
	for (int i = 0; i < cant_pasajeros; i++) {
		cout << apurados[i] << " ";
	}
	cout << endl;

	int * taxis_disponibles = lectura_taxis_disponibles(file);
	cout << "Taxis Disponibles> ";
	for (int i = 0; i < 10; i++) {
		cout << taxis_disponibles[i] << " ";
	}
	cout << endl;

	double ** costos = lectura_costos(file, cant_pasajeros);
	cout << "Matriz de costos> " << endl;
	for (int i = 0; i < cant_pasajeros + 1; i++) {
		for (int j = 0; j < cant_pasajeros + 1; j++) {
			cout << costos[i][j] << "\t";
		}
		cout << endl;
	}

	// Cerramos el archivo
	fclose(file);
	
	// // Generamos la solucion
	int solucion [(cant_pasajeros * 2) - 1];
	for (int i = 0; i < (cant_pasajeros*2) - 1; i++) {
		solucion[i] = 0;
	}
    
	bool asignados [cant_pasajeros];
	for (int i = 0; i < cant_pasajeros; i++){
		asignados[i] = false;
	}

	// Buscamos la capacidad del mayor taxi
	int maxPorTaxiActual = 10;
	int cantAsignados = 0;
	int cantTaxi = 0;
	int indice = 0;
	
	while (cantAsignados < cant_pasajeros){
		cout << "DEBUG> " << "indice " << indice << endl;
		
		// Acabamos de llenar un taxi
		if (cantTaxi >= maxPorTaxiActual) {
			if(debug) {
				cout << "DEBUG> " << "taxi lleno, reiniciando todo" << endl;
			}

			solucion[indice] = 0;
			indice++;
			cantTaxi = 0;
		}
		
		// Si arrancamos un nuevo taxi
		else if (cantTaxi == 0) {

			maxPorTaxiActual = sacar_a_capacidad_maxima(taxis_disponibles);
			
			if(debug) {
				cout << "DEBUG> " << "empezando un nuevo taxi" << endl;
				cout << "DEBUG> " << "la capacidad sera de " << maxPorTaxiActual << endl;
				cout << "DEBUG> " << "buscando el mas cercano al origen" << endl;
			}

			// Nearest to the origin
			int pas = 1;
			while(asignados[pas - 1])
				pas++;
			for (int i=1; i<cant_pasajeros + 1; i++) {
				if(debug && !asignados[i-1]) {
					cout << "DEBUG> " << "el costo desde el origen al punto no asignado " << i << " es de " << costos[0][i] << endl;
				}
				if ((costos[0][i] < costos[0][pas]) && (!asignados[i-1]))
					pas = i;
			}

			if(debug) {
				cout << "DEBUG> " << "nos quedamos con " << pas << endl;
			}
			
			solucion[indice] = pas;
			asignados[pas-1] = true;
			indice++;
			cantTaxi++;
			cantAsignados++;
		}

		// Estamos llenando un taxi
		else {
	
			if(debug) {
				cout << "DEBUG> " << "llenando el taxi" << endl;
				cout << "DEBUG> " << "buscando el mas cercano al punto anterior " << solucion[indice - 1] << endl;
			}

			// Nearest of the previous marker
			int pas2 = 1;
			while(asignados[pas2 - 1])
				pas2++;

			for (int i = 1; i < cant_pasajeros + 1; i++) {
				if(debug && !asignados[i-1]) {
					cout << "DEBUG> " << "el costo desde el punto anterior al punto no asignado " << i << " es de " << costos[0][i] << endl;
				}
				if ((!(asignados[i - 1])) && ((costos[(solucion[indice - 1])][i]) < (costos[(solucion[indice - 1])][pas2])))
					pas2 = i;
			}

			if(debug) {
				cout << "DEBUG> " << "nos quedamos con " << pas2 << endl;
			}


			if(costos[(solucion[indice - 1])][pas2] > costos[0][pas2]) {
				if(debug) {
					cout << "DEBUG> " << "cierro taxi ya que es mas cercano desde el origen" << endl;
					cout << "DEBUG> " << "costo desde el destino anterior " << costos[(solucion[indice - 1])][pas2] << endl;
					cout << "DEBUG> " << "costo desde el origen " << costos[0][pas2] << endl;
				}
				// En este caso es mas barato salir desde el origen
				solucion[indice] = 0;
				indice++;
				cantTaxi = 0;

			} else {
				if(debug) {
					cout << "DEBUG> " << "no creamos taxi nuevo ya que es mas cercano desde el taxi actual" << endl;
					cout << "DEBUG> " << "costo desde el destino anterior " << costos[(solucion[indice - 1])][pas2] << endl;
					cout << "DEBUG> " << "costo desde el origen " << costos[0][pas2] << endl;
				}
				solucion[indice] = pas2;
				cantTaxi++;
				indice++;
				cantAsignados++;
				asignados[pas2 - 1] = true;
			}
		}
	}

	cout << "DEBUG> " << endl;
	for(int i = 0; i < 2*cant_pasajeros - 1; i++) {
		cout << solucion[i] << " ";
	}    
	cout << endl;
	
	// Calculate fitness
	double fitness = 0.0;
	int pos = 0;
	double costotaxi = 0;
	bool nuevotaxi = true;
	while (pos < (cant_pasajeros * 2) - 1){
		if (solucion[pos] == 0){
			nuevotaxi = true;
			fitness = fitness + costotaxi;
			costotaxi = 0;
		}
		else if ((solucion[pos] != 0) && nuevotaxi){
			nuevotaxi = false;
			costotaxi = costotaxi + bandera + (costos[0][solucion[pos]]);
		}
		else {
			costotaxi=costotaxi + (costos[solucion[pos - 1]][solucion[pos]]);
		}
		pos++;
	}
	fitness = fitness + costotaxi;
    
	cout << "Fitness> " << fitness << endl;
    
	char* aux2 = new char [256];
	strcpy(aux2, argv[1]);
	strcat(aux2, "/greedy_costos_sol.txt");
	FILE* file2 = fopen (aux2, "w");
	
	for (int j = 0; j < (cant_pasajeros * 2) - 1; j++)
		fprintf(file2,"%d ", solucion[j]);
	fclose(file2);
    
	char* aux3 = new char [256];
	strcpy(aux3, argv[1]);
	strcat(aux3, "/greedy_costos_fit.txt");
	FILE* file3 = fopen(aux3, "w");
	fprintf(file3, "%lf FITNESS", fitness);
	fclose(file3);
    
	// Return the memory
	delete [] aux2;
	delete [] aux3;
	delete [] path;
	for (int i = 0; i < cant_pasajeros + 1; i++)
		delete [] costos[i];
	delete [] costos;
	
	return 0;
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

void lectura_hasta_fin_de_linea(FILE * file) {
	char * texto;
	fscanf(file, "%s", texto);
	cout << "Texto ignorado> " << texto << endl;
};

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

int sacar_a_capacidad_maxima(int * taxis) {
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

