// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Cita de matricula y funciones de fecha y hora

#pragma once

#include <string>

// La fecha y hora se guarda como un numero AAAAMMDDHHMM
struct CitaMatricula {
    long long fechaHora;
    std::string carne;
};

long long armarFechaHora(int anio, int mes, int dia, int hora, int minuto);
bool fechaHoraValida(int anio, int mes, int dia, int hora, int minuto);

void imprimirFechaHora(long long fechaHora);
void imprimirDiaHora(long long fechaHora);

bool estaHabilitada(CitaMatricula cita, long long horaActual);
