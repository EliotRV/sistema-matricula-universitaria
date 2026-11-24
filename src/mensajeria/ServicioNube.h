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

// El menu nunca espera a la red: los componentes publican en la cola y
// este servicio la vacia en segundo plano. Los mensajes que fallan
// (por ejemplo sin internet) se guardan para reintentarlos despues.
class ServicioNube {
public:
    ServicioNube(ColaMensajes& cola, ClienteNube& cliente);
    ~ServicioNube();

    ServicioNube(const ServicioNube&) = delete;
    ServicioNube& operator=(const ServicioNube&) = delete;

    void iniciar();

    // Termina de enviar lo que quede en la cola y cierra el hilo
    void detener();

    // Espera hasta que la cola este vacia (o hasta el limite de segundos)
    bool esperarVacia(int segundos);

    // Vuelve a poner en la cola los mensajes que fallaron
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

// Fecha y hora real del equipo, AAAA-MM-DD HH:MM:SS
std::string fechaHoraDelEquipo();
