// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Grupo de un curso y funciones de horario

#pragma once

#include <string>

// El dia va de 1 (lunes) a 7 (domingo)
// Las horas se guardan como HHMM, por ejemplo 1800
struct Grupo {
    std::string codigoCurso;
    int numero;
    std::string periodo;
    int dia;
    int horaInicio;
    int horaFin;
    int cupo;
    int matriculados;
};

// Convierte "Lunes", "lunes" o "LUNES" a 1..7, 0 si no es un dia
int numeroDeDia(std::string texto);
std::string nombreDeDia(int dia);

bool horaValida(int horaHHMM);

// Dos grupos chocan si son del mismo periodo y dia y sus horas se traslapan
bool hayChoque(Grupo a, Grupo b);

bool tieneCupo(Grupo grupo);

// Imprime "Lunes 18:00-21:00"
void imprimirHorario(Grupo grupo);
