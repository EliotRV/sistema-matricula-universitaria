// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Sistema de matricula: une los estudiantes y el arbol de citas

#pragma once

#include <string>
#include <vector>
#include "estudiantes/Estudiante.h"
#include "citas/CitaMatricula.h"
#include "citas/ArbolRNCitas.h"

class SistemaMatricula {
public:
    SistemaMatricula();

    // Datos iniciales
    void cargarDatos(std::string rutaEstudiantes, std::string rutaCitas);

    // Citas de matricula
    void asignarCita(long long fechaHora, std::string carne);
    void buscarCita(long long fechaHora);
    void consultarCitaDe(std::string carne);
    void cancelarCita(long long fechaHora);
    void agendaDeCitas();
    void citasEnRango(long long desde, long long hasta);
    void validarCita(std::string carne);

    // Hora actual simulada
    void cambiarHoraActual(long long fechaHora);
    void mostrarHoraActual();

    void listarEstudiantes();
    void mostrarArbolDeCitas();

private:
    ListaEstudiantes estudiantes;
    ArbolRNCitas citas;
    long long horaActual;

    void imprimirCita(CitaMatricula cita);
    void imprimirListaDeCitas(std::vector<CitaMatricula>& lista);
};
