// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Servicio que atiende la cola de mensajes en su propio hilo y los lleva a la nube

#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "mensajeria/ColaMensajes.h"
#include "nube/ClienteNube.h"

// Envia a la nube los mensajes de la cola
class ServicioNube {
public:
    ServicioNube(ColaMensajes& cola, ClienteNube& cliente);
    ~ServicioNube();

    ServicioNube(const ServicioNube&) = delete;
    ServicioNube& operator=(const ServicioNube&) = delete;

    void iniciar();

    // Detener el servicio
    void detener();

    // Esperar a que la cola quede vacia
    bool esperarVacia(int segundos);

    // Reintentar los mensajes fallidos
    int reintentarFallidos();

    void mostrarEstado();
    void mostrarBitacora();

    int enviados();
    int fallidos();
    int pendientes();

private:
    ColaMensajes& cola;
    ClienteNube& cliente;

    std::thread hilo;
    std::mutex candado;
    bool corriendo;
    bool procesando;

    int totalEnviados;
    std::vector<Mensaje> listaFallidos;
    std::vector<std::string> bitacora;
    std::string ultimoError;

    void trabajar();
    bool procesar(Mensaje& mensaje);
    void anotar(std::string linea);
};

// Fecha y hora del equipo
std::string fechaHoraDelEquipo();
