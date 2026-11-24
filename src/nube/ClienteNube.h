// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Cliente de la base de datos en la nube (API MongoDB del curso)

#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "comun/Documento.h"

const std::string URL_API_NUBE = "https://paginas-web-cr.com/Api/apis/mongodb.php";

// Peticiones HTTP con curl
class ClienteNube {
public:
    ClienteNube(std::string url = URL_API_NUBE);

    // GET: todos los documentos de la coleccion
    bool consultar(std::string coleccion, std::vector<Documento>& documentos);

    // POST, PUT y DELETE
    bool insertar(std::string coleccion, Documento datos);
    bool actualizar(std::string coleccion, Documento filtro, Documento datos);
    bool eliminar(std::string coleccion, Documento filtro);

    // Una consulta corta para saber si hay conexion
    bool disponible();

    std::string ultimoError();

private:
    std::string url;
    std::string error;
    std::mutex candado;
    int contador;

    bool ejecutar(std::string metodo, std::string direccion, std::string cuerpo, std::string& respuesta);
    void anotarError(std::string texto);
};
