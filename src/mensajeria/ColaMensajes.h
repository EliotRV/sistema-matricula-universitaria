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

// Los componentes (productores) publican desde el hilo del menu y el
// servicio de la nube (consumidor) saca desde su propio hilo.
// El mutex protege la lista y la variable de condicion despierta al
// consumidor cuando llega un mensaje, sin que tenga que preguntar a cada rato.
class ColaMensajes : public IPublicador {
public:
    ColaMensajes();
    ~ColaMensajes();

    ColaMensajes(const ColaMensajes&) = delete;
    ColaMensajes& operator=(const ColaMensajes&) = delete;

    // Productor: encola y avisa
    void publicar(Mensaje mensaje) override;

    // Consumidor: espera hasta que haya un mensaje.
    // Devuelve false cuando la cola se cerro y ya no queda nada.
    bool esperarYSacar(Mensaje& mensaje);

    // Despues de cerrar ya no se esperan mensajes nuevos
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
