// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Grupo de un curso y funciones de horario

#pragma once

#include <string>

// Dia de 1 a 7 y horas como HHMM
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

// Nombre del dia a numero
int numeroDeDia(std::string texto);
std::string nombreDeDia(int dia);

bool horaValida(int horaHHMM);

// Choque de horario
bool hayChoque(Grupo a, Grupo b);

bool tieneCupo(Grupo grupo);

// Imprime una hora HHMM como HH:MM
void imprimirHora(int horaHHMM);

// Imprime "Lunes 18:00-21:00"
void imprimirHorario(Grupo grupo);
