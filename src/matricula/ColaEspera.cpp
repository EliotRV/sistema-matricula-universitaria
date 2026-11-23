// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de la cola de espera

#include <iostream>
#include "matricula/ColaEspera.h"

using namespace std;

ColaEspera::ColaEspera() {
    primero = nullptr;
    ultimo = nullptr;
    total = 0;
}

ColaEspera::~ColaEspera() {
    vaciar();
}

void ColaEspera::vaciar() {
    while (primero != nullptr) {
        NodoCola* siguiente = primero->siguiente;
        delete primero;
        primero = siguiente;
    }

    ultimo = nullptr;
    total = 0;
}

// Se entra por el final
bool ColaEspera::encolar(string carne) {
    if (carne == "" || contiene(carne)) {
        return false;
    }

    NodoCola* nuevo = new NodoCola;

    nuevo->carne = carne;
    nuevo->siguiente = nullptr;

    if (ultimo == nullptr) {
        primero = nuevo;
    } else {
        ultimo->siguiente = nuevo;
    }

    ultimo = nuevo;
    total++;

    return true;
}

// Se sale por el frente
bool ColaEspera::desencolar(string& carne) {
    if (primero == nullptr) {
        return false;
    }

    NodoCola* sale = primero;

    carne = sale->carne;
    primero = sale->siguiente;

    if (primero == nullptr) {
        ultimo = nullptr;
    }

    delete sale;
    total--;

    return true;
}

bool ColaEspera::sacar(string carne) {
    NodoCola* anterior = nullptr;
    NodoCola* actual = primero;

    while (actual != nullptr && actual->carne != carne) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == nullptr) {
        return false;
    }

    if (anterior == nullptr) {
        primero = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    if (ultimo == actual) {
        ultimo = anterior;
    }

    delete actual;
    total--;

    return true;
}

string ColaEspera::frente() {
    if (primero == nullptr) {
        return "";
    }

    return primero->carne;
}

bool ColaEspera::contiene(string carne) {
    return posicionDe(carne) > 0;
}

int ColaEspera::posicionDe(string carne) {
    int posicion = 1;

    for (NodoCola* actual = primero; actual != nullptr; actual = actual->siguiente) {
        if (actual->carne == carne) {
            return posicion;
        }

        posicion++;
    }

    return 0;
}

void ColaEspera::listar(vector<string>& salida) {
    for (NodoCola* actual = primero; actual != nullptr; actual = actual->siguiente) {
        salida.push_back(actual->carne);
    }
}

// Frente -> ... -> final
void ColaEspera::mostrar() {
    if (primero == nullptr) {
        cout << "(vacia)";
        return;
    }

    cout << "frente -> ";

    for (NodoCola* actual = primero; actual != nullptr; actual = actual->siguiente) {
        cout << actual->carne;

        if (actual->siguiente != nullptr) {
            cout << " -> ";
        }
    }

    cout << " <- final";
}

int ColaEspera::cantidad() {
    return total;
}

bool ColaEspera::esVacia() {
    return total == 0;
}
