// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas del sistema: programa principal y conteo de memoria

#include <iostream>
#include <cstdlib>
#include <new>
#include <atomic>
#include "Pruebas.h"

using namespace std;

// Conteo de memoria

atomic<long long> reservas(0);
atomic<long long> liberaciones(0);

void* operator new(size_t bytes) {
    reservas++;
    void* bloque = malloc(bytes);

    if (bloque == nullptr) {
        throw bad_alloc();
    }

    return bloque;
}

void operator delete(void* bloque) noexcept {
    if (bloque != nullptr) {
        liberaciones++;
        free(bloque);
    }
}

void operator delete(void* bloque, size_t) noexcept {
    if (bloque != nullptr) {
        liberaciones++;
        free(bloque);
    }
}

// Apoyo

int pruebasHechas = 0;
int pruebasFalladas = 0;

// Salida de los resultados
streambuf* pantalla = nullptr;

void revisar(bool condicion, string descripcion) {
    ostream salida(pantalla != nullptr ? pantalla : cout.rdbuf());
    pruebasHechas++;

    if (condicion) {
        salida << "  [ok]    " << descripcion << "\n";
    } else {
        salida << "  [FALLO] " << descripcion << "\n";
        pruebasFalladas++;
    }
}

// Programa principal

int main() {
    pantalla = cout.rdbuf();
    cout << "PRUEBAS DEL SISTEMA DE MATRICULA\n";

    long long reservasAntes = reservas;
    long long liberacionesAntes = liberaciones;

    {
        probarArchivos();
        probarArchivosDelPlan();
        probarDominio();
        probarInsercion();
        probarEliminacion();
        probarAlAzar();
        probarAltura();
        probarReprogramar();
        probarHashBasico();
        probarHashEliminacion();
        probarHashAlAzar();
        probarHashGrande();
        probarBPlusBasico();
        probarBPlusEliminacion();
        probarBPlusAlAzar();
        probarBPlusGrande();
        probarGrafoBasico();
        probarGrafoCambios();
        probarGrafoAlAzar();
        probarColaBasica();
        probarColaAlAzar();
        probarSistemaCarga();
        probarSistemaEscenario();
        probarSistemaReglas();
        probarSistemaBorrados();
        probarSistemaCitasPorPromedio();
        probarSistemaExportar();
        probarJson();
        probarColecciones();
        probarColaMensajes();
        probarMensajesDeComponentes();
        probarNubeReal();
    }

    long long reservadas = reservas - reservasAntes;
    long long liberadas = liberaciones - liberacionesAntes;

    cout << "\nMemoria\n";
    cout << "  Reservas: " << reservadas << ", liberaciones: " << liberadas << "\n";
    revisar(reservadas == liberadas, "no quedan bloques sin liberar");

    cout << "\nResultado: " << pruebasHechas - pruebasFalladas << " de " << pruebasHechas << " pruebas correctas\n";

    if (pruebasFalladas > 0) {
        return 1;
    }

    return 0;
}
