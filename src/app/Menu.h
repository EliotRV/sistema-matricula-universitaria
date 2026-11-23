// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Menu de consola por modulos

#pragma once

#include "app/SistemaMatricula.h"

class Menu {
public:
    Menu(SistemaMatricula& sistema, IFuenteDatos& fuente);

    void ejecutar();

private:
    SistemaMatricula& sistema;
    IFuenteDatos& fuente;

    void encabezado(std::string modulo);

    void menuEstudiantes();
    void menuPlan();
    void menuMatricula();
    void menuCitas();
    void menuHistorial();
    void cambiarHora();
    void recargar();
};
