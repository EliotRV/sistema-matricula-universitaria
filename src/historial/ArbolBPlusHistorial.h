// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Arbol B+ del historial academico, ordenado por carne + periodo + curso

#pragma once

#include <string>
#include <vector>
#include "historial/RegistroNota.h"

// Maximo de claves por nodo; una hoja llena se divide 2 + 2
const int MAX_CLAVES_BPLUS = 3;

// Minimos para que un nodo que no es la raiz no quede casi vacio
const int MIN_CLAVES_HOJA = (MAX_CLAVES_BPLUS + 1) / 2;
const int MIN_CLAVES_INTERNO = (MAX_CLAVES_BPLUS + 1) / 2 - 1;

// Nodo: en los internos las claves son el indice; en las hojas cada
// clave va con su registro de nota y las hojas se enlazan con siguiente
struct NodoBPlus {
    std::string claves[MAX_CLAVES_BPLUS + 1];
    RegistroNota registros[MAX_CLAVES_BPLUS + 1];
    NodoBPlus* hijos[MAX_CLAVES_BPLUS + 2];

    int cantidad;
    bool esHoja;

    NodoBPlus* siguiente;
};

class ArbolBPlusHistorial {
public:
    ArbolBPlusHistorial();
    ~ArbolBPlusHistorial();

    ArbolBPlusHistorial(const ArbolBPlusHistorial&) = delete;
    ArbolBPlusHistorial& operator=(const ArbolBPlusHistorial&) = delete;

    // Operaciones principales
    bool insertar(RegistroNota registro);
    RegistroNota* buscar(std::string carne, std::string periodo, std::string codigoCurso);
    bool modificarNota(std::string carne, std::string periodo, std::string codigoCurso, int nota);
    bool eliminar(std::string carne, std::string periodo, std::string codigoCurso);

    // Consultas por rango recorriendo las hojas enlazadas
    // Devuelven cuantas hojas se leyeron
    int historial(std::string carne, std::vector<RegistroNota>& salida);
    int rangoPeriodos(std::string carne, std::string desde, std::string hasta, std::vector<RegistroNota>& salida);
    void todos(std::vector<RegistroNota>& salida);

    // Vista interna
    void mostrarNiveles();
    void mostrarHojas();
    int mostrarRuta(std::string carne, std::string periodo, std::string codigoCurso);
    void resumenCambios();

    // Datos del arbol
    int cantidad();
    int altura();
    int contarHojas();
    bool estructuraCorrecta();

    bool esVacio();
    void vaciar();

private:
    NodoBPlus* raiz;
    int total;

    // Cambios de la ultima insercion o eliminacion
    int divisiones;
    int prestamos;
    int uniones;
    int cambiosDeAltura;

    NodoBPlus* crearNodo(bool esHoja);
    void reiniciarCambios();

    NodoBPlus* buscarHoja(std::string clave);
    NodoBPlus* primeraHoja();
    int posicionHijo(NodoBPlus* nodo, std::string clave);

    // Insercion
    void insertarEnHoja(NodoBPlus* hoja, std::string clave, RegistroNota registro);
    bool insertarEn(NodoBPlus* nodo, std::string clave, RegistroNota registro, std::string& claveSube, NodoBPlus*& derecha);
    NodoBPlus* dividirHoja(NodoBPlus* hoja);
    NodoBPlus* dividirInterno(NodoBPlus* nodo, std::string& claveSube);

    // Eliminacion
    bool eliminarEn(NodoBPlus* nodo, std::string clave);
    void corregirHijo(NodoBPlus* padre, int posicion);
    void quitarDelPadre(NodoBPlus* padre, int posicionClave, int posicionHijo);

    // Vista y revision
    std::string claveCorta(std::string clave);
    void imprimirNodo(NodoBPlus* nodo);
    void imprimirNivel(NodoBPlus* nodo, int nivelActual, int nivelBuscado);
    bool revisarNodo(NodoBPlus* nodo, std::string minimo, bool hayMinimo, std::string maximo, bool hayMaximo,
                     int nivel, int& nivelHojas, std::vector<NodoBPlus*>& hojas);

    void liberar(NodoBPlus* nodo);
};
