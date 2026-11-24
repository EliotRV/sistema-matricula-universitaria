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

    // false si no hay conexion o si la nube todavia no tiene datos
    bool leer(DatosIniciales& datos) override;
    std::string nombre() override;

    // Despues de leer: true si fallo porque la nube estaba vacia
    bool estabaVacia();

private:
    ClienteNube& cliente;
    bool vacia;

    bool consultar(std::string coleccion, std::vector<Documento>& documentos);
};
