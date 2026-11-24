// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Lectura de las respuestas JSON de la API

#pragma once

#include <string>
#include <vector>
#include "comun/Documento.h"

// Leer la respuesta de la API
bool leerDocumentos(std::string json, std::vector<Documento>& documentos);

// Valor de una clave del primer nivel
std::string valorDeClave(std::string json, std::string clave);
