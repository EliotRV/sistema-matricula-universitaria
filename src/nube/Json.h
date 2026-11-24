// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Lectura de las respuestas JSON de la API

#pragma once

#include <string>
#include <vector>
#include "comun/Documento.h"

// Lee {"coleccion": ..., "total": N, "documentos": [ {...}, ... ]}.
// Cada documento queda plano: los numeros y booleanos pasan a texto y
// el _id ({"$oid": "..."}) queda como el texto del oid.
// Devuelve false si el texto no tiene esa forma.
bool leerDocumentos(std::string json, std::vector<Documento>& documentos);

// Valor de texto o numero de una clave del primer nivel, "" si no esta.
// Sirve para leer {"mensaje": "...", "eliminados": 3}
std::string valorDeClave(std::string json, std::string clave);
