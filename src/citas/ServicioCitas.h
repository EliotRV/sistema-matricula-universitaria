// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Componente de citas de matricula sobre el arbol rojo-negro

#pragma once

#include <string>
#include <vector>
#include "citas/ArbolRNCitas.h"
#include "estudiantes/ServicioEstudiantes.h"
#include "mensajeria/Mensaje.h"

enum ResultadoCita {
    CITA_ASIGNADA,
    CITA_SIN_ESTUDIANTE,
    CITA_YA_TIENE,
    CITA_HORA_OCUPADA
};

class ServicioCitas : public EmisorDeMensajes {
public:
    ServicioCitas(ServicioEstudiantes& estudiantes);

    // Sin mensajes: para la carga de datos
    ResultadoCita cargar(CitaMatricula cita);

    // Con mensajes: para el menu
    bool asignar(long long fechaHora, std::string carne);
    bool reprogramar(std::string carne, long long nuevaFechaHora);
    bool cancelar(long long fechaHora);

    void buscarPorHora(long long fechaHora);
    void consultarDe(std::string carne);
    void agenda();
    void rango(long long desde, long long hasta);
    void validar(std::string carne, long long horaActual);
    void mostrarArbol();

    // Consultas para los demas componentes
    bool habilitada(std::string carne, long long horaActual);
    bool cancelarDe(std::string carne);
    void todas(std::vector<CitaMatricula>& salida);

    int cantidad();

    // Borrar todas las citas
    void vaciar();

private:
    ArbolRNCitas arbol;
    ServicioEstudiantes& estudiantes;

    void imprimirCita(CitaMatricula cita);
    void imprimirLista(std::vector<CitaMatricula>& lista);
};
