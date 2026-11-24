// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del componente de citas

#include <iostream>
#include "citas/ServicioCitas.h"
#include "comun/Colecciones.h"

using namespace std;

ServicioCitas::ServicioCitas(ServicioEstudiantes& estudiantes) : estudiantes(estudiantes) {
}

// Una cita por estudiante y por hora
ResultadoCita ServicioCitas::cargar(CitaMatricula cita) {
    Estudiante* estudiante = estudiantes.buscar(cita.carne);

    if (estudiante == nullptr) {
        return CITA_SIN_ESTUDIANTE;
    }

    if (estudiante->cita != 0) {
        return CITA_YA_TIENE;
    }

    if (!arbol.insertar(cita)) {
        return CITA_HORA_OCUPADA;
    }

    estudiante->cita = cita.fechaHora;
    return CITA_ASIGNADA;
}

// Imprimir

void ServicioCitas::imprimirCita(CitaMatricula cita) {
    imprimirFechaHora(cita.fechaHora);
    cout << "   " << cita.carne << "   " << estudiantes.nombreDe(cita.carne) << "\n";
}

void ServicioCitas::imprimirLista(vector<CitaMatricula>& lista) {
    cout << "  Fecha y hora       Carne       Estudiante\n";
    cout << "  ----------------------------------------------------------\n";

    for (int i = 0; i < (int)lista.size(); i++) {
        cout << "  ";
        imprimirCita(lista[i]);
    }

    cout << "\n  Total: " << lista.size() << " citas\n";
}

// Asignar (insertar en el arbol)

bool ServicioCitas::asignar(long long fechaHora, string carne) {
    CitaMatricula cita;

    cita.fechaHora = fechaHora;
    cita.carne = carne;

    Estudiante* estudiante = estudiantes.buscar(carne);
    long long anterior = 0;

    if (estudiante != nullptr) {
        anterior = estudiante->cita;
    }

    switch (cargar(cita)) {
        case CITA_SIN_ESTUDIANTE:
            cout << "  No existe un estudiante con el carne " << carne << ".\n";
            return false;

        case CITA_YA_TIENE:
            cout << "  " << estudiante->nombre << " ya tiene cita el ";
            imprimirFechaHora(anterior);
            cout << ". Use reprogramar.\n";
            return false;

        case CITA_HORA_OCUPADA:
            cout << "  Ya hay una cita asignada el ";
            imprimirFechaHora(fechaHora);
            cout << ".\n";
            return false;

        case CITA_ASIGNADA:
            break;
    }

    emitir(mensajeInsertar(COL_CITAS, documentoDe(cita), "asignar cita " + carne));
    emitir(mensajeNotificar(carne, "Su cita de matricula es el " + textoFechaHora(fechaHora)));

    cout << "  Cita asignada: ";
    imprimirCita(cita);
    cout << "\n";
    arbol.resumenCambios();

    return true;
}

// Reprogramar (modificar la hora)

bool ServicioCitas::reprogramar(string carne, long long nuevaFechaHora) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return false;
    }

    if (estudiante->cita == 0) {
        cout << "  " << estudiante->nombre << " no tiene cita. Use asignar.\n";
        return false;
    }

    long long anterior = estudiante->cita;

    if (!arbol.reprogramar(anterior, nuevaFechaHora)) {
        cout << "  Ya hay una cita asignada el ";
        imprimirFechaHora(nuevaFechaHora);
        cout << ".\n";
        return false;
    }

    estudiante->cita = nuevaFechaHora;

    Documento datos;
    datos.poner("fecha", textoFecha(nuevaFechaHora));
    datos.poner("hora", textoHora(nuevaFechaHora));
    emitir(mensajeActualizar(COL_CITAS, filtroCita(carne), datos, "reprogramar cita " + carne));
    emitir(mensajeNotificar(carne, "Su cita de matricula cambio al " + textoFechaHora(nuevaFechaHora)));

    cout << "  Cita de " << estudiante->nombre << ": ";
    imprimirFechaHora(anterior);
    cout << " -> ";
    imprimirFechaHora(nuevaFechaHora);
    cout << "\n\n";
    arbol.resumenCambios();

    return true;
}

// Cancelar (eliminar del arbol)

bool ServicioCitas::cancelar(long long fechaHora) {
    CitaMatricula* cita = arbol.buscar(fechaHora);

    if (cita == nullptr) {
        cout << "  No hay cita el ";
        imprimirFechaHora(fechaHora);
        cout << ".\n";
        return false;
    }

    cout << "  Cita cancelada: ";
    imprimirCita(*cita);

    string carne = cita->carne;

    estudiantes.quitarCita(carne);
    arbol.eliminar(fechaHora);

    emitir(mensajeEliminar(COL_CITAS, filtroCita(carne), "cancelar cita " + carne));
    emitir(mensajeNotificar(carne, "Su cita de matricula fue cancelada"));

    cout << "\n";
    arbol.resumenCambios();

    return true;
}

bool ServicioCitas::cancelarDe(string carne) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr || estudiante->cita == 0) {
        return false;
    }

    arbol.eliminar(estudiante->cita);
    estudiante->cita = 0;

    emitir(mensajeEliminar(COL_CITAS, filtroCita(carne), "cancelar cita " + carne));

    return true;
}

// Consultas

void ServicioCitas::buscarPorHora(long long fechaHora) {
    arbol.mostrarCamino(fechaHora);
    cout << "\n";

    CitaMatricula* cita = arbol.buscar(fechaHora);

    if (cita == nullptr) {
        cout << "  No hay cita el ";
        imprimirFechaHora(fechaHora);
        cout << ".\n";
        return;
    }

    cout << "  Cita encontrada: ";
    imprimirCita(*cita);
}

// Buscar la cita desde el carne
void ServicioCitas::consultarDe(string carne) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    cout << "  Estudiante: " << estudiante->nombre << "\n";

    if (estudiante->cita == 0 || arbol.buscar(estudiante->cita) == nullptr) {
        cout << "  Cita: no tiene cita asignada.\n";
        return;
    }

    cout << "  Cita: ";
    imprimirFechaHora(estudiante->cita);
    cout << "\n";
}

void ServicioCitas::agenda() {
    if (arbol.esVacio()) {
        cout << "  No hay citas asignadas.\n";
        return;
    }

    vector<CitaMatricula> lista;
    arbol.enOrden(lista);

    imprimirLista(lista);
}

void ServicioCitas::rango(long long desde, long long hasta) {
    if (desde > hasta) {
        cout << "  El inicio del rango no puede ser mayor que el final.\n";
        return;
    }

    vector<CitaMatricula> lista;
    arbol.rango(desde, hasta, lista);

    cout << "  Citas del ";
    imprimirFechaHora(desde);
    cout << " al ";
    imprimirFechaHora(hasta);
    cout << "\n\n";

    if (lista.size() == 0) {
        cout << "  No hay citas en ese rango.\n";
        return;
    }

    imprimirLista(lista);
}

// RN-01: cita habilitada
bool ServicioCitas::habilitada(string carne, long long horaActual) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    return estudiante != nullptr && estudiante->cita != 0 && estudiante->cita <= horaActual;
}

void ServicioCitas::validar(string carne, long long horaActual) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    if (estudiante->cita == 0) {
        cout << "  " << estudiante->nombre << " no tiene cita asignada.\n";
        return;
    }

    cout << "  Hora actual: ";
    imprimirFechaHora(horaActual);
    cout << "\n  Cita de " << estudiante->nombre << ": ";
    imprimirFechaHora(estudiante->cita);
    cout << "\n\n";

    if (habilitada(carne, horaActual)) {
        cout << "  La cita ya esta habilitada: puede matricular.\n";
    } else {
        cout << "  La cita todavia no esta habilitada: no puede matricular.\n";
    }
}

void ServicioCitas::mostrarArbol() {
    if (arbol.esVacio()) {
        cout << "  El arbol esta vacio.\n";
        return;
    }

    arbol.mostrarArbol();
    cout << "\n";
    arbol.verificarPropiedades();
}

void ServicioCitas::todas(vector<CitaMatricula>& salida) {
    arbol.enOrden(salida);
}

int ServicioCitas::cantidad() {
    return arbol.contarNodos();
}

void ServicioCitas::vaciar() {
    vector<CitaMatricula> lista;
    arbol.enOrden(lista);

    for (int i = 0; i < (int)lista.size(); i++) {
        estudiantes.quitarCita(lista[i].carne);
    }

    arbol.vaciar();
}
