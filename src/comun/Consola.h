// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Lectura segura de datos por consola

#pragma once

#include <string>

// Todas las lecturas usan getline: una letra donde va un numero no rompe
// el programa, solo se vuelve a pedir el dato.
// Si la entrada se cierra (fin de archivo), las lecturas devuelven 0 o ""
// para que los menus terminen solos.

bool entradaCerrada();

int leerEntero(std::string mensaje);
int leerOpcion();
std::string leerTexto(std::string mensaje);
std::string leerCarne();
std::string leerCodigoCurso();
bool leerSiNo(std::string mensaje);

// Pide la fecha (DD/MM/AAAA) y la hora (HH:MM); false si no son validas
bool leerFechaHora(std::string mensaje, long long& fechaHora);

// Pide una hora del dia HH:MM y la deja como HHMM; false si no es valida
bool leerHoraDelDia(std::string mensaje, int& horaHHMM);

// Pide un periodo AAAA-Cn; false si no es valido
bool leerPeriodo(std::string mensaje, std::string& periodo);

void pausar();
void titulo(std::string texto);

// Escribe el texto y lo completa con espacios hasta el ancho
void columna(std::string texto, int ancho);
