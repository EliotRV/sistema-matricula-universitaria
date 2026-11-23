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
