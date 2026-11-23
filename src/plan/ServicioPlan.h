// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Componente del plan de estudios: cursos y requisitos sobre el grafo

#pragma once

#include <string>
#include <vector>
#include "plan/GrafoRequisitos.h"

class ServicioPlan {
public:
    // Sin mensajes: para la carga de datos
    bool cargarCurso(Curso curso);
    ResultadoRequisito cargarRequisito(std::string requisito, std::string curso);

    // Con mensajes: para el menu
    bool agregarCurso(Curso curso);
    bool modificarCurso(std::string codigo, std::string nombre, int creditos);
    bool eliminarCurso(std::string codigo);
    bool agregarRequisito(std::string requisito, std::string curso);
    bool quitarRequisito(std::string requisito, std::string curso);

    void mostrarPlan();
    void mostrarListas();
    void mostrarCurso(std::string codigo);

    // Consultas para los demas componentes
    bool existe(std::string codigo);
    Curso* buscar(std::string codigo);
    std::string nombreDe(std::string codigo);
    void requisitosFaltantes(std::string codigo, std::vector<std::string>& aprobados, std::vector<std::string>& faltan);
    void habilitados(std::vector<std::string>& aprobados, std::vector<std::string>& salida);
    void cursos(std::vector<Curso>& salida);
    void requisitos(std::vector<std::string>& origen, std::vector<std::string>& destino);

    int cantidadCursos();
    int cantidadRequisitos();
    void vaciar();

private:
    GrafoRequisitos grafo;
};
