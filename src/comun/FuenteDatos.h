// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Datos iniciales del sistema y la interfaz de donde se leen

#pragma once

#include <string>
#include <vector>
#include "comun/CargadorArchivos.h"

// Todo lo que el sistema necesita para arrancar
struct DatosIniciales {
    std::vector<Estudiante> estudiantes;
    std::vector<Curso> cursos;
    std::vector<Requisito> requisitos;
    std::vector<RegistroNota> notas;
    std::vector<CitaMatricula> citas;
    std::vector<Grupo> grupos;
    std::vector<Matricula> matriculas;
    std::vector<EnEspera> espera;
};

// Una fuente de datos sabe llenar DatosIniciales; el sistema no sabe
// si vienen de archivos o de la nube
class IFuenteDatos {
public:
    virtual ~IFuenteDatos() {}

    // false si no se pudieron leer los datos
    virtual bool leer(DatosIniciales& datos) = 0;
    virtual std::string nombre() = 0;
};

// Lee los archivos de texto de una carpeta
class FuenteArchivos : public IFuenteDatos {
public:
    FuenteArchivos(std::string carpeta);

    bool leer(DatosIniciales& datos) override;
    std::string nombre() override;

private:
    std::string carpeta;
};
