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

    // Un estudiante no puede estar dos veces en la misma cola
    bool encolar(std::string carne);

    // Saca al primero; false si la cola esta vacia
    bool desencolar(std::string& carne);

    // Saca a un estudiante de cualquier posicion (por ejemplo si se retira de la espera)
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
