// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Funciones del estudiante

#include "estudiantes/Estudiante.h"

using namespace std;

bool carneValido(string carne) {
    if (carne.length() != 9 || carne[4] != '-') {
        return false;
    }

    for (int i = 0; i < 9; i++) {
        if (i != 4 && (carne[i] < '0' || carne[i] > '9')) {
            return false;
        }
    }

    return true;
}
