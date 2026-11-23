// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Lectura de los datos iniciales desde archivos de texto

#pragma once

#include <string>
#include <vector>
#include "Estudiante.h"
#include "CitaMatricula.h"
#include "Curso.h"
#include "Grupo.h"
#include "Matricula.h"
#include "RegistroNota.h"

// Arista del grafo: requisito es requisito de curso
struct Requisito {
    std::string requisito;
    std::string curso;
};

// Un estudiante en la lista de espera de un grupo
struct EnEspera {
    std::string codigoCurso;
    int numeroGrupo;
    std::string periodo;
    std::string carne;
};

// Todas devuelven false si el archivo no se pudo abrir
// En lineasConError queda la cantidad de lineas que no se pudieron leer
// Las lineas vacias y las que empiezan con # se ignoran

bool leerEstudiantes(std::string ruta, std::vector<Estudiante>& estudiantes, int& lineasConError);
bool leerCitas(std::string ruta, std::vector<CitaMatricula>& citas, int& lineasConError);
bool leerCursos(std::string ruta, std::vector<Curso>& cursos, int& lineasConError);
bool leerRequisitos(std::string ruta, std::vector<Requisito>& requisitos, int& lineasConError);
bool leerGrupos(std::string ruta, std::vector<Grupo>& grupos, int& lineasConError);
bool leerNotas(std::string ruta, std::vector<RegistroNota>& notas, int& lineasConError);
bool leerMatriculas(std::string ruta, std::vector<Matricula>& matriculas, int& lineasConError);
bool leerEspera(std::string ruta, std::vector<EnEspera>& espera, int& lineasConError);

// Convierte "HH:MM" a HHMM, -1 si no es una hora valida
int leerHora(std::string texto);
