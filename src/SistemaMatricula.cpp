// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del sistema de matricula

#include <iostream>
#include "SistemaMatricula.h"
#include "CargadorArchivos.h"

using namespace std;

SistemaMatricula::SistemaMatricula() {
    // La matricula abre el 7 de diciembre de 2026 a las 08:00
    horaActual = armarFechaHora(2026, 12, 7, 8, 0);
}

// Cargar estudiantes y citas desde los archivos

void SistemaMatricula::cargarDatos(string rutaEstudiantes, string rutaCitas) {
    estudiantes.vaciar();
    citas.vaciar();

    // Estudiantes
    vector<Estudiante> leidos;
    int lineasConError = 0;

    if (!leerEstudiantes(rutaEstudiantes, leidos, lineasConError)) {
        cout << "  No se pudo abrir " << rutaEstudiantes << "\n";
    } else {
        int repetidos = 0;

        for (int i = 0; i < (int)leidos.size(); i++) {
            if (!estudiantes.agregar(leidos[i])) {
                cout << "  Aviso: el carne " << leidos[i].carne << " esta repetido\n";
                repetidos++;
            }
        }

        cout << "  Estudiantes cargados: " << estudiantes.cantidad()
             << " (lineas con error: " << lineasConError
             << ", repetidos: " << repetidos << ")\n";
    }

    // Citas
    vector<CitaMatricula> citasLeidas;

    if (!leerCitas(rutaCitas, citasLeidas, lineasConError)) {
        cout << "  No se pudo abrir " << rutaCitas << "\n";
    } else {
        int rechazadas = 0;

        for (int i = 0; i < (int)citasLeidas.size(); i++) {
            CitaMatricula cita = citasLeidas[i];

            if (estudiantes.buscar(cita.carne) == nullptr) {
                cout << "  Aviso: cita rechazada, no existe el carne " << cita.carne << "\n";
                rechazadas++;
            } else if (citas.buscarPorCarne(cita.carne) != nullptr) {
                cout << "  Aviso: cita rechazada, " << cita.carne << " ya tiene cita\n";
                rechazadas++;
            } else if (!citas.insertar(cita)) {
                cout << "  Aviso: cita rechazada, ya hay una cita el ";
                imprimirFechaHora(cita.fechaHora);
                cout << "\n";
                rechazadas++;
            }
        }

        cout << "  Citas cargadas: " << citas.contarNodos()
             << " (lineas con error: " << lineasConError
             << ", rechazadas: " << rechazadas << ")\n";
    }
}

// Imprimir citas

void SistemaMatricula::imprimirCita(CitaMatricula cita) {
    imprimirFechaHora(cita.fechaHora);
    cout << "   " << cita.carne << "   ";

    Estudiante* estudiante = estudiantes.buscar(cita.carne);

    if (estudiante != nullptr) {
        cout << estudiante->nombre;
    }

    cout << "\n";
}

void SistemaMatricula::imprimirListaDeCitas(vector<CitaMatricula>& lista) {
    cout << "  Fecha y hora       Carne       Estudiante\n";
    cout << "  ------------------------------------------------------\n";

    for (int i = 0; i < (int)lista.size(); i++) {
        cout << "  ";
        imprimirCita(lista[i]);
    }

    cout << "\n  Total: " << lista.size() << " citas\n";
}

// Asignar cita: inserta en el arbol

void SistemaMatricula::asignarCita(long long fechaHora, string carne) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    CitaMatricula* anterior = citas.buscarPorCarne(carne);

    if (anterior != nullptr) {
        cout << "  " << estudiante->nombre << " ya tiene cita el ";
        imprimirFechaHora(anterior->fechaHora);
        cout << ".\n";
        return;
    }

    CitaMatricula cita;

    cita.fechaHora = fechaHora;
    cita.carne = carne;

    if (!citas.insertar(cita)) {
        cout << "  Ya hay una cita asignada el ";
        imprimirFechaHora(fechaHora);
        cout << ".\n";
        return;
    }

    cout << "  Cita asignada: ";
    imprimirCita(cita);
    cout << "\n";
    citas.resumenCambios();
}

// Buscar cita por fecha y hora

void SistemaMatricula::buscarCita(long long fechaHora) {
    citas.mostrarCamino(fechaHora);
    cout << "\n";

    CitaMatricula* cita = citas.buscar(fechaHora);

    if (cita == nullptr) {
        cout << "  No hay cita el ";
        imprimirFechaHora(fechaHora);
        cout << ".\n";
        return;
    }

    cout << "  Cita encontrada: ";
    imprimirCita(*cita);
}

// Consultar la cita de un estudiante

void SistemaMatricula::consultarCitaDe(string carne) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    cout << "  Estudiante: " << estudiante->nombre << "\n";
    cout << "  Correo: " << estudiante->correo << "\n";

    CitaMatricula* cita = citas.buscarPorCarne(carne);

    if (cita == nullptr) {
        cout << "  Cita: no tiene cita asignada.\n";
        return;
    }

    cout << "  Cita: ";
    imprimirFechaHora(cita->fechaHora);
    cout << "\n";
}

// Cancelar cita: elimina del arbol

void SistemaMatricula::cancelarCita(long long fechaHora) {
    CitaMatricula* cita = citas.buscar(fechaHora);

    if (cita == nullptr) {
        cout << "  No hay cita el ";
        imprimirFechaHora(fechaHora);
        cout << ".\n";
        return;
    }

    cout << "  Cita cancelada: ";
    imprimirCita(*cita);

    citas.eliminar(fechaHora);

    cout << "\n";
    citas.resumenCambios();
}

// Agenda y rango

void SistemaMatricula::agendaDeCitas() {
    if (citas.esVacio()) {
        cout << "  No hay citas asignadas.\n";
        return;
    }

    vector<CitaMatricula> lista;
    citas.enOrden(lista);

    imprimirListaDeCitas(lista);
}

void SistemaMatricula::citasEnRango(long long desde, long long hasta) {
    if (desde > hasta) {
        cout << "  El inicio del rango no puede ser mayor que el final.\n";
        return;
    }

    vector<CitaMatricula> lista;
    citas.rango(desde, hasta, lista);

    cout << "  Citas del ";
    imprimirFechaHora(desde);
    cout << " al ";
    imprimirFechaHora(hasta);
    cout << "\n\n";

    if (lista.size() == 0) {
        cout << "  No hay citas en ese rango.\n";
        return;
    }

    imprimirListaDeCitas(lista);
}

// Validar si el estudiante ya puede matricular

void SistemaMatricula::validarCita(string carne) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    CitaMatricula* cita = citas.buscarPorCarne(carne);

    if (cita == nullptr) {
        cout << "  " << estudiante->nombre << " no tiene cita asignada.\n";
        return;
    }

    cout << "  Hora actual: ";
    imprimirFechaHora(horaActual);
    cout << "\n  Cita de " << estudiante->nombre << ": ";
    imprimirFechaHora(cita->fechaHora);
    cout << "\n\n";

    if (estaHabilitada(*cita, horaActual)) {
        cout << "  La cita ya esta habilitada: puede matricular.\n";
    } else {
        cout << "  La cita todavia no esta habilitada: no puede matricular.\n";
    }
}

// Hora actual simulada

void SistemaMatricula::cambiarHoraActual(long long fechaHora) {
    horaActual = fechaHora;

    cout << "  Hora actual: ";
    imprimirFechaHora(horaActual);
    cout << "\n";
}

void SistemaMatricula::mostrarHoraActual() {
    imprimirFechaHora(horaActual);
}

// Estudiantes con su cita

void SistemaMatricula::listarEstudiantes() {
    if (estudiantes.cantidad() == 0) {
        cout << "  No hay estudiantes registrados.\n";
        return;
    }

    cout << "  Carne       Cita               Estudiante\n";
    cout << "  ------------------------------------------------------\n";

    for (int i = 0; i < estudiantes.cantidad(); i++) {
        Estudiante* estudiante = estudiantes.obtener(i);
        CitaMatricula* cita = citas.buscarPorCarne(estudiante->carne);

        cout << "  " << estudiante->carne << "   ";

        if (cita != nullptr) {
            imprimirFechaHora(cita->fechaHora);
        } else {
            cout << "sin cita        ";
        }

        cout << "   " << estudiante->nombre << "\n";
    }

    cout << "\n  Total: " << estudiantes.cantidad() << " estudiantes\n";
}

// Arbol de citas y sus propiedades

void SistemaMatricula::mostrarArbolDeCitas() {
    if (citas.esVacio()) {
        cout << "  El arbol esta vacio.\n";
        return;
    }

    citas.mostrarArbol();
    cout << "\n";
    citas.verificarPropiedades();
}
