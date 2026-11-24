// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Componente de estudiantes: reglas y consultas sobre la tabla hash

#pragma once

#include <string>
#include <vector>
#include "estudiantes/TablaHashEstudiantes.h"
#include "mensajeria/Mensaje.h"

class ServicioEstudiantes : public EmisorDeMensajes {
public:
    // Sin mensajes: para la carga de datos
    bool cargar(Estudiante estudiante);

    // Con mensajes: para el menu
    bool registrar(Estudiante estudiante);
    bool modificar(std::string carne, std::string nombre, std::string correo);
    bool eliminar(std::string carne);

    void mostrar(std::string carne);
    void listar();
    void mostrarTabla();

    // Consultas para los demas componentes
    Estudiante* buscar(std::string carne);
    bool existe(std::string carne);
    std::string nombreDe(std::string carne);
    void todos(std::vector<Estudiante>& salida);

    // Guardar la cita en el estudiante
    void ponerCita(std::string carne, long long fechaHora);
    void quitarCita(std::string carne);

    int cantidad();
    void vaciar();

private:
    TablaHashEstudiantes tabla;
};
