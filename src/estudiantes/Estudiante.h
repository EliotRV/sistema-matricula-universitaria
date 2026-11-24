// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Estudiante del sistema

#pragma once

#include <string>

// Estudiante con su cita de matricula
struct Estudiante {
    std::string carne;
    std::string nombre;
    std::string correo;
    long long cita = 0;
};

// Carne con formato AAAA-NNNN
bool carneValido(std::string carne);
