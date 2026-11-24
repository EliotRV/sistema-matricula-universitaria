// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Funciones para armar mensajes

#include "mensajeria/Mensaje.h"
#include "comun/Colecciones.h"

using namespace std;

Mensaje mensajeInsertar(string coleccion, Documento datos, string descripcion) {
    Mensaje mensaje;

    mensaje.tipo = MENSAJE_INSERTAR;
    mensaje.coleccion = coleccion;
    mensaje.datos = datos;
    mensaje.descripcion = descripcion;

    return mensaje;
}

Mensaje mensajeActualizar(string coleccion, Documento filtro, Documento datos, string descripcion) {
    Mensaje mensaje;

    mensaje.tipo = MENSAJE_ACTUALIZAR;
    mensaje.coleccion = coleccion;
    mensaje.filtro = filtro;
    mensaje.datos = datos;
    mensaje.descripcion = descripcion;

    return mensaje;
}

Mensaje mensajeEliminar(string coleccion, Documento filtro, string descripcion) {
    Mensaje mensaje;

    mensaje.tipo = MENSAJE_ELIMINAR;
    mensaje.coleccion = coleccion;
    mensaje.filtro = filtro;
    mensaje.descripcion = descripcion;

    return mensaje;
}

// Notificacion para el estudiante
Mensaje mensajeNotificar(string carne, string texto) {
    Mensaje mensaje;

    mensaje.tipo = MENSAJE_NOTIFICAR;
    mensaje.coleccion = COL_NOTIFICACIONES;
    mensaje.datos.poner("carne", carne);
    mensaje.datos.poner("mensaje", texto);
    mensaje.descripcion = "notificar a " + carne + ": " + texto;

    return mensaje;
}

string nombreDeTipo(TipoMensaje tipo) {
    switch (tipo) {
        case MENSAJE_INSERTAR:
            return "insertar";

        case MENSAJE_ACTUALIZAR:
            return "actualizar";

        case MENSAJE_ELIMINAR:
            return "eliminar";

        case MENSAJE_NOTIFICAR:
            return "notificar";
    }

    return "?";
}
