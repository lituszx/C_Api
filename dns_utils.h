#ifndef DNS_UTILS_H_
#define DNS_UTILS_H_

#include <stdio.h>
#include <windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

#define MAX_IP_LENGTH 16
#define MAX_RUTA 100

// Estructura para almacenar la información de un servidor DNS
typedef struct {
    char ip[MAX_IP_LENGTH];
    double tiempo_respuesta;
    int hop_count;
} ServidorDNS;

// Función para mostrar los adaptadores de red disponibles
void mostrarAdaptadoresRed();

// Función para seleccionar un adaptador de red
PIP_ADAPTER_INFO seleccionarAdaptador();

// Función para mostrar los servidores DNS de un adaptador seleccionado
char* obtenerServidorDNS(const char* ipAddress);

// Función para medir la velocidad de respuesta de un servidor DNS
double medirVelocidadDNS(const char* ip);

//Función para medir los saltos de ips
int obtenerHopCount(const char* ip);

#endif 
