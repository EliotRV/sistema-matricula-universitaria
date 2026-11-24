// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Lectura segura de datos por consola

#pragma once

#include <string>

// Lectura de datos desde la consola

bool entradaCerrada();

int leerEntero(std::string mensaje);
int leerOpcion();
std::string leerTexto(std::string mensaje);
std::string leerCarne();
std::string leerCodigoCurso();
bool leerSiNo(std::string mensaje);

// Pedir fecha y hora
bool leerFechaHora(std::string mensaje, long long& fechaHora);

// Pedir una hora HH:MM
bool leerHoraDelDia(std::string mensaje, int& horaHHMM);

// Pide un periodo AAAA-Cn; false si no es valido
bool leerPeriodo(std::string mensaje, std::string& periodo);

void pausar();
void titulo(std::string texto);

// Texto con ancho fijo
void columna(std::string texto, int ancho);
