// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Programa principal: arma los componentes, carga los datos y abre el menu

#include <iostream>
#include "app/SistemaMatricula.h"
#include "app/Menu.h"
#include "nube/ClienteNube.h"
#include "nube/FuenteNube.h"
#include "mensajeria/ColaMensajes.h"
#include "mensajeria/ServicioNube.h"

using namespace std;

int main() {
    cout << "\n==========================================================\n";
    cout << "  SISTEMA DE MATRICULA UNIVERSITARIA\n";
    cout << "  Estructura de Datos II - Eliot Rojas Vargas\n";
    cout << "==========================================================\n\n";

    // Comunicacion: cola de mensajes atendida por el servicio de la nube
    ClienteNube cliente;
    ColaMensajes cola;
    ServicioNube servicioNube(cola, cliente);

    SistemaMatricula sistema;
    FuenteNube nube(cliente);
    FuenteArchivos archivos("datos/");

    IFuenteDatos* fuente = &nube;

    // Primero la nube; si no hay conexion o esta vacia, los archivos locales
    if (!sistema.cargarDatos(nube)) {
        cout << "\n  Se usan los datos de los archivos locales.\n";
        fuente = &archivos;
        sistema.cargarDatos(archivos);

        if (nube.estabaVacia()) {
            cout << "  Para guardarlos en la nube: menu 6, opcion 5.\n";
        }
    }

    sistema.conectarCola(&cola);
    servicioNube.iniciar();

    Menu menu(sistema, *fuente, &servicioNube, &cliente);
    menu.ejecutar();

    return 0;
}
