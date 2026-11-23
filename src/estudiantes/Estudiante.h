// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Estudiante del sistema

#pragma once

#include <string>

// El carne es la clave en la tabla hash.
// cita guarda la fecha y hora de su cita de matricula (0 si no tiene):
// asi se llega a la cita desde el carne sin recorrer el arbol de citas.
struct Estudiante {
    std::string carne;
    std::string nombre;
    std::string correo;
    long long cita = 0;
};

// Carne con formato AAAA-NNNN
bool carneValido(std::string carne);
