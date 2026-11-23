// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Programa principal: carga los datos y abre el menu

#include <iostream>
#include "app/SistemaMatricula.h"
#include "app/Menu.h"

using namespace std;

int main() {
    cout << "\n==========================================================\n";
    cout << "  SISTEMA DE MATRICULA UNIVERSITARIA\n";
    cout << "  Estructura de Datos II - Eliot Rojas Vargas\n";
    cout << "==========================================================\n\n";

    SistemaMatricula sistema;
    FuenteArchivos archivos("datos/");

    sistema.cargarDatos(archivos);

    Menu menu(sistema, archivos);
    menu.ejecutar();

    return 0;
}
