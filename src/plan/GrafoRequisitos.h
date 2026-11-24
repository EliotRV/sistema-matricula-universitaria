// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Grafo dirigido aciclico del plan de estudios (requisitos entre cursos)

#pragma once

#include <string>
#include <vector>
#include "plan/Curso.h"

struct Vertice;

// Nodo de una lista de adyacencia
struct NodoAdyacencia {
    Vertice* vertice;
    NodoAdyacencia* siguiente;
};

// Curso con sus listas de adyacencia
struct Vertice {
    Curso curso;
    NodoAdyacencia* habilita;
    NodoAdyacencia* requisitos;
    Vertice* siguiente;

    // Apoyo para los recorridos
    bool visitado;
    int pendientes;
};

// Resultado de agregar un requisito
enum ResultadoRequisito {
    REQUISITO_AGREGADO,
    CURSO_NO_EXISTE,
    REQUISITO_REPETIDO,
    MISMO_CURSO,
    FORMA_CICLO
};

class GrafoRequisitos {
public:
    GrafoRequisitos();
    ~GrafoRequisitos();

    GrafoRequisitos(const GrafoRequisitos&) = delete;
    GrafoRequisitos& operator=(const GrafoRequisitos&) = delete;

    // Cursos (vertices)
    bool agregarCurso(Curso curso);
    Curso* buscarCurso(std::string codigo);
    bool modificarCurso(std::string codigo, std::string nombre, int creditos);
    bool eliminarCurso(std::string codigo);

    // Requisitos (aristas): requisito -> curso
    ResultadoRequisito agregarRequisito(std::string requisito, std::string curso);
    bool quitarRequisito(std::string requisito, std::string curso);
    bool esRequisito(std::string requisito, std::string curso);

    // Consultas
    void requisitosDe(std::string codigo, std::vector<std::string>& salida);
    void habilitaA(std::string codigo, std::vector<std::string>& salida);
    void cursos(std::vector<Curso>& salida);
    bool hayCamino(std::string desde, std::string hasta);

    // Orden topologico
    bool ordenTopologico(std::vector<std::string>& salida);

    // Cursos habilitados
    void cursosHabilitados(std::vector<std::string>& aprobados, std::vector<std::string>& salida);

    // Vista interna
    void mostrarListas();
    void mostrarOrden();

    int cantidadCursos();
    int cantidadRequisitos();
    bool esVacio();
    void vaciar();

private:
    Vertice* primero;
    Vertice* ultimo;
    int totalCursos;
    int totalRequisitos;

    Vertice* buscarVertice(std::string codigo);

    void agregarAlFinal(NodoAdyacencia*& lista, Vertice* vertice);
    bool quitarDeLista(NodoAdyacencia*& lista, Vertice* vertice);
    bool estaEnLista(NodoAdyacencia* lista, Vertice* vertice);
    void liberarLista(NodoAdyacencia*& lista);

    void desmarcar();
    bool buscarCamino(Vertice* actual, Vertice* destino);

    bool contiene(std::vector<std::string>& lista, std::string codigo);
    void imprimirLista(NodoAdyacencia* lista);
};
