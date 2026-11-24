// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del lector de JSON (descenso recursivo)

#include "nube/Json.h"

using namespace std;

// Lector de JSON

class LectorJson {
public:
    LectorJson(string& texto) : texto(texto), posicion(0) {
    }

    void saltarEspacios() {
        while (posicion < texto.length() && (texto[posicion] == ' ' || texto[posicion] == '\n'
               || texto[posicion] == '\r' || texto[posicion] == '\t')) {
            posicion++;
        }
    }

    bool siguienteEs(char c) {
        saltarEspacios();
        return posicion < texto.length() && texto[posicion] == c;
    }

    bool consumir(char c) {
        if (!siguienteEs(c)) {
            return false;
        }

        posicion++;
        return true;
    }

    // "texto con \"escapes\""
    bool leerTexto(string& resultado) {
        if (!consumir('"')) {
            return false;
        }

        resultado = "";

        while (posicion < texto.length()) {
            char c = texto[posicion];
            posicion++;

            if (c == '"') {
                return true;
            }

            if (c != '\\') {
                resultado += c;
                continue;
            }

            if (posicion >= texto.length()) {
                return false;
            }

            char escape = texto[posicion];
            posicion++;

            if (escape == 'n') {
                resultado += '\n';
            } else if (escape == 't') {
                resultado += '\t';
            } else if (escape == 'r') {
                resultado += '\r';
            } else if (escape == 'u') {
                // Solo caracteres ASCII
                if (posicion + 4 > texto.length()) {
                    return false;
                }

                int codigo = stoi(texto.substr(posicion, 4), nullptr, 16);
                posicion += 4;
                resultado += (codigo < 128) ? (char)codigo : '?';
            } else {
                // \" \\ \/ y los demas se copian tal cual
                resultado += escape;
            }
        }

        return false;
    }

    // Numeros, true, false y null
    bool leerPalabra(string& resultado) {
        saltarEspacios();
        resultado = "";

        while (posicion < texto.length()) {
            char c = texto[posicion];

            if (c == ',' || c == '}' || c == ']' || c == ' ' || c == '\n' || c == '\r' || c == '\t') {
                break;
            }

            resultado += c;
            posicion++;
        }

        if (resultado == "null") {
            resultado = "";
            return true;
        }

        return resultado.length() > 0;
    }

    // Cualquier valor como texto
    bool leerValorPlano(string& resultado) {
        if (siguienteEs('"')) {
            return leerTexto(resultado);
        }

        if (siguienteEs('{')) {
            Documento interno;

            if (!leerObjeto(interno)) {
                return false;
            }

            resultado = interno.valor("$oid");
            return true;
        }

        if (siguienteEs('[')) {
            resultado = "";
            return saltarArreglo();
        }

        return leerPalabra(resultado);
    }

    // { "campo": valor, ... }
    bool leerObjeto(Documento& documento) {
        if (!consumir('{')) {
            return false;
        }

        if (consumir('}')) {
            return true;
        }

        while (true) {
            string campo;
            string valor;

            if (!leerTexto(campo) || !consumir(':') || !leerValorPlano(valor)) {
                return false;
            }

            documento.poner(campo, valor);

            if (consumir(',')) {
                continue;
            }

            return consumir('}');
        }
    }

    bool saltarArreglo() {
        if (!consumir('[')) {
            return false;
        }

        if (consumir(']')) {
            return true;
        }

        while (true) {
            string ignorado;

            if (!leerValorPlano(ignorado)) {
                return false;
            }

            if (consumir(',')) {
                continue;
            }

            return consumir(']');
        }
    }

    // [ {...}, {...} ]
    bool leerArregloDeObjetos(vector<Documento>& documentos) {
        if (!consumir('[')) {
            return false;
        }

        if (consumir(']')) {
            return true;
        }

        while (true) {
            Documento documento;

            if (!leerObjeto(documento)) {
                return false;
            }

            documentos.push_back(documento);

            if (consumir(',')) {
                continue;
            }

            return consumir(']');
        }
    }

    // Buscar la clave documentos
    bool leerRespuesta(vector<Documento>& documentos) {
        if (!consumir('{')) {
            return false;
        }

        bool encontrado = false;

        while (!siguienteEs('}')) {
            string campo;

            if (!leerTexto(campo) || !consumir(':')) {
                return false;
            }

            if (campo == "documentos") {
                if (!leerArregloDeObjetos(documentos)) {
                    return false;
                }

                encontrado = true;
            } else {
                string ignorado;

                if (!leerValorPlano(ignorado)) {
                    return false;
                }
            }

            if (!consumir(',')) {
                break;
            }
        }

        return consumir('}') && encontrado;
    }

private:
    string& texto;
    size_t posicion;
};

bool leerDocumentos(string json, vector<Documento>& documentos) {
    LectorJson lector(json);
    vector<Documento> leidos;

    // Escape mal escrito
    try {
        if (!lector.leerRespuesta(leidos)) {
            return false;
        }
    } catch (...) {
        return false;
    }

    for (int i = 0; i < (int)leidos.size(); i++) {
        documentos.push_back(leidos[i]);
    }

    return true;
}

string valorDeClave(string json, string clave) {
    LectorJson lector(json);
    Documento documento;

    try {
        if (!lector.leerObjeto(documento)) {
            return "";
        }
    } catch (...) {
        return "";
    }

    return documento.valor(clave);
}
