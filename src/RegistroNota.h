// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Registro de nota y manejo de periodos

#pragma once

#include <string>

const int NOTA_MINIMA = 70;

// Nota final de un estudiante en un curso y periodo
struct RegistroNota {
    std::string carne;
    std::string periodo;
    std::string codigoCurso;
    int nota;
};

// Periodo con formato AAAA-Cn, por ejemplo 2026-C3
bool periodoValido(std::string periodo);

// Clave del arbol B+: carne|periodo|curso
// Ordena primero por carne, luego por periodo y luego por curso
std::string armarClaveNota(std::string carne, std::string periodo, std::string codigoCurso);
std::string claveDe(RegistroNota registro);

bool estaAprobado(RegistroNota registro);
