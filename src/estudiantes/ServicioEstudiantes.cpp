// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del componente de estudiantes

#include <iostream>
#include "estudiantes/ServicioEstudiantes.h"
#include "citas/CitaMatricula.h"
#include "comun/Consola.h"

using namespace std;

bool ServicioEstudiantes::cargar(Estudiante estudiante) {
    estudiante.cita = 0;
    return carneValido(estudiante.carne) && tabla.insertar(estudiante);
}

// Registro (RN-09: el carne es unico)

bool ServicioEstudiantes::registrar(Estudiante estudiante) {
    if (!carneValido(estudiante.carne)) {
        cout << "  El carne va con el formato AAAA-NNNN.\n";
        return false;
    }

    if (estudiante.nombre == "") {
        cout << "  Falta el nombre.\n";
        return false;
    }

    estudiante.cita = 0;

    if (!tabla.insertar(estudiante)) {
        cout << "  Ya existe un estudiante con el carne " << estudiante.carne << ".\n";
        return false;
    }

    cout << "  Estudiante registrado: " << estudiante.carne << "  " << estudiante.nombre << "\n\n";
    tabla.resumenCambios();

    return true;
}

bool ServicioEstudiantes::modificar(string carne, string nombre, string correo) {
    if (nombre == "") {
        cout << "  Falta el nombre.\n";
        return false;
    }

    if (!tabla.modificar(carne, nombre, correo)) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return false;
    }

    cout << "  Datos actualizados.\n";
    return true;
}

bool ServicioEstudiantes::eliminar(string carne) {
    if (!tabla.eliminar(carne)) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return false;
    }

    cout << "  Estudiante eliminado.\n\n";
    tabla.resumenCambios();

    return true;
}

// Consultas

void ServicioEstudiantes::mostrar(string carne) {
    tabla.mostrarBusqueda(carne);
    cout << "\n";

    Estudiante* estudiante = tabla.buscar(carne);

    if (estudiante == nullptr) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    cout << "  Carne:  " << estudiante->carne << "\n";
    cout << "  Nombre: " << estudiante->nombre << "\n";
    cout << "  Correo: " << estudiante->correo << "\n";
    cout << "  Cita:   ";

    if (estudiante->cita == 0) {
        cout << "sin cita";
    } else {
        imprimirFechaHora(estudiante->cita);
    }

    cout << "\n";
}

void ServicioEstudiantes::listar() {
    vector<Estudiante> lista;
    tabla.listar(lista);

    if (lista.size() == 0) {
        cout << "  No hay estudiantes registrados.\n";
        return;
    }

    cout << "  Carne       Cita               Nombre\n";
    cout << "  ----------------------------------------------------------\n";

    for (int i = 0; i < (int)lista.size(); i++) {
        cout << "  " << lista[i].carne << "   ";

        if (lista[i].cita == 0) {
            columna("sin cita", 16);
        } else {
            imprimirFechaHora(lista[i].cita);
        }

        cout << "   " << lista[i].nombre << "\n";
    }

    cout << "\n  Total: " << lista.size() << " estudiantes\n";
}

void ServicioEstudiantes::mostrarTabla() {
    tabla.mostrarDirectorio();
}

Estudiante* ServicioEstudiantes::buscar(string carne) {
    return tabla.buscar(carne);
}

bool ServicioEstudiantes::existe(string carne) {
    return tabla.buscar(carne) != nullptr;
}

string ServicioEstudiantes::nombreDe(string carne) {
    Estudiante* estudiante = tabla.buscar(carne);

    if (estudiante == nullptr) {
        return "";
    }

    return estudiante->nombre;
}

void ServicioEstudiantes::todos(vector<Estudiante>& salida) {
    tabla.listar(salida);
}

void ServicioEstudiantes::ponerCita(string carne, long long fechaHora) {
    Estudiante* estudiante = tabla.buscar(carne);

    if (estudiante != nullptr) {
        estudiante->cita = fechaHora;
    }
}

void ServicioEstudiantes::quitarCita(string carne) {
    ponerCita(carne, 0);
}

int ServicioEstudiantes::cantidad() {
    return tabla.cantidad();
}

void ServicioEstudiantes::vaciar() {
    tabla.vaciar();
}
