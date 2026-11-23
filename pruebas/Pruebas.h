// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Apoyo comun de las pruebas

#pragma once

#include <string>

// Muestra [ok] o [FALLO] y lleva la cuenta
void revisar(bool condicion, std::string descripcion);

// Lectura de archivos y dominio
void probarArchivos();
void probarArchivosDelPlan();
void probarDominio();

// Arbol rojo-negro de citas
void probarInsercion();
void probarEliminacion();
void probarAlAzar();
void probarAltura();
void probarReprogramar();

// Tabla hash extensible de estudiantes
void probarHashBasico();
void probarHashEliminacion();
void probarHashAlAzar();
void probarHashGrande();

// Arbol B+ del historial
void probarBPlusBasico();
void probarBPlusEliminacion();
void probarBPlusAlAzar();
void probarBPlusGrande();

// Grafo de requisitos
void probarGrafoBasico();
void probarGrafoCambios();
void probarGrafoAlAzar();

// Cola de espera
void probarColaBasica();
void probarColaAlAzar();
