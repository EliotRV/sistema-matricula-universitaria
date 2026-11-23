// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Curso del plan de estudios

#pragma once

#include <string>

// El codigo es la clave del curso en el grafo de requisitos
struct Curso {
    std::string codigo;
    std::string nombre;
    int creditos;
};
