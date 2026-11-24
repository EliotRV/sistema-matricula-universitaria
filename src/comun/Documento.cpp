// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del documento plano

#include "comun/Documento.h"

using namespace std;

// Si el campo ya existe se reemplaza su valor
void Documento::poner(string campo, string valor) {
    for (int i = 0; i < (int)campos.size(); i++) {
        if (campos[i] == campo) {
            valores[i] = valor;
            return;
        }
    }

    campos.push_back(campo);
    valores.push_back(valor);
}

string Documento::valor(string campo) const {
    for (int i = 0; i < (int)campos.size(); i++) {
        if (campos[i] == campo) {
            return valores[i];
        }
    }

    return "";
}

bool Documento::tiene(string campo) const {
    for (int i = 0; i < (int)campos.size(); i++) {
        if (campos[i] == campo) {
            return true;
        }
    }

    return false;
}

int Documento::cantidad() const {
    return campos.size();
}

string Documento::campo(int posicion) const {
    return campos[posicion];
}

string Documento::valorEn(int posicion) const {
    return valores[posicion];
}

string escaparJson(string texto) {
    string resultado = "";

    for (int i = 0; i < (int)texto.length(); i++) {
        char c = texto[i];

        if (c == '"') {
            resultado += "\\\"";
        } else if (c == '\\') {
            resultado += "\\\\";
        } else if (c == '\n') {
            resultado += "\\n";
        } else if (c == '\r') {
            resultado += "\\r";
        } else if (c == '\t') {
            resultado += "\\t";
        } else {
            resultado += c;
        }
    }

    return resultado;
}

string Documento::aJson() const {
    string json = "{";

    for (int i = 0; i < (int)campos.size(); i++) {
        if (i > 0) {
            json += ",";
        }

        json += "\"" + escaparJson(campos[i]) + "\":\"" + escaparJson(valores[i]) + "\"";
    }

    return json + "}";
}
