// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Cola de mensajes segura entre hilos (productor-consumidor)

#pragma once

#include <mutex>
#include <condition_variable>
#include "mensajeria/Mensaje.h"

struct NodoMensaje {
    Mensaje mensaje;
    NodoMensaje* siguiente;
};

// Cola de mensajes entre hilos
class ColaMensajes : public IPublicador {
public:
    ColaMensajes();
    ~ColaMensajes();

    ColaMensajes(const ColaMensajes&) = delete;
    ColaMensajes& operator=(const ColaMensajes&) = delete;

    // Productor: encola y avisa
    void publicar(Mensaje mensaje) override;

    // Consumidor: espera un mensaje
    bool esperarYSacar(Mensaje& mensaje);

    // Cerrar la cola
    void cerrar();

    int cantidad();
    bool estaVacia();

private:
    NodoMensaje* primero;
    NodoMensaje* ultimo;
    int total;
    bool cerrada;

    std::mutex candado;
    std::condition_variable hayMensajes;
};
