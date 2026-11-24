// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Registro de nota y manejo de periodos

#pragma once

#include <string>

const int NOTA_MINIMA = 70;

// Nota de un estudiante
struct RegistroNota {
    std::string carne;
    std::string periodo;
    std::string codigoCurso;
    int nota;
};

// Periodo AAAA-Cn
bool periodoValido(std::string periodo);

// Clave del arbol B+: carne|periodo|curso
std::string armarClaveNota(std::string carne, std::string periodo, std::string codigoCurso);
std::string claveDe(RegistroNota registro);

bool estaAprobado(RegistroNota registro);
