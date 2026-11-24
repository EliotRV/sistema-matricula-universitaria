// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Mensaje entre componentes y la interfaz para publicarlos

#pragma once

#include <string>
#include "comun/Documento.h"

enum TipoMensaje {
    MENSAJE_INSERTAR,
    MENSAJE_ACTUALIZAR,
    MENSAJE_ELIMINAR,
    MENSAJE_NOTIFICAR
};

// Cambio para la base de datos o notificacion
struct Mensaje {
    TipoMensaje tipo;
    std::string coleccion;
    Documento filtro;
    Documento datos;
    std::string descripcion;
};

// Destino de los mensajes
class IPublicador {
public:
    virtual ~IPublicador() {}
    virtual void publicar(Mensaje mensaje) = 0;
};

// Base de los componentes que publican cambios
class EmisorDeMensajes {
public:
    void conectar(IPublicador* destino) {
        this->destino = destino;
    }

protected:
    void emitir(Mensaje mensaje) {
        if (destino != nullptr) {
            destino->publicar(mensaje);
        }
    }

private:
    IPublicador* destino = nullptr;
};

// Arman los mensajes mas comunes
Mensaje mensajeInsertar(std::string coleccion, Documento datos, std::string descripcion);
Mensaje mensajeActualizar(std::string coleccion, Documento filtro, Documento datos, std::string descripcion);
Mensaje mensajeEliminar(std::string coleccion, Documento filtro, std::string descripcion);
Mensaje mensajeNotificar(std::string carne, std::string texto);

std::string nombreDeTipo(TipoMensaje tipo);
