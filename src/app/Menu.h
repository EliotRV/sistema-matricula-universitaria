// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Menu de consola por modulos

#pragma once

#include "app/SistemaMatricula.h"
#include "mensajeria/ServicioNube.h"
#include "nube/ClienteNube.h"

class Menu {
public:
    // nube y cliente pueden ser nullptr si se trabaja sin nube
    Menu(SistemaMatricula& sistema, IFuenteDatos& fuente, ServicioNube* nube, ClienteNube* cliente);

    void ejecutar();

    // Cambia la fuente que usa "volver a cargar"
    void usarFuente(IFuenteDatos& otra);

private:
    SistemaMatricula& sistema;
    IFuenteDatos* fuente;
    ServicioNube* nube;
    ClienteNube* cliente;

    void encabezado(std::string modulo);

    void menuEstudiantes();
    void menuPlan();
    void menuMatricula();
    void menuCitas();
    void menuHistorial();
    void menuNube();
    void cambiarHora();
    void recargar();
    void terminarEnvios();
};
