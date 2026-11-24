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

// Modo local: solo archivos, sin nube
int ejecutarLocal() {
    SistemaMatricula sistema;
    FuenteArchivos archivos("datos/");

    cout << "  Modo local: los cambios no se guardan en la nube.\n";
    sistema.cargarDatos(archivos);

    Menu menu(sistema, archivos, nullptr, nullptr);
    menu.ejecutar();

    return 0;
}

int main(int argc, char* argv[]) {
    cout << "\n==========================================================\n";
    cout << "  SISTEMA DE MATRICULA UNIVERSITARIA\n";
    cout << "  Estructura de Datos II - Eliot Rojas Vargas\n";
    cout << "==========================================================\n\n";

    if (argc > 1 && string(argv[1]) == "--local") {
        return ejecutarLocal();
    }

    // Cola de mensajes y servicio de la nube
    ClienteNube cliente;
    ColaMensajes cola;
    ServicioNube servicioNube(cola, cliente);

    SistemaMatricula sistema;
    FuenteNube nube(cliente);
    FuenteArchivos archivos("datos/");

    IFuenteDatos* fuente = &nube;

    // Carga desde la nube o desde los archivos
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
