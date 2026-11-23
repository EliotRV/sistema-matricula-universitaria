// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Componente del historial academico: notas sobre el arbol B+

#pragma once

#include <string>
#include <vector>
#include "historial/ArbolBPlusHistorial.h"
#include "estudiantes/ServicioEstudiantes.h"
#include "plan/ServicioPlan.h"

class ServicioHistorial {
public:
    ServicioHistorial(ServicioEstudiantes& estudiantes, ServicioPlan& plan);

    // Sin mensajes: para la carga de datos
    bool cargar(RegistroNota registro);

    // Con mensajes: para el menu
    bool registrar(RegistroNota registro);
    bool corregir(std::string carne, std::string periodo, std::string codigoCurso, int nota);
    bool eliminar(std::string carne, std::string periodo, std::string codigoCurso);

    void mostrarHistorial(std::string carne);
    void mostrarRango(std::string carne, std::string desde, std::string hasta);
    void mostrarNota(std::string carne, std::string periodo, std::string codigoCurso);
    void mostrarArbol();
    void mostrarHojas();

    // Consultas para los demas componentes
    void aprobados(std::string carne, std::vector<std::string>& salida);
    bool aprobo(std::string carne, std::string codigoCurso);
    bool promedio(std::string carne, double& resultado);
    bool tieneNotas(std::string carne);
    bool hayNotasDelCurso(std::string codigoCurso);
    void todas(std::vector<RegistroNota>& salida);

    int cantidad();
    void vaciar();

private:
    ArbolBPlusHistorial arbol;
    ServicioEstudiantes& estudiantes;
    ServicioPlan& plan;

    void imprimirNotas(std::vector<RegistroNota>& notas);
};
