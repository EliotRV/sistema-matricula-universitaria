// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Datos iniciales leidos desde la base de datos en la nube

#pragma once

#include "comun/FuenteDatos.h"
#include "nube/ClienteNube.h"

class FuenteNube : public IFuenteDatos {
public:
    FuenteNube(ClienteNube& cliente);

    // false si no hay conexion o no hay datos
    bool leer(DatosIniciales& datos) override;
    std::string nombre() override;

    // true si la nube estaba vacia
    bool estabaVacia();

private:
    ClienteNube& cliente;
    bool vacia;

    bool consultar(std::string coleccion, std::vector<Documento>& documentos);
};
