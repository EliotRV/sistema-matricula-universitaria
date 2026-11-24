// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Documento plano de la base de datos: pares campo -> valor (todo como texto)

#pragma once

#include <string>
#include <vector>

// Todos los valores se guardan como texto
class Documento {
public:
    void poner(std::string campo, std::string valor);
    std::string valor(std::string campo) const;
    bool tiene(std::string campo) const;

    int cantidad() const;
    std::string campo(int posicion) const;
    std::string valorEn(int posicion) const;

    // {"campo":"valor",...}
    std::string aJson() const;

private:
    std::vector<std::string> campos;
    std::vector<std::string> valores;
};

// Escapar texto para JSON
std::string escaparJson(std::string texto);
