#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <iphlpapi.h>
#include <wchar.h>
#include "dns_utils.h"

#pragma comment(lib, "Setupapi.lib")

#define MAX_HOPS 30
#define MAX_IP_LENGTH 16
void mostrarAdaptadoresRed() {
	PIP_ADAPTER_INFO adaptador;
	DWORD tamaño = sizeof(IP_ADAPTER_INFO);
	DWORD resultado;

	adaptador = (IP_ADAPTER_INFO*)malloc(tamaño);
	if (adaptador == NULL) {
		printf("Error al reservar memoria para adaptadores de red.\n");
		return;
	}

	// Obtener información sobre adaptadores de red
	resultado = GetAdaptersInfo(adaptador, &tamaño);
	if (resultado == ERROR_BUFFER_OVERFLOW) {
		free(adaptador);
		adaptador = (IP_ADAPTER_INFO*)malloc(tamaño);
		if (adaptador == NULL) {
			printf("Error al reservar memoria para adaptadores de red.\n");
			return;
		}
		resultado = GetAdaptersInfo(adaptador, &tamaño);
	}

	if (resultado == ERROR_SUCCESS) {
		printf("Adaptadores de red:\n\n ");
		int contador = 1;
		while (adaptador) {
			printf("%d. Nombre: %s\n", contador, adaptador->AdapterName);
			printf("   Descripción: %s\n", adaptador->Description);
			printf("   Dirección IP: %s\n", adaptador->IpAddressList.IpAddress.String);
			printf("   Máscara de subred: %s\n", adaptador->IpAddressList.IpMask.String);
			printf("   Puerta de enlace predeterminada: %s\n", adaptador->GatewayList.IpAddress.String);
			printf("\n");
			adaptador = adaptador->Next;
			contador++;
		}
	}
	else {
		printf("Error al obtener información sobre adaptadores de red.\n");
	}

	free(adaptador);
}

PIP_ADAPTER_INFO seleccionarAdaptador() {
	int opcion;
	printf("Seleccione el adaptador de red (ingrese el numero correspondiente): ");
	scanf_s("%d", &opcion);

	PIP_ADAPTER_INFO adaptador;
	DWORD tamaño = sizeof(IP_ADAPTER_INFO);
	DWORD resultado;

	adaptador = (IP_ADAPTER_INFO*)malloc(tamaño);
	if (adaptador == NULL) {
		printf("Error al reservar memoria para adaptadores de red.\n");
		return NULL;
	}

	// Obtener información sobre adaptadores de red
	resultado = GetAdaptersInfo(adaptador, &tamaño);
	if (resultado == ERROR_BUFFER_OVERFLOW) {
		free(adaptador);
		adaptador = (IP_ADAPTER_INFO*)malloc(tamaño);
		if (adaptador == NULL) {
			printf("Error al reservar memoria para adaptadores de red.\n");
			return NULL;
		}
		resultado = GetAdaptersInfo(adaptador, &tamaño);
	}

	if (resultado == ERROR_SUCCESS) {
		int contador = 1;
		while (adaptador) {
			if (contador == opcion) {
				return adaptador;
			}
			adaptador = adaptador->Next;
			contador++;
		}
	}
	else {
		printf("Error al obtener informacion sobre adaptadores de red.\n");
	}

	free(adaptador);
	return NULL;
}

char* obtenerServidorDNS(const char* ipAddress) {
	char comando[100];
	sprintf_s(comando, sizeof(comando), "nslookup %s > temp.txt", ipAddress);

	// Ejecutar el comando nslookup y redirigir la salida estándar a un archivo temporal
	system(comando);

	FILE* file;
	if (fopen_s(&file, "temp.txt", "r") != 0) {
		fprintf(stderr, "Error: No se pudo abrir el archivo temporal\n");
		return NULL;
	}

	char line[256];
	char* dns_server = NULL;
	while (fgets(line, sizeof(line), file) != NULL) {
		// Buscar la línea que contiene la dirección IP del servidor DNS
		if (strstr(line, "Address:") != NULL) {
			// Obtener la dirección IP del servidor DNS de la línea
			char* ip_start = strstr(line, ":");
			if (ip_start != NULL) {
				// Avanzar al primer carácter después del delimitador ":"
				ip_start += 1;
				// Eliminar los espacios en blanco al principio de la cadena
				while (isspace(*ip_start)) {
					++ip_start;
				}
				// Copiar la dirección IP del servidor DNS
				dns_server = _strdup(ip_start);
				break;
			}
		}
	}
	fclose(file);

	// Eliminar el archivo temporal
	remove("temp.txt");
	return dns_server;
}

double medirVelocidadDNS(const char* ip) {
	char comando[100];
	clock_t inicio, fin;
	double tiempo_transcurrido;

	// Construir el comando nslookup para obtener la información del servidor DNS
	sprintf_s(comando, sizeof(comando), "nslookup %s", ip);

	// Registrar el tiempo de inicio
	inicio = clock();

	// Ejecutar el comando nslookup
	system(comando);

	// Registrar el tiempo de fin
	fin = clock();

	// Calcular el tiempo transcurrido en segundos
	tiempo_transcurrido = ((double)(fin - inicio)) / CLOCKS_PER_SEC;

	return tiempo_transcurrido;
}

int obtenerHopCount(const char* ip) {
	char comando[100];
	FILE* pipe;
	int hop_count = -1;

	// Construir el comando tracert
	sprintf_s(comando, sizeof(comando), "tracert -h 1 %s", ip);

	// Ejecutar el comando tracert y capturar la salida
	pipe = _popen(comando, "r");
	if (pipe != NULL) {
		char buffer[256];
		while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
			// Buscar el hop count en la salida de tracert
			char* hop_str = strstr(buffer, " 1 ");
			if (hop_str != NULL) {
				sscanf_s(hop_str, "%*d %*d %*d %d", &hop_count);
				break;
			}
		}
		_pclose(pipe);
	}

	return hop_count;
}
void obtenerIPsSaltos(const char* servidor_dns, char ips_saltos[MAX_HOPS][MAX_IP_LENGTH], int* hop_count) {
	char comando[100];
	FILE* pipe;

	// Construir el comando tracert
	sprintf_s(comando, sizeof(comando), "tracert -h %d %s", MAX_HOPS, servidor_dns);

	// Ejecutar el comando tracert y capturar la salida
	pipe = _popen(comando, "r");
	if (pipe != NULL) {
		char buffer[256];
		*hop_count = 0;
		while (fgets(buffer, sizeof(buffer), pipe) != NULL && *hop_count < MAX_HOPS) {
			// Buscar la dirección IP en la salida de tracert
			char* ip_str = strstr(buffer, "[");
			if (ip_str != NULL) {
				// Avanzar al primer carácter después del '['
				ip_str += 1;
				// Buscar el cierre del corchete ']'
				char* ip_end = strstr(ip_str, "]");
				if (ip_end != NULL) {
					*ip_end = '\0';  // Terminar la cadena en el cierre del corchete
					// Copiar la dirección IP del salto
					strncpy_s(ips_saltos[*hop_count], MAX_IP_LENGTH, ip_str, _TRUNCATE);
					*hop_count += 1;
				}
			}
		}
		_pclose(pipe);
	}
}