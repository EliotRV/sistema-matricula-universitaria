// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del registro de nota

#include "historial/RegistroNota.h"

using namespace std;

bool periodoValido(string periodo) {
    // AAAA-Cn tiene 7 caracteres
    if (periodo.length() != 7) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (periodo[i] < '0' || periodo[i] > '9') {
            return false;
        }
    }

    return periodo[4] == '-' && periodo[5] == 'C' && periodo[6] >= '1' && periodo[6] <= '3';
}

string armarClaveNota(string carne, string periodo, string codigoCurso) {
    return carne + "|" + periodo + "|" + codigoCurso;
}

string claveDe(RegistroNota registro) {
    return armarClaveNota(registro.carne, registro.periodo, registro.codigoCurso);
}

bool estaAprobado(RegistroNota registro) {
    return registro.nota >= NOTA_MINIMA;
}
