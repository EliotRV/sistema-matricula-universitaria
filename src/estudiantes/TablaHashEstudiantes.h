// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Tabla hash dinamica de estudiantes por carne (hashing extensible)

#pragma once

#include <string>
#include <vector>
#include "estudiantes/Estudiante.h"

// Cada cubeta guarda pocos estudiantes; al llenarse se divide en dos
const int CAPACIDAD_CUBETA = 2;

// Limite del directorio: 2^20 entradas
const int PROFUNDIDAD_MAXIMA = 20;

struct Cubeta {
    int profundidadLocal;
    int cantidad;
    Estudiante registros[CAPACIDAD_CUBETA];
};

// El directorio es un arreglo de 2^profundidadGlobal punteros a cubetas.
// Varias entradas pueden apuntar a la misma cubeta: una cubeta con
// profundidad local p la comparten 2^(global - p) entradas.
// La entrada de un carne son los ultimos bits de su valor hash.
class TablaHashEstudiantes {
public:
    TablaHashEstudiantes();
    ~TablaHashEstudiantes();

    TablaHashEstudiantes(const TablaHashEstudiantes&) = delete;
    TablaHashEstudiantes& operator=(const TablaHashEstudiantes&) = delete;

    // Operaciones principales
    // Los punteros que devuelve buscar dejan de servir al insertar o eliminar
    bool insertar(Estudiante estudiante);
    Estudiante* buscar(std::string carne);
    bool modificar(std::string carne, std::string nombre, std::string correo);
    bool eliminar(std::string carne);

    // Todos los estudiantes ordenados por carne
    void listar(std::vector<Estudiante>& salida);

    // Vista interna
    void mostrarDirectorio();
    void mostrarBusqueda(std::string carne);
    void resumenCambios();

    // Datos de la tabla
    int cantidad();
    int profundidadGlobal();
    int tamanoDirectorio();
    int contarCubetas();
    double factorDeCarga();
    bool estructuraCorrecta();

    bool esVacia();
    void vaciar();

    // Valor hash del carne: sus digitos como numero (2026-0105 -> 20260105)
    static unsigned long long valorHash(std::string carne);

private:
    Cubeta** directorio;
    int profundidad;
    int total;

    // Cambios de la ultima insercion o eliminacion
    int divisiones;
    int duplicaciones;
    int uniones;
    int reducciones;

    Cubeta* crearCubeta(int profundidadLocal);
    int indiceDe(std::string carne);
    int posicionEnCubeta(Cubeta* cubeta, std::string carne);

    void duplicarDirectorio();
    bool dividirCubeta(int indice);
    void unirCubetas(int indice);
    void reducirDirectorio();
    void reiniciarCambios();

    void imprimirBits(int valor, int bits);
    void liberar();
};
