// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del menu de consola

#include <iostream>
#include "app/Menu.h"
#include "comun/Consola.h"
#include "comun/Colecciones.h"

using namespace std;

Menu::Menu(SistemaMatricula& sistema, IFuenteDatos& fuente, ServicioNube* nube, ClienteNube* cliente)
    : sistema(sistema), fuente(&fuente), nube(nube), cliente(cliente) {
}

void Menu::usarFuente(IFuenteDatos& otra) {
    fuente = &otra;
}

void Menu::encabezado(string modulo) {
    cout << "\n==========================================================\n";
    cout << "  SISTEMA DE MATRICULA - " << modulo << "\n";
    cout << "  Periodo " << sistema.matricula().periodo() << "   Hora actual: ";
    imprimirFechaHora(sistema.horaActual());
    cout << "\n==========================================================\n";
}

// Menu principal

void Menu::ejecutar() {
    int opcion;

    do {
        encabezado("MENU PRINCIPAL");
        cout << "   1. Estudiantes            (tabla hash)\n";
        cout << "   2. Plan de estudios       (grafo)\n";
        cout << "   3. Grupos y matricula     (colas de espera)\n";
        cout << "   4. Citas de matricula     (arbol rojo-negro)\n";
        cout << "   5. Historial academico    (arbol B+)\n";
        cout << "   6. Nube y cola de mensajes\n";
        cout << "   7. Cambiar hora actual\n";
        cout << "   8. Volver a cargar los datos\n";
        cout << "   0. Salir\n";

        opcion = leerOpcion();

        switch (opcion) {
            case 1:
                menuEstudiantes();
                break;

            case 2:
                menuPlan();
                break;

            case 3:
                menuMatricula();
                break;

            case 4:
                menuCitas();
                break;

            case 5:
                menuHistorial();
                break;

            case 6:
                menuNube();
                break;

            case 7:
                cambiarHora();
                pausar();
                break;

            case 8:
                recargar();
                pausar();
                break;

            case 0:
                break;

            default:
                cout << "  Opcion invalida.\n";
                pausar();
        }

    } while (opcion != 0 && !entradaCerrada());

    terminarEnvios();
    cout << "\n  Programa finalizado.\n";
}

// Antes de salir se termina de enviar lo que quede en la cola
void Menu::terminarEnvios() {
    if (nube == nullptr) {
        return;
    }

    int pendientes = nube->pendientes();

    if (pendientes > 0) {
        cout << "\n  Enviando " << pendientes << " mensajes pendientes a la nube...\n";
        nube->esperarVacia(300);
    }

    nube->detener();

    if (nube->fallidos() > 0) {
        cout << "  Aviso: " << nube->fallidos() << " cambios no se pudieron guardar en la nube.\n";
    }
}

void Menu::cambiarHora() {
    long long fechaHora;

    cout << "\n";

    if (leerFechaHora("Nueva hora actual", fechaHora)) {
        sistema.cambiarHoraActual(fechaHora);
        cout << "\n  Hora actual: ";
        imprimirFechaHora(fechaHora);
        cout << "\n";
    }
}

void Menu::recargar() {
    cout << "\n  Se vuelven a leer los datos desde " << fuente->nombre() << ".\n";

    if (!leerSiNo("Continuar")) {
        return;
    }

    // Primero se termina de guardar lo pendiente para leer datos al dia
    if (nube != nullptr && nube->pendientes() > 0) {
        cout << "\n  Esperando que se envien " << nube->pendientes() << " mensajes...\n";
        nube->esperarVacia(300);
    }

    cout << "\n";
    sistema.cargarDatos(*fuente);
}

// Nube y cola de mensajes

void Menu::menuNube() {
    int opcion;

    do {
        encabezado("NUBE Y COLA DE MENSAJES");
        cout << "   1. Estado de la cola y del servicio\n";
        cout << "   2. Bitacora de mensajes enviados\n";
        cout << "   3. Probar la conexion con la nube\n";
        cout << "   4. Reintentar los mensajes que fallaron\n";
        cout << "   5. Subir todos los datos actuales a la nube\n";
        cout << "   6. Ver las ultimas notificaciones enviadas\n";
        cout << "   0. Volver\n";

        opcion = leerOpcion();
        cout << "\n";

        if (opcion != 0 && (nube == nullptr || cliente == nullptr)) {
            cout << "  El sistema esta trabajando sin nube.\n";
            pausar();
            continue;
        }

        switch (opcion) {
            case 1:
                nube->mostrarEstado();
                break;

            case 2:
                nube->mostrarBitacora();
                break;

            case 3:
                cout << "  Consultando " << URL_API_NUBE << " ...\n";

                if (cliente->disponible()) {
                    cout << "  Hay conexion con la nube.\n";
                } else {
                    cout << "  " << cliente->ultimoError() << ".\n";
                }
                break;

            case 4: {
                int reintentos = nube->reintentarFallidos();
                cout << "  Mensajes devueltos a la cola: " << reintentos << "\n";
                break;
            }

            case 5: {
                cout << "  Se reemplazan las colecciones eliotrojas_ con los datos actuales.\n";

                if (leerSiNo("Continuar")) {
                    int mensajes = sistema.publicarTodo();
                    cout << "\n  " << mensajes << " mensajes en la cola. Se envian en segundo plano:\n";
                    cout << "  puede seguir usando el sistema y ver el avance en la opcion 1.\n";
                }
                break;
            }

            case 6: {
                vector<Documento> notificaciones;

                if (!cliente->consultar(COL_NOTIFICACIONES, notificaciones)) {
                    cout << "  " << cliente->ultimoError() << ".\n";
                    break;
                }

                if (notificaciones.size() == 0) {
                    cout << "  Todavia no hay notificaciones.\n";
                    break;
                }

                int desde = notificaciones.size() > 10 ? notificaciones.size() - 10 : 0;

                cout << "  Enviada              Carne       Mensaje\n";
                cout << "  ----------------------------------------------------------\n";

                for (int i = desde; i < (int)notificaciones.size(); i++) {
                    cout << "  " << notificaciones[i].valor("enviada") << "  " << notificaciones[i].valor("carne")
                         << "   " << notificaciones[i].valor("mensaje") << "\n";
                }

                cout << "\n  Total en la nube: " << notificaciones.size() << "\n";
                break;
            }

            case 0:
                break;

            default:
                cout << "  Opcion invalida.\n";
        }

        if (opcion != 0) {
            pausar();
        }

    } while (opcion != 0 && !entradaCerrada());
}

// Estudiantes

void Menu::menuEstudiantes() {
    int opcion;

    do {
        encabezado("ESTUDIANTES");
        cout << "   1. Registrar estudiante (insertar)\n";
        cout << "   2. Buscar estudiante por carne\n";
        cout << "   3. Modificar datos de un estudiante\n";
        cout << "   4. Eliminar estudiante\n";
        cout << "   5. Listar estudiantes\n";
        cout << "   6. Ver la tabla hash (directorio y cubetas)\n";
        cout << "   0. Volver\n";

        opcion = leerOpcion();
        cout << "\n";

        switch (opcion) {
            case 1: {
                Estudiante estudiante;

                estudiante.carne = leerCarne();
                estudiante.nombre = leerTexto("Nombre completo");
                estudiante.correo = leerTexto("Correo");
                cout << "\n";

                sistema.estudiantes().registrar(estudiante);
                break;
            }

            case 2: {
                string carne = leerCarne();
                cout << "\n";
                sistema.estudiantes().mostrar(carne);
                break;
            }

            case 3: {
                string carne = leerCarne();
                Estudiante* actual = sistema.estudiantes().buscar(carne);

                if (actual == nullptr) {
                    cout << "\n  No existe un estudiante con el carne " << carne << ".\n";
                    break;
                }

                cout << "  Nombre actual: " << actual->nombre << "\n";
                cout << "  Correo actual: " << actual->correo << "\n";

                string nombre = leerTexto("Nombre nuevo (ENTER para dejarlo igual)");
                string correo = leerTexto("Correo nuevo (ENTER para dejarlo igual)");

                // Se vuelve a buscar por si el puntero cambio
                actual = sistema.estudiantes().buscar(carne);

                if (nombre == "") {
                    nombre = actual->nombre;
                }

                if (correo == "") {
                    correo = actual->correo;
                }

                cout << "\n";
                sistema.estudiantes().modificar(carne, nombre, correo);
                break;
            }

            case 4: {
                string carne = leerCarne();
                cout << "\n";
                sistema.eliminarEstudiante(carne);
                break;
            }

            case 5:
                sistema.estudiantes().listar();
                break;

            case 6:
                sistema.estudiantes().mostrarTabla();
                break;

            case 0:
                break;

            default:
                cout << "  Opcion invalida.\n";
        }

        if (opcion != 0) {
            pausar();
        }

    } while (opcion != 0 && !entradaCerrada());
}

// Plan de estudios

void Menu::menuPlan() {
    int opcion;

    do {
        encabezado("PLAN DE ESTUDIOS");
        cout << "   1. Ver el plan en orden (orden topologico)\n";
        cout << "   2. Ver el grafo (listas de adyacencia)\n";
        cout << "   3. Consultar un curso\n";
        cout << "   4. Cursos habilitados de un estudiante\n";
        cout << "   5. Agregar curso (insertar)\n";
        cout << "   6. Modificar curso\n";
        cout << "   7. Eliminar curso\n";
        cout << "   8. Agregar requisito\n";
        cout << "   9. Quitar requisito\n";
        cout << "   0. Volver\n";

        opcion = leerOpcion();
        cout << "\n";

        switch (opcion) {
            case 1:
                sistema.plan().mostrarPlan();
                break;

            case 2:
                sistema.plan().mostrarListas();
                break;

            case 3: {
                string codigo = leerCodigoCurso();
                cout << "\n";
                sistema.plan().mostrarCurso(codigo);
                break;
            }

            case 4: {
                string carne = leerCarne();
                cout << "\n";
                sistema.mostrarHabilitados(carne);
                break;
            }

            case 5: {
                Curso curso;

                curso.codigo = leerCodigoCurso();
                curso.nombre = leerTexto("Nombre");
                curso.creditos = leerEntero("Creditos");
                cout << "\n";

                sistema.plan().agregarCurso(curso);
                break;
            }

            case 6: {
                string codigo = leerCodigoCurso();

                if (!sistema.plan().existe(codigo)) {
                    cout << "\n  No existe el curso " << codigo << ".\n";
                    break;
                }

                string nombre = leerTexto("Nombre nuevo");
                int creditos = leerEntero("Creditos");
                cout << "\n";

                sistema.plan().modificarCurso(codigo, nombre, creditos);
                break;
            }

            case 7: {
                string codigo = leerCodigoCurso();
                cout << "\n";
                sistema.eliminarCurso(codigo);
                break;
            }

            case 8: {
                cout << "  Requisito\n";
                string requisito = leerCodigoCurso();
                cout << "  Curso que lo pide\n";
                string curso = leerCodigoCurso();
                cout << "\n";

                sistema.plan().agregarRequisito(requisito, curso);
                break;
            }

            case 9: {
                cout << "  Requisito\n";
                string requisito = leerCodigoCurso();
                cout << "  Curso que lo pide\n";
                string curso = leerCodigoCurso();
                cout << "\n";

                sistema.plan().quitarRequisito(requisito, curso);
                break;
            }

            case 0:
                break;

            default:
                cout << "  Opcion invalida.\n";
        }

        if (opcion != 0) {
            pausar();
        }

    } while (opcion != 0 && !entradaCerrada());
}

// Grupos y matricula

void Menu::menuMatricula() {
    int opcion;

    do {
        encabezado("GRUPOS Y MATRICULA");
        cout << "   1. Ver grupos del periodo\n";
        cout << "   2. Matricular un grupo\n";
        cout << "   3. Retirar un curso\n";
        cout << "   4. Salir de una lista de espera\n";
        cout << "   5. Matricula de un estudiante\n";
        cout << "   6. Ver listas de espera (colas)\n";
        cout << "   7. Abrir grupo\n";
        cout << "   8. Modificar cupo de un grupo\n";
        cout << "   9. Cerrar grupo\n";
        cout << "   0. Volver\n";

        opcion = leerOpcion();
        cout << "\n";

        ServicioMatricula& matricula = sistema.matricula();

        switch (opcion) {
            case 1:
                matricula.mostrarGrupos();
                break;

            case 2: {
                string carne = leerCarne();
                string curso = leerCodigoCurso();
                int numero = leerEntero("Numero de grupo");
                cout << "\n";

                ResultadoMatricula resultado = matricula.matricular(carne, curso, numero, sistema.horaActual());

                if (resultado == GRUPO_LLENO && leerSiNo("Desea entrar a la lista de espera")) {
                    cout << "\n";
                    matricula.unirseAEspera(carne, curso, numero, sistema.horaActual());
                }
                break;
            }

            case 3: {
                string carne = leerCarne();
                string curso = leerCodigoCurso();
                cout << "\n";

                matricula.retirar(carne, curso);
                break;
            }

            case 4: {
                string carne = leerCarne();
                string curso = leerCodigoCurso();
                int numero = leerEntero("Numero de grupo");
                cout << "\n";

                matricula.salirDeEspera(carne, curso, numero);
                break;
            }

            case 5: {
                string carne = leerCarne();
                cout << "\n";
                matricula.mostrarMatriculaDe(carne);
                break;
            }

            case 6:
                matricula.mostrarListasEspera();
                break;

            case 7: {
                Grupo grupo;

                grupo.codigoCurso = leerCodigoCurso();
                grupo.numero = leerEntero("Numero de grupo");
                grupo.dia = numeroDeDia(leerTexto("Dia (Lunes a Domingo)"));

                if (grupo.dia == 0) {
                    cout << "\n  Dia invalido.\n";
                    break;
                }

                if (!leerHoraDelDia("Hora de inicio", grupo.horaInicio) || !leerHoraDelDia("Hora de fin", grupo.horaFin)) {
                    break;
                }

                grupo.cupo = leerEntero("Cupo");
                cout << "\n";

                matricula.abrirGrupo(grupo);
                break;
            }

            case 8: {
                string curso = leerCodigoCurso();
                int numero = leerEntero("Numero de grupo");
                int cupo = leerEntero("Cupo nuevo");
                cout << "\n";

                matricula.modificarCupo(curso, numero, cupo);
                break;
            }

            case 9: {
                string curso = leerCodigoCurso();
                int numero = leerEntero("Numero de grupo");
                cout << "\n";

                matricula.cerrarGrupo(curso, numero);
                break;
            }

            case 0:
                break;

            default:
                cout << "  Opcion invalida.\n";
        }

        if (opcion != 0) {
            pausar();
        }

    } while (opcion != 0 && !entradaCerrada());
}

// Citas de matricula

void Menu::menuCitas() {
    int opcion;

    do {
        encabezado("CITAS DE MATRICULA");
        cout << "   1. Agenda de citas (recorrido en orden)\n";
        cout << "   2. Buscar cita por fecha y hora\n";
        cout << "   3. Consultar la cita de un estudiante\n";
        cout << "   4. Asignar cita (insertar)\n";
        cout << "   5. Reprogramar cita (modificar)\n";
        cout << "   6. Cancelar cita (eliminar)\n";
        cout << "   7. Citas en un rango de horas\n";
        cout << "   8. Validar si un estudiante ya puede matricular\n";
        cout << "   9. Asignar todas las citas por promedio\n";
        cout << "  10. Ver el arbol rojo-negro y sus propiedades\n";
        cout << "   0. Volver\n";

        opcion = leerOpcion();
        cout << "\n";

        ServicioCitas& citas = sistema.citas();

        switch (opcion) {
            case 1:
                citas.agenda();
                break;

            case 2: {
                long long fechaHora;

                if (leerFechaHora("Fecha y hora de la cita", fechaHora)) {
                    cout << "\n";
                    citas.buscarPorHora(fechaHora);
                }
                break;
            }

            case 3: {
                string carne = leerCarne();
                cout << "\n";
                citas.consultarDe(carne);
                break;
            }

            case 4: {
                string carne = leerCarne();
                long long fechaHora;

                if (leerFechaHora("Fecha y hora de la cita", fechaHora)) {
                    cout << "\n";
                    citas.asignar(fechaHora, carne);
                }
                break;
            }

            case 5: {
                string carne = leerCarne();
                long long fechaHora;

                if (leerFechaHora("Nueva fecha y hora", fechaHora)) {
                    cout << "\n";
                    citas.reprogramar(carne, fechaHora);
                }
                break;
            }

            case 6: {
                long long fechaHora;

                if (leerFechaHora("Fecha y hora de la cita a cancelar", fechaHora)) {
                    cout << "\n";
                    citas.cancelar(fechaHora);
                }
                break;
            }

            case 7: {
                long long desde;
                long long hasta;

                if (leerFechaHora("Desde", desde) && leerFechaHora("Hasta", hasta)) {
                    cout << "\n";
                    citas.rango(desde, hasta);
                }
                break;
            }

            case 8: {
                string carne = leerCarne();
                cout << "\n";
                citas.validar(carne, sistema.horaActual());
                break;
            }

            case 9: {
                long long inicio;

                cout << "  Se reemplazan todas las citas: mayor promedio, cita mas temprana.\n";

                if (!leerFechaHora("Primera cita", inicio)) {
                    break;
                }

                int minutos = leerEntero("Minutos entre citas");
                cout << "\n";

                if (leerSiNo("Confirmar")) {
                    cout << "\n";
                    sistema.asignarCitasPorPromedio(inicio, minutos);
                }
                break;
            }

            case 10:
                citas.mostrarArbol();
                break;

            case 0:
                break;

            default:
                cout << "  Opcion invalida.\n";
        }

        if (opcion != 0) {
            pausar();
        }

    } while (opcion != 0 && !entradaCerrada());
}

// Historial academico

void Menu::menuHistorial() {
    int opcion;

    do {
        encabezado("HISTORIAL ACADEMICO");
        cout << "   1. Historial de un estudiante (con promedio)\n";
        cout << "   2. Notas en un rango de periodos\n";
        cout << "   3. Buscar una nota (ruta en el arbol)\n";
        cout << "   4. Registrar nota (insertar)\n";
        cout << "   5. Corregir nota (modificar)\n";
        cout << "   6. Eliminar nota\n";
        cout << "   7. Ver el arbol B+ por niveles\n";
        cout << "   8. Ver las hojas enlazadas\n";
        cout << "   0. Volver\n";

        opcion = leerOpcion();
        cout << "\n";

        ServicioHistorial& historial = sistema.historial();

        switch (opcion) {
            case 1: {
                string carne = leerCarne();
                cout << "\n";
                historial.mostrarHistorial(carne);
                break;
            }

            case 2: {
                string carne = leerCarne();
                string desde;
                string hasta;

                if (leerPeriodo("Desde el periodo", desde) && leerPeriodo("Hasta el periodo", hasta)) {
                    cout << "\n";
                    historial.mostrarRango(carne, desde, hasta);
                }
                break;
            }

            case 3: {
                string carne = leerCarne();
                string periodo;

                if (!leerPeriodo("Periodo", periodo)) {
                    break;
                }

                string curso = leerCodigoCurso();
                cout << "\n";

                historial.mostrarNota(carne, periodo, curso);
                break;
            }

            case 4: {
                RegistroNota registro;

                registro.carne = leerCarne();

                if (!leerPeriodo("Periodo", registro.periodo)) {
                    break;
                }

                registro.codigoCurso = leerCodigoCurso();
                registro.nota = leerEntero("Nota (0 a 100)");
                cout << "\n";

                historial.registrar(registro);
                break;
            }

            case 5: {
                string carne = leerCarne();
                string periodo;

                if (!leerPeriodo("Periodo", periodo)) {
                    break;
                }

                string curso = leerCodigoCurso();
                int nota = leerEntero("Nota nueva (0 a 100)");
                cout << "\n";

                historial.corregir(carne, periodo, curso, nota);
                break;
            }

            case 6: {
                string carne = leerCarne();
                string periodo;

                if (!leerPeriodo("Periodo", periodo)) {
                    break;
                }

                string curso = leerCodigoCurso();
                cout << "\n";

                historial.eliminar(carne, periodo, curso);
                break;
            }

            case 7:
                historial.mostrarArbol();
                break;

            case 8:
                historial.mostrarHojas();
                break;

            case 0:
                break;

            default:
                cout << "  Opcion invalida.\n";
        }

        if (opcion != 0) {
            pausar();
        }

    } while (opcion != 0 && !entradaCerrada());
}
