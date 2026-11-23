// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Matricula de un estudiante en un grupo

#pragma once

#include <string>

// Una matricula retirada se conserva con activa = false
struct Matricula {
    std::string carne;
    std::string codigoCurso;
    int numeroGrupo;
    std::string periodo;
    bool activa;
};
