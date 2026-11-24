// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Lectura de los datos iniciales desde archivos de texto

#pragma once

#include <string>
#include <vector>
#include "estudiantes/Estudiante.h"
#include "citas/CitaMatricula.h"
#include "plan/Curso.h"
#include "matricula/Grupo.h"
#include "matricula/Matricula.h"
#include "historial/RegistroNota.h"

// Arista del grafo
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

// Lectura de cada archivo de datos

bool leerEstudiantes(std::string ruta, std::vector<Estudiante>& estudiantes, int& lineasConError);
bool leerCitas(std::string ruta, std::vector<CitaMatricula>& citas, int& lineasConError);
bool leerCursos(std::string ruta, std::vector<Curso>& cursos, int& lineasConError);
bool leerRequisitos(std::string ruta, std::vector<Requisito>& requisitos, int& lineasConError);
bool leerGrupos(std::string ruta, std::vector<Grupo>& grupos, int& lineasConError);
bool leerNotas(std::string ruta, std::vector<RegistroNota>& notas, int& lineasConError);
bool leerMatriculas(std::string ruta, std::vector<Matricula>& matriculas, int& lineasConError);
bool leerEspera(std::string ruta, std::vector<EnEspera>& espera, int& lineasConError);

// Hora HH:MM a HHMM
int leerHora(std::string texto);

// Apoyo de texto que tambien usa la consola
std::string recortar(std::string texto);
int separar(std::string linea, char separador, std::string campos[], int maximo);

// Texto de digitos a numero
int aNumero(std::string texto);
