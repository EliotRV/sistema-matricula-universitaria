// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Componente de matricula: grupos, listas de espera y reglas de matricula

#pragma once

#include <string>
#include <vector>
#include "matricula/Grupo.h"
#include "matricula/Matricula.h"
#include "matricula/ColaEspera.h"
#include "estudiantes/ServicioEstudiantes.h"
#include "plan/ServicioPlan.h"
#include "historial/ServicioHistorial.h"
#include "citas/ServicioCitas.h"

// Cada grupo lleva su propia cola de espera
struct NodoGrupo {
    Grupo grupo;
    ColaEspera espera;
    NodoGrupo* siguiente;
};

enum ResultadoMatricula {
    MATRICULADO,
    SIN_ESTUDIANTE,
    SIN_GRUPO,
    SIN_CITA,
    CITA_NO_HABILITADA,
    YA_APROBADO,
    YA_MATRICULADO,
    FALTAN_REQUISITOS,
    CHOQUE_HORARIO,
    GRUPO_LLENO
};

class ServicioMatricula {
public:
    ServicioMatricula(ServicioEstudiantes& estudiantes, ServicioPlan& plan,
                      ServicioHistorial& historial, ServicioCitas& citas);
    ~ServicioMatricula();

    ServicioMatricula(const ServicioMatricula&) = delete;
    ServicioMatricula& operator=(const ServicioMatricula&) = delete;

    // Sin mensajes: para la carga de datos
    bool cargarGrupo(Grupo grupo);
    bool cargarMatricula(Matricula matricula);
    bool cargarEspera(std::string codigoCurso, int numero, std::string periodo, std::string carne);

    // Grupos
    bool abrirGrupo(Grupo grupo);
    bool modificarCupo(std::string codigoCurso, int numero, int cupo);
    bool cerrarGrupo(std::string codigoCurso, int numero);
    void mostrarGrupos();
    void mostrarListasEspera();

    // Matricula (CU-04 a CU-09)
    ResultadoMatricula matricular(std::string carne, std::string codigoCurso, int numero, long long horaActual);
    bool unirseAEspera(std::string carne, std::string codigoCurso, int numero, long long horaActual);
    bool salirDeEspera(std::string carne, std::string codigoCurso, int numero);
    bool retirar(std::string carne, std::string codigoCurso);
    void mostrarMatriculaDe(std::string carne);

    // Consultas para los demas componentes
    Grupo* buscarGrupo(std::string codigoCurso, int numero);
    bool tieneMatriculasActivas(std::string carne);
    bool hayGruposDelCurso(std::string codigoCurso);
    void sacarDeTodasLasColas(std::string carne);
    void grupos(std::vector<Grupo>& salida);
    void matriculas(std::vector<Matricula>& salida);
    void esperas(std::vector<std::string>& cursos, std::vector<int>& numeros, std::vector<std::string>& carnes);

    std::string periodo();
    void cambiarPeriodo(std::string periodo);
    void vaciar();

private:
    NodoGrupo* primero;
    std::vector<Matricula> lista;
    std::string periodoActual;

    ServicioEstudiantes& estudiantes;
    ServicioPlan& plan;
    ServicioHistorial& historial;
    ServicioCitas& citas;

    NodoGrupo* buscarNodo(std::string codigoCurso, int numero);
    int buscarMatriculaActiva(std::string carne, std::string codigoCurso);

    // Revisa todo menos el cupo; en faltan quedan los requisitos pendientes
    ResultadoMatricula validar(std::string carne, NodoGrupo* nodo, bool revisarCita, long long horaActual,
                               std::vector<std::string>& faltan, std::string& choqueCon);

    void explicar(ResultadoMatricula resultado, std::string carne, NodoGrupo* nodo,
                  std::vector<std::string>& faltan, std::string choqueCon);

    void registrar(std::string carne, NodoGrupo* nodo);
    void asignarCupoLiberado(NodoGrupo* nodo);
    void imprimirGrupo(Grupo& grupo);
};
