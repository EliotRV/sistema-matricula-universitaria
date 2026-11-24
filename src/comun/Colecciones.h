// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Colecciones de la base de datos y conversion entre entidades y documentos

#pragma once

#include <string>
#include "comun/Documento.h"
#include "comun/CargadorArchivos.h"

// Todas las colecciones llevan el prefijo eliotrojas_
const std::string COL_ESTUDIANTES = "eliotrojas_estudiantes";
const std::string COL_CURSOS = "eliotrojas_cursos";
const std::string COL_REQUISITOS = "eliotrojas_requisitos";
const std::string COL_NOTAS = "eliotrojas_notas";
const std::string COL_CITAS = "eliotrojas_citas";
const std::string COL_GRUPOS = "eliotrojas_grupos";
const std::string COL_MATRICULAS = "eliotrojas_matriculas";
const std::string COL_ESPERA = "eliotrojas_espera";
const std::string COL_NOTIFICACIONES = "eliotrojas_notificaciones";

// Entidad -> documento
Documento documentoDe(Estudiante estudiante);
Documento documentoDe(Curso curso);
Documento documentoDe(Requisito requisito);
Documento documentoDe(RegistroNota nota);
Documento documentoDe(CitaMatricula cita);
Documento documentoDe(Grupo grupo);
Documento documentoDe(Matricula matricula);
Documento documentoDe(EnEspera espera, long long llegada);

// Documento -> entidad; false si al documento le faltan datos o no son validos
bool leerDocumento(Documento& documento, Estudiante& estudiante);
bool leerDocumento(Documento& documento, Curso& curso);
bool leerDocumento(Documento& documento, Requisito& requisito);
bool leerDocumento(Documento& documento, RegistroNota& nota);
bool leerDocumento(Documento& documento, CitaMatricula& cita);
bool leerDocumento(Documento& documento, Grupo& grupo);
bool leerDocumento(Documento& documento, Matricula& matricula);
bool leerDocumento(Documento& documento, EnEspera& espera);

// Filtros para actualizar o borrar documentos
Documento filtroEstudiante(std::string carne);
Documento filtroCurso(std::string codigo);
Documento filtroRequisito(std::string requisito, std::string curso);
Documento filtroNota(std::string carne, std::string periodo, std::string codigoCurso);
Documento filtroCita(std::string carne);
Documento filtroGrupo(std::string codigoCurso, int numero, std::string periodo);
Documento filtroMatriculaActiva(std::string carne, std::string codigoCurso, int numero, std::string periodo);
Documento filtroEspera(std::string carne, std::string codigoCurso, int numero, std::string periodo);

// Campos de la cita: fecha AAAA-MM-DD y hora HH:MM
std::string textoFecha(long long fechaHora);
std::string textoHora(long long fechaHora);
std::string textoHoraDelDia(int horaHHMM);
