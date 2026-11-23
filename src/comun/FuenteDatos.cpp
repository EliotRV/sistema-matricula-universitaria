// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Lectura de los datos iniciales desde archivos

#include <iostream>
#include "comun/FuenteDatos.h"

using namespace std;

FuenteArchivos::FuenteArchivos(string carpeta) : carpeta(carpeta) {
}

string FuenteArchivos::nombre() {
    return "archivos de " + carpeta;
}

// Lee los 8 archivos; estudiantes y cursos son obligatorios,
// los demas pueden faltar (por ejemplo matriculas y espera al inicio)
bool FuenteArchivos::leer(DatosIniciales& datos) {
    int errores = 0;
    int totalErrores = 0;
    bool obligatorios = true;

    if (!leerEstudiantes(carpeta + "estudiantes.txt", datos.estudiantes, errores)) {
        cout << "  No se pudo abrir " << carpeta << "estudiantes.txt\n";
        obligatorios = false;
    }

    totalErrores += errores;

    if (!leerCursos(carpeta + "cursos.txt", datos.cursos, errores)) {
        cout << "  No se pudo abrir " << carpeta << "cursos.txt\n";
        obligatorios = false;
    }

    totalErrores += errores;

    if (!leerRequisitos(carpeta + "requisitos.txt", datos.requisitos, errores)) {
        cout << "  No se pudo abrir " << carpeta << "requisitos.txt\n";
    }

    totalErrores += errores;

    if (!leerNotas(carpeta + "notas.txt", datos.notas, errores)) {
        cout << "  No se pudo abrir " << carpeta << "notas.txt\n";
    }

    totalErrores += errores;

    if (!leerCitas(carpeta + "citas.txt", datos.citas, errores)) {
        cout << "  No se pudo abrir " << carpeta << "citas.txt\n";
    }

    totalErrores += errores;

    if (!leerGrupos(carpeta + "grupos.txt", datos.grupos, errores)) {
        cout << "  No se pudo abrir " << carpeta << "grupos.txt\n";
    }

    totalErrores += errores;

    if (!leerMatriculas(carpeta + "matriculas.txt", datos.matriculas, errores)) {
        cout << "  No se pudo abrir " << carpeta << "matriculas.txt\n";
    }

    totalErrores += errores;

    if (!leerEspera(carpeta + "espera.txt", datos.espera, errores)) {
        cout << "  No se pudo abrir " << carpeta << "espera.txt\n";
    }

    totalErrores += errores;

    if (totalErrores > 0) {
        cout << "  Lineas con error en los archivos: " << totalErrores << "\n";
    }

    return obligatorios;
}
