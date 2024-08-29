#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iphlpapi.h>
#include "dns_utils.h" // Incluimos nuestra librería de utilidades DNS
#include <errno.h>

#pragma comment(lib, "iphlpapi.lib")

#define MAX_IP_LENGTH 16
#define MAX_HOPS 30
// Estructura para almacenar la información del adaptador de red
typedef struct {
    char ip[MAX_IP_LENGTH];
    char mascara[MAX_IP_LENGTH];
    char puerta_enlace[MAX_IP_LENGTH];
    char servidor_dns[MAX_IP_LENGTH];
    double velocidad_dns;
    int hop_count;
    char ips_saltos[MAX_HOPS][MAX_IP_LENGTH];
} InformacionAdaptador;
// Función para generar un archivo XML con la información del adaptador de red
void generarArchivoXML(InformacionAdaptador info) {
    FILE* archivoXML;
    errno_t err;
    if ((err = fopen_s(&archivoXML, "informacion_adaptador.xml", "w")) != 0) {
        printf("Error al crear el archivo XML. Código de error: %d\n", err);
        return;
    }

    // Escribir el encabezado del XML
    fprintf(archivoXML, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(archivoXML, "<?xml-stylesheet type=\"text/xsl\" href=\"estilo.xslt\"?>\n");
    fprintf(archivoXML, "<adaptador>\n");
    fprintf(archivoXML, "  <ip>%s</ip>\n", info.ip);
    fprintf(archivoXML, "  <mascara>%s</mascara>\n", info.mascara);
    fprintf(archivoXML, "  <puerta_enlace>%s</puerta_enlace>\n", info.puerta_enlace);
    fprintf(archivoXML, "  <servidor_dns>\n");
    fprintf(archivoXML, "    <ip>%s</ip>\n", info.servidor_dns);
    fprintf(archivoXML, "    <velocidad_media_respuesta>%.2f</velocidad_media_respuesta>\n", info.velocidad_dns);
    fprintf(archivoXML, "    <hop_count>%d</hop_count>\n", info.hop_count);
    // Escribir las IPs de los saltos en el XML
    fprintf(archivoXML, "    <ips_saltos>\n");
    for (int i = 0; i < info.hop_count; ++i) {
        fprintf(archivoXML, "      <salto>%s</salto>\n", info.ips_saltos[i]);
    }
    fprintf(archivoXML, "    </ips_saltos>\n");
    fprintf(archivoXML, "  </servidor_dns>\n");
    fprintf(archivoXML, "</adaptador>\n");

    fclose(archivoXML);
    printf("Archivo XML generado con éxito: informacion_adaptador.xml\n");
}
//Funcion para inicializar la classe de InformacionAdaptador a valores vacios para que no salte el warning de inicializacion.
void inicializarInformacionAdaptador(InformacionAdaptador* info) {
    strcpy_s(info->ip, sizeof(info->ip), "");
    strcpy_s(info->mascara, sizeof(info->mascara), "");
    strcpy_s(info->puerta_enlace, sizeof(info->puerta_enlace), "");
    strcpy_s(info->servidor_dns, sizeof(info->servidor_dns), "");
    info->velocidad_dns = 0.0;
    info->hop_count = 0;
    for (int i = 0; i < MAX_HOPS; ++i) {
        strcpy_s(info->ips_saltos[i], sizeof(info->ips_saltos[i]), "");
    }
}
//Funcion para generar el archivo XSLT
void generarArchivoXSLT() {
    FILE* archivoXSLT;
    errno_t err;
    if ((err = fopen_s(&archivoXSLT, "estilo.xslt", "w")) != 0) {
        printf("Error al crear el archivo XSLT. Código de error: %d\n", err);
        return;
    }

    // Escribir el contenido del XSLT
    fprintf(archivoXSLT, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(archivoXSLT, "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n");
    fprintf(archivoXSLT, "  <xsl:output method=\"html\" encoding=\"UTF-8\" indent=\"yes\"/>\n");
    fprintf(archivoXSLT, "  <xsl:template match=\"/\">\n");
    fprintf(archivoXSLT, "    <html>\n");
    fprintf(archivoXSLT, "      <head>\n");
    fprintf(archivoXSLT, "        <title>Informacion del Adaptador de Red</title>\n");
    fprintf(archivoXSLT, "        <style>\n");
    fprintf(archivoXSLT, "          table {\n");
    fprintf(archivoXSLT, "            width: 100%%;\n");
    fprintf(archivoXSLT, "            border-collapse: collapse;\n");
    fprintf(archivoXSLT, "          }\n");
    fprintf(archivoXSLT, "          th, td {\n");
    fprintf(archivoXSLT, "            border: 1px solid black;\n");
    fprintf(archivoXSLT, "            padding: 8px;\n");
    fprintf(archivoXSLT, "            text-align: left;\n");
    fprintf(archivoXSLT, "          }\n");
    fprintf(archivoXSLT, "          th {\n");
    fprintf(archivoXSLT, "            background-color: #33A5FF;\n");
    fprintf(archivoXSLT, "          }\n");
    fprintf(archivoXSLT, "          tr:nth-child(even) {\n");
    fprintf(archivoXSLT, "            background-color: #33DDFF;\n");
    fprintf(archivoXSLT, "          }\n");
    fprintf(archivoXSLT, "          tr:nth-child(odd) {\n");
    fprintf(archivoXSLT, "            background-color: #4EFEF9;\n");
    fprintf(archivoXSLT, "          }\n");
    fprintf(archivoXSLT, "        </style>\n");
    fprintf(archivoXSLT, "      </head>\n");
    fprintf(archivoXSLT, "      <body>\n");
    fprintf(archivoXSLT, "        <h2>Informacion del Adaptador de Red</h2>\n");
    fprintf(archivoXSLT, "        <table>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <th>Campo</th>\n");
    fprintf(archivoXSLT, "            <th>Valor</th>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <td>IP</td>\n");
    fprintf(archivoXSLT, "            <td><xsl:value-of select=\"/adaptador/ip\"/></td>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <td>Mascara</td>\n");
    fprintf(archivoXSLT, "            <td><xsl:value-of select=\"/adaptador/mascara\"/></td>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <td>Puerta de Enlace</td>\n");
    fprintf(archivoXSLT, "            <td><xsl:value-of select=\"/adaptador/puerta_enlace\"/></td>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <td>Servidor DNS IP</td>\n");
    fprintf(archivoXSLT, "            <td><xsl:value-of select=\"/adaptador/servidor_dns/ip\"/></td>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <td>Velocidad Media de Respuesta</td>\n");
    fprintf(archivoXSLT, "            <td><xsl:value-of select=\"/adaptador/servidor_dns/velocidad_media_respuesta\"/> segundos</td>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <td>Numero de Saltos</td>\n");
    fprintf(archivoXSLT, "            <td><xsl:value-of select=\"/adaptador/servidor_dns/hop_count\"/></td>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "          <tr>\n");
    fprintf(archivoXSLT, "            <td>IPs de los Saltos</td>\n");
    fprintf(archivoXSLT, "            <td>\n");
    fprintf(archivoXSLT, "              <xsl:for-each select=\"/adaptador/servidor_dns/ips_saltos/salto\">\n");
    fprintf(archivoXSLT, "                <div><xsl:value-of select=\".\"/></div>\n");
    fprintf(archivoXSLT, "              </xsl:for-each>\n");
    fprintf(archivoXSLT, "            </td>\n");
    fprintf(archivoXSLT, "          </tr>\n");
    fprintf(archivoXSLT, "        </table>\n");
    fprintf(archivoXSLT, "      </body>\n");
    fprintf(archivoXSLT, "    </html>\n");
    fprintf(archivoXSLT, "  </xsl:template>\n");
    fprintf(archivoXSLT, "</xsl:stylesheet>\n");

    fclose(archivoXSLT);
    printf("Archivo XSLT generado con éxito: estilo.xslt\n");
}
//Funcion HTML
void generarArchivoHTML(const char* xmlFile, const char* xsltFile, const char* htmlFile) {
    FILE* archivoHTML;
    errno_t err;

    if ((err = fopen_s(&archivoHTML, htmlFile, "w")) != 0) {
        printf("Error al crear el archivo HTML. Código de error: %d\n", err);
        return;
    }

    // Escribir el contenido del archivo HTML
    fprintf(archivoHTML, "<!DOCTYPE html>\n");
    fprintf(archivoHTML, "<html>\n");
    fprintf(archivoHTML, "  <head>\n");
    fprintf(archivoHTML, "    <title>Información del Adaptador de Red</title>\n");
    fprintf(archivoHTML, "    <link rel=\"stylesheet\" type=\"text/xsl\" href=\"%s\">\n", xsltFile);
    fprintf(archivoHTML, "  </head>\n");
    fprintf(archivoHTML, "  <body>\n");
    fprintf(archivoHTML, "    <iframe src=\"%s\" style=\"width:100%%; height:100vh; border:none;\"></iframe>\n", xmlFile);
    fprintf(archivoHTML, "  </body>\n");
    fprintf(archivoHTML, "</html>\n");

    fclose(archivoHTML);
    printf("Archivo HTML generado con éxito: %s\n", htmlFile);
}
int main() {
    //Inicializamos info adaptador vacia.
    InformacionAdaptador info;
    inicializarInformacionAdaptador(&info);

    //muestra los adaptadores de red
    mostrarAdaptadoresRed();

    //seleccionamos el que queremos modificar
    PIP_ADAPTER_INFO adaptadorSeleccionado = seleccionarAdaptador();
    if (adaptadorSeleccionado != NULL) {
        strcpy_s(info.ip, sizeof(info.ip), adaptadorSeleccionado->IpAddressList.IpAddress.String);
        strcpy_s(info.mascara, sizeof(info.mascara), adaptadorSeleccionado->IpAddressList.IpMask.String);
        strcpy_s(info.puerta_enlace, sizeof(info.puerta_enlace), adaptadorSeleccionado->GatewayList.IpAddress.String);
       
        // Obtener el servidor DNS
        char* servidor_dns_temp = obtenerServidorDNS(adaptadorSeleccionado->IpAddressList.IpAddress.String);
        if (servidor_dns_temp != NULL) {
            strcpy_s(info.servidor_dns, sizeof(info.servidor_dns), servidor_dns_temp);
            free(servidor_dns_temp); // Liberar la memoria asignada dinámicamente por obtenerServidorDNS
        }
        else {
            fprintf(stderr, "Error: No se pudo obtener el servidor DNS\n");
        }
        // Medir la velocidad media de respuesta del servidor DNS primario
        info.velocidad_dns = medirVelocidadDNS(info.servidor_dns);
        // Obtener el número de saltos hasta el servidor DNS primario
        info.hop_count = obtenerHopCount(info.servidor_dns);

        // Obtener las IPs de cada salto
        obtenerIPsSaltos(info.servidor_dns, info.ips_saltos, &info.hop_count);
    }
    //Generamos el archivo con la informacion en la clase.
    generarArchivoXML(info);
    generarArchivoXSLT();
    generarArchivoHTML("informacion_adaptador.xml", "estilo.xslt", "informacion_adaptador.html");
    return 0;
}

