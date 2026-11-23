// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de la lista enlazada de estudiantes

#include "Estudiante.h"

using namespace std;

// Constructor y destructor

ListaEstudiantes::ListaEstudiantes() {
    primero = nullptr;
    ultimo = nullptr;
    total = 0;
}

ListaEstudiantes::~ListaEstudiantes() {
    vaciar();
}

void ListaEstudiantes::vaciar() {
    NodoEstudiante* actual = primero;

    while (actual != nullptr) {
        NodoEstudiante* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }

    primero = nullptr;
    ultimo = nullptr;
    total = 0;
}

// Agregar al final, false si el carne ya existe

bool ListaEstudiantes::agregar(Estudiante estudiante) {
    if (buscar(estudiante.carne) != nullptr) {
        return false;
    }

    NodoEstudiante* nuevoNodo = new NodoEstudiante;

    nuevoNodo->estudiante = estudiante;
    nuevoNodo->siguiente = nullptr;

    if (primero == nullptr) {
        primero = nuevoNodo;
    } else {
        ultimo->siguiente = nuevoNodo;
    }

    ultimo = nuevoNodo;
    total++;

    return true;
}

// Buscar por carne, nullptr si no existe

Estudiante* ListaEstudiantes::buscar(string carne) {
    NodoEstudiante* actual = primero;

    while (actual != nullptr) {
        if (actual->estudiante.carne == carne) {
            return &actual->estudiante;
        }

        actual = actual->siguiente;
    }

    return nullptr;
}

// Estudiante en una posicion, la primera es 0

Estudiante* ListaEstudiantes::obtener(int posicion) {
    NodoEstudiante* actual = primero;
    int i = 0;

    while (actual != nullptr) {
        if (i == posicion) {
            return &actual->estudiante;
        }

        actual = actual->siguiente;
        i++;
    }

    return nullptr;
}

int ListaEstudiantes::cantidad() {
    return total;
}
