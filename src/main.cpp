// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Menu del prototipo: citas de matricula en un arbol rojo-negro

#include <iostream>
#include <string>
#include "SistemaMatricula.h"

using namespace std;

// Archivos de datos iniciales

const string ARCHIVO_ESTUDIANTES = "datos/estudiantes.txt";
const string ARCHIVO_CITAS = "datos/citas.txt";

// Prototipos

void mostrarMenu(SistemaMatricula& sistema);
void pausa();
bool leerFechaHora(string mensaje, long long& fechaHora);
string leerCarne();

// Programa principal

int main() {
    SistemaMatricula sistema;
    int opcion;

    cout << "\n==========================================\n";
    cout << "  SISTEMA DE MATRICULA UNIVERSITARIA\n";
    cout << "  Eliot Rojas Vargas\n";
    cout << "==========================================\n";
    cout << "\n  Cargando datos iniciales...\n";

    sistema.cargarDatos(ARCHIVO_ESTUDIANTES, ARCHIVO_CITAS);

    do {
        mostrarMenu(sistema);
        cin >> opcion;
        cout << "\n";

        switch (opcion) {

            case 1:
                sistema.agendaDeCitas();
                break;

            case 2: {
                long long fechaHora;

                if (leerFechaHora("Fecha y hora de la cita", fechaHora)) {
                    cout << "\n";
                    sistema.buscarCita(fechaHora);
                }
                break;
            }

            case 3: {
                string carne = leerCarne();
                cout << "\n";
                sistema.consultarCitaDe(carne);
                break;
            }

            case 4: {
                string carne = leerCarne();
                long long fechaHora;

                if (leerFechaHora("Fecha y hora de la cita", fechaHora)) {
                    cout << "\n";
                    sistema.asignarCita(fechaHora, carne);
                }
                break;
            }

            case 5: {
                long long fechaHora;

                if (leerFechaHora("Fecha y hora de la cita a cancelar", fechaHora)) {
                    cout << "\n";
                    sistema.cancelarCita(fechaHora);
                }
                break;
            }

            case 6: {
                long long desde;
                long long hasta;

                if (leerFechaHora("Desde", desde) && leerFechaHora("Hasta", hasta)) {
                    cout << "\n";
                    sistema.citasEnRango(desde, hasta);
                }
                break;
            }

            case 7: {
                string carne = leerCarne();
                cout << "\n";
                sistema.validarCita(carne);
                break;
            }

            case 8: {
                long long fechaHora;

                if (leerFechaHora("Nueva hora actual", fechaHora)) {
                    cout << "\n";
                    sistema.cambiarHoraActual(fechaHora);
                }
                break;
            }

            case 9:
                sistema.mostrarArbolDeCitas();
                break;

            case 10:
                sistema.listarEstudiantes();
                break;

            case 11:
                cout << "  Cargando datos iniciales...\n";
                sistema.cargarDatos(ARCHIVO_ESTUDIANTES, ARCHIVO_CITAS);
                break;

            case 0:
                cout << "  Programa finalizado.\n";
                break;

            default:
                cout << "  Opcion invalida.\n";
        }

        if (opcion != 0) {
            pausa();
        }

    } while (opcion != 0);

    return 0;
}

// Menu

void mostrarMenu(SistemaMatricula& sistema) {
    cout << "\n==========================================\n";
    cout << "  SISTEMA DE MATRICULA - CITAS\n";
    cout << "  Hora actual: ";
    sistema.mostrarHoraActual();
    cout << "\n==========================================\n";
    cout << "   1. Agenda de citas\n";
    cout << "   2. Buscar cita por fecha y hora\n";
    cout << "   3. Consultar cita de un estudiante\n";
    cout << "   4. Asignar cita (insertar)\n";
    cout << "   5. Cancelar cita (eliminar)\n";
    cout << "   6. Citas en un rango de horas\n";
    cout << "   7. Validar cita de un estudiante\n";
    cout << "   8. Cambiar hora actual\n";
    cout << "   9. Mostrar arbol rojo-negro y propiedades\n";
    cout << "  10. Listar estudiantes\n";
    cout << "  11. Volver a cargar los archivos\n";
    cout << "   0. Salir\n";
    cout << "  Opcion: ";
}

void pausa() {
    cout << "\n  Presione ENTER para continuar...";
    cin.ignore();
    cin.get();
}

// Lectura de datos

// Pide dia, mes, anio, hora y minuto; false si la fecha no es valida
bool leerFechaHora(string mensaje, long long& fechaHora) {
    int dia;
    int mes;
    int anio;
    int hora;
    int minuto;

    cout << "  " << mensaje << "\n";
    cout << "    Dia mes anio (ej. 7 12 2026): ";
    cin >> dia >> mes >> anio;
    cout << "    Hora minuto (ej. 8 30): ";
    cin >> hora >> minuto;

    if (!fechaHoraValida(anio, mes, dia, hora, minuto)) {
        cout << "\n  La fecha o la hora no es valida.\n";
        return false;
    }

    fechaHora = armarFechaHora(anio, mes, dia, hora, minuto);
    return true;
}

string leerCarne() {
    string carne;

    cout << "  Carne (ej. 2026-0101): ";
    cin >> carne;

    return carne;
}
