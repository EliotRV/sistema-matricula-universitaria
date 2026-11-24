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

// Hace las peticiones HTTP con el programa curl, que viene con
// Windows 10/11 y con Linux: asi no hace falta instalar bibliotecas.
// El cuerpo de cada peticion se escribe en un archivo temporal para no
// tener problemas con las comillas en la linea de comandos.
class ClienteNube {
public:
    ClienteNube(std::string url = URL_API_NUBE);

    // GET: todos los documentos de la coleccion
    bool consultar(std::string coleccion, std::vector<Documento>& documentos);

    // POST, PUT y DELETE; el filtro elige los documentos (todos los que cumplan)
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
