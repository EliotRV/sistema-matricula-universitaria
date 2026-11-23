// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Arbol rojo-negro de citas de matricula, ordenado por fecha y hora

#pragma once

#include <string>
#include <vector>
#include "citas/CitaMatricula.h"

// Nodo
struct NodoRN {
    CitaMatricula cita;
    bool rojo;

    NodoRN* izquierdo;
    NodoRN* derecho;
    NodoRN* padre;
};

class ArbolRNCitas {
public:
    ArbolRNCitas();
    ~ArbolRNCitas();

    ArbolRNCitas(const ArbolRNCitas&) = delete;
    ArbolRNCitas& operator=(const ArbolRNCitas&) = delete;

    // Operaciones principales
    bool insertar(CitaMatricula cita);
    CitaMatricula* buscar(long long fechaHora);
    CitaMatricula* buscarPorCarne(std::string carne);
    bool eliminar(long long fechaHora);

    // Cambia la hora de una cita; false si no existe o la hora nueva esta ocupada
    bool reprogramar(long long fechaHora, long long nuevaFechaHora);

    // Recorridos
    void enOrden(std::vector<CitaMatricula>& salida);
    void rango(long long desde, long long hasta, std::vector<CitaMatricula>& salida);

    void mostrarArbol();
    void mostrarCamino(long long fechaHora);
    void resumenCambios();

    // Propiedades
    bool propiedadesCorrectas();
    void verificarPropiedades();

    bool esVacio();
    int contarNodos();
    int altura();
    void vaciar();

private:
    NodoRN* raiz;

    // Cambios de la ultima insercion o eliminacion
    int rotacionesIzquierda;
    int rotacionesDerecha;
    int recoloraciones;

    NodoRN* crearNodo(CitaMatricula cita);
    NodoRN* buscarNodo(long long fechaHora);
    NodoRN* buscarPorCarne(NodoRN* nodo, std::string carne);
    NodoRN* minimo(NodoRN* nodo);

    bool esRojo(NodoRN* nodo);
    void pintar(NodoRN* nodo, bool rojo);
    void reiniciarCambios();

    void rotarIzquierda(NodoRN* nodo);
    void rotarDerecha(NodoRN* nodo);
    void reemplazar(NodoRN* viejo, NodoRN* nuevo);

    void corregirInsercion(NodoRN* nodo);
    void corregirEliminacion(NodoRN* nodo, NodoRN* padre);

    void enOrden(NodoRN* nodo, std::vector<CitaMatricula>& salida);
    void rango(NodoRN* nodo, long long desde, long long hasta, std::vector<CitaMatricula>& salida);
    void mostrarNodo(NodoRN* nodo, int nivel);
    void imprimirNodo(NodoRN* nodo);

    bool sinRojoConHijoRojo(NodoRN* nodo);
    bool ordenCorrecto();
    bool padresCorrectos(NodoRN* nodo);
    int alturaNegra(NodoRN* nodo);
    int altura(NodoRN* nodo);
    int contarNodos(NodoRN* nodo);

    void liberar(NodoRN* nodo);
};
