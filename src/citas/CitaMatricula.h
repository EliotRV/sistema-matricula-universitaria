// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Cita de matricula y funciones de fecha y hora

#pragma once

#include <string>

// Fecha y hora como AAAAMMDDHHMM
struct CitaMatricula {
    long long fechaHora;
    std::string carne;
};

long long armarFechaHora(int anio, int mes, int dia, int hora, int minuto);
bool fechaHoraValida(int anio, int mes, int dia, int hora, int minuto);

// Sumar minutos a una fecha y hora
long long sumarMinutos(long long fechaHora, int minutos);

void imprimirFechaHora(long long fechaHora);

// La misma fecha como texto: DD/MM/AAAA HH:MM
std::string textoFechaHora(long long fechaHora);
void imprimirDiaHora(long long fechaHora);

bool estaHabilitada(CitaMatricula cita, long long horaActual);
