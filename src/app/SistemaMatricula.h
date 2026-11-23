// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Sistema de matricula: une los componentes y coordina lo que involucra a varios

#pragma once

#include <string>
#include "comun/FuenteDatos.h"
#include "estudiantes/ServicioEstudiantes.h"
#include "plan/ServicioPlan.h"
#include "historial/ServicioHistorial.h"
#include "citas/ServicioCitas.h"
#include "matricula/ServicioMatricula.h"

class SistemaMatricula {
public:
    SistemaMatricula();

    // Carga los datos iniciales desde una fuente (archivos o nube)
    bool cargarDatos(IFuenteDatos& fuente);
    void cargar(DatosIniciales& datos);
    void vaciar();

    // Operaciones que tocan a varios componentes
    bool eliminarEstudiante(std::string carne);
    bool eliminarCurso(std::string codigo);
    void mostrarHabilitados(std::string carne);
    void asignarCitasPorPromedio(long long inicio, int minutosEntreCitas);

    // Hora simulada para probar las citas
    long long horaActual();
    void cambiarHoraActual(long long fechaHora);

    // Datos del sistema en el mismo formato en que se cargan
    void exportar(DatosIniciales& datos);

    // Componentes
    ServicioEstudiantes& estudiantes();
    ServicioPlan& plan();
    ServicioHistorial& historial();
    ServicioCitas& citas();
    ServicioMatricula& matricula();

private:
    // El orden importa: cada servicio usa a los que estan antes
    ServicioEstudiantes servicioEstudiantes;
    ServicioPlan servicioPlan;
    ServicioHistorial servicioHistorial;
    ServicioCitas servicioCitas;
    ServicioMatricula servicioMatricula;

    long long hora;
};
