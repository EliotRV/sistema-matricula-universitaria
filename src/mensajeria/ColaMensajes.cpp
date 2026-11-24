// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de la cola de mensajes

#include "mensajeria/ColaMensajes.h"

using namespace std;

ColaMensajes::ColaMensajes() {
    primero = nullptr;
    ultimo = nullptr;
    total = 0;
    cerrada = false;
}

ColaMensajes::~ColaMensajes() {
    while (primero != nullptr) {
        NodoMensaje* siguiente = primero->siguiente;
        delete primero;
        primero = siguiente;
    }
}

void ColaMensajes::publicar(Mensaje mensaje) {
    NodoMensaje* nuevo = new NodoMensaje;

    nuevo->mensaje = mensaje;
    nuevo->siguiente = nullptr;

    {
        lock_guard<mutex> bloqueo(candado);

        if (ultimo == nullptr) {
            primero = nuevo;
        } else {
            ultimo->siguiente = nuevo;
        }

        ultimo = nuevo;
        total++;
    }

    // Despierta al consumidor si estaba esperando
    hayMensajes.notify_one();
}

bool ColaMensajes::esperarYSacar(Mensaje& mensaje) {
    unique_lock<mutex> bloqueo(candado);

    // wait suelta el mutex mientras duerme y lo vuelve a tomar al despertar
    hayMensajes.wait(bloqueo, [this] { return primero != nullptr || cerrada; });

    if (primero == nullptr) {
        return false;
    }

    NodoMensaje* sale = primero;

    mensaje = sale->mensaje;
    primero = sale->siguiente;

    if (primero == nullptr) {
        ultimo = nullptr;
    }

    total--;
    delete sale;

    return true;
}

void ColaMensajes::cerrar() {
    {
        lock_guard<mutex> bloqueo(candado);
        cerrada = true;
    }

    hayMensajes.notify_all();
}

int ColaMensajes::cantidad() {
    lock_guard<mutex> bloqueo(candado);
    return total;
}

bool ColaMensajes::estaVacia() {
    return cantidad() == 0;
}
