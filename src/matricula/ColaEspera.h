// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Cola de la lista de espera de un grupo (primero en llegar, primero en salir)

#pragma once

#include <string>
#include <vector>

struct NodoCola {
    std::string carne;
    NodoCola* siguiente;
};

class ColaEspera {
public:
    ColaEspera();
    ~ColaEspera();

    ColaEspera(const ColaEspera&) = delete;
    ColaEspera& operator=(const ColaEspera&) = delete;

    // Agregar al final de la cola
    bool encolar(std::string carne);

    // Saca al primero; false si la cola esta vacia
    bool desencolar(std::string& carne);

    // Sacar de cualquier posicion
    bool sacar(std::string carne);

    std::string frente();
    bool contiene(std::string carne);

    // Posicion desde 1; 0 si no esta
    int posicionDe(std::string carne);

    void listar(std::vector<std::string>& salida);
    void mostrar();

    int cantidad();
    bool esVacia();
    void vaciar();

private:
    NodoCola* primero;
    NodoCola* ultimo;
    int total;
};
