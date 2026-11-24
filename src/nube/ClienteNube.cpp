// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del cliente de la nube

#include <cstdio>
#include <fstream>
#include <filesystem>
#include "nube/ClienteNube.h"
#include "nube/Json.h"

// popen en Windows
#ifdef _WIN32
#define abrirProceso _popen
#define cerrarProceso _pclose
#define SIN_ERRORES " 2>NUL"
#else
#define abrirProceso popen
#define cerrarProceso pclose
#define SIN_ERRORES " 2>/dev/null"
#endif

using namespace std;

ClienteNube::ClienteNube(string url) : url(url) {
    contador = 0;
}

string ClienteNube::ultimoError() {
    lock_guard<mutex> bloqueo(candado);
    return error;
}

void ClienteNube::anotarError(string texto) {
    lock_guard<mutex> bloqueo(candado);
    error = texto;
}

// Ejecutar curl
bool ClienteNube::ejecutar(string metodo, string direccion, string cuerpo, string& respuesta) {
    string archivo = "";
    string comando = "curl -s -m 20 -X " + metodo + " -w \"\\n%{http_code}\"";

    if (cuerpo != "") {
        int numero;

        {
            lock_guard<mutex> bloqueo(candado);
            contador++;
            numero = contador;
        }

        // Archivo temporal propio de esta peticion
        filesystem::path ruta = filesystem::temp_directory_path() /
                                ("eliotrojas_nube_" + to_string((long long)this % 100000) + "_" + to_string(numero) + ".json");
        archivo = ruta.string();

        ofstream salida(archivo, ios::binary);

        if (!salida.is_open()) {
            anotarError("no se pudo crear el archivo temporal");
            return false;
        }

        salida << cuerpo;
        salida.close();

        comando += " -H \"Content-Type: application/json\" --data-binary \"@" + archivo + "\"";
    }

    comando += " \"" + direccion + "\"" + SIN_ERRORES;

    FILE* proceso = abrirProceso(comando.c_str(), "r");

    if (proceso == nullptr) {
        anotarError("no se pudo ejecutar curl");

        if (archivo != "") {
            filesystem::remove(archivo);
        }

        return false;
    }

    string salida = "";
    char bloque[4096];

    while (fgets(bloque, sizeof(bloque), proceso) != nullptr) {
        salida += bloque;
    }

    cerrarProceso(proceso);

    if (archivo != "") {
        error_code ignorado;
        filesystem::remove(archivo, ignorado);
    }

    // La ultima linea es el codigo HTTP
    size_t corte = salida.find_last_of('\n');
    string codigo = (corte == string::npos) ? salida : salida.substr(corte + 1);
    respuesta = (corte == string::npos) ? "" : salida.substr(0, corte);

    if (codigo != "200") {
        if (codigo == "000" || codigo == "") {
            anotarError("sin conexion con la nube");
        } else {
            anotarError("la API respondio con el codigo " + codigo);
        }

        return false;
    }

    return true;
}

bool ClienteNube::consultar(string coleccion, vector<Documento>& documentos) {
    string respuesta;

    if (!ejecutar("GET", url + "?coleccion=" + coleccion, "", respuesta)) {
        return false;
    }

    if (!leerDocumentos(respuesta, documentos)) {
        anotarError("respuesta de la API con formato inesperado");
        return false;
    }

    return true;
}

bool ClienteNube::insertar(string coleccion, Documento datos) {
    string cuerpo = "{\"coleccion\":\"" + escaparJson(coleccion) + "\",\"datos\":" + datos.aJson() + "}";
    string respuesta;

    if (!ejecutar("POST", url, cuerpo, respuesta)) {
        return false;
    }

    if (valorDeClave(respuesta, "mensaje") == "") {
        anotarError("la API no confirmo la insercion");
        return false;
    }

    return true;
}

bool ClienteNube::actualizar(string coleccion, Documento filtro, Documento datos) {
    string cuerpo = "{\"coleccion\":\"" + escaparJson(coleccion) + "\",\"filtro\":" + filtro.aJson()
                  + ",\"datos\":" + datos.aJson() + "}";
    string respuesta;

    if (!ejecutar("PUT", url, cuerpo, respuesta)) {
        return false;
    }

    if (valorDeClave(respuesta, "mensaje") == "") {
        anotarError("la API no confirmo la actualizacion");
        return false;
    }

    return true;
}

bool ClienteNube::eliminar(string coleccion, Documento filtro) {
    string cuerpo = "{\"coleccion\":\"" + escaparJson(coleccion) + "\",\"filtro\":" + filtro.aJson() + "}";
    string respuesta;

    if (!ejecutar("DELETE", url, cuerpo, respuesta)) {
        return false;
    }

    if (valorDeClave(respuesta, "mensaje") == "") {
        anotarError("la API no confirmo el borrado");
        return false;
    }

    return true;
}

bool ClienteNube::disponible() {
    vector<Documento> documentos;
    return consultar("eliotrojas_estudiantes", documentos);
}
