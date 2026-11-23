// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Estudiante y lista enlazada de estudiantes

#pragma once

#include <string>

struct Estudiante {
    std::string carne;
    std::string nombre;
    std::string correo;
};

// Nodo de la lista
struct NodoEstudiante {
    Estudiante estudiante;
    NodoEstudiante* siguiente;
};

class ListaEstudiantes {
public:
    ListaEstudiantes();
    ~ListaEstudiantes();

    ListaEstudiantes(const ListaEstudiantes&) = delete;
    ListaEstudiantes& operator=(const ListaEstudiantes&) = delete;

    bool agregar(Estudiante estudiante);
    Estudiante* buscar(std::string carne);
    Estudiante* obtener(int posicion);

    int cantidad();
    void vaciar();

private:
    NodoEstudiante* primero;
    NodoEstudiante* ultimo;
    int total;
};
