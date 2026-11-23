// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del sistema de matricula

#include <iostream>
#include <iomanip>
#include <algorithm>
#include "app/SistemaMatricula.h"
#include "comun/Consola.h"

using namespace std;

SistemaMatricula::SistemaMatricula()
    : servicioHistorial(servicioEstudiantes, servicioPlan),
      servicioCitas(servicioEstudiantes),
      servicioMatricula(servicioEstudiantes, servicioPlan, servicioHistorial, servicioCitas) {
    // La matricula abre el 7 de diciembre de 2026 a las 08:00
    hora = armarFechaHora(2026, 12, 7, 8, 0);
}

ServicioEstudiantes& SistemaMatricula::estudiantes() {
    return servicioEstudiantes;
}

ServicioPlan& SistemaMatricula::plan() {
    return servicioPlan;
}

ServicioHistorial& SistemaMatricula::historial() {
    return servicioHistorial;
}

ServicioCitas& SistemaMatricula::citas() {
    return servicioCitas;
}

ServicioMatricula& SistemaMatricula::matricula() {
    return servicioMatricula;
}

long long SistemaMatricula::horaActual() {
    return hora;
}

void SistemaMatricula::cambiarHoraActual(long long fechaHora) {
    hora = fechaHora;
}

// Carga de datos

// Se vacia en orden inverso: primero lo que depende de los demas
void SistemaMatricula::vaciar() {
    servicioMatricula.vaciar();
    servicioCitas.vaciar();
    servicioHistorial.vaciar();
    servicioPlan.vaciar();
    servicioEstudiantes.vaciar();
}

bool SistemaMatricula::cargarDatos(IFuenteDatos& fuente) {
    DatosIniciales datos;

    cout << "  Leyendo " << fuente.nombre() << "...\n";

    if (!fuente.leer(datos)) {
        cout << "  No se pudieron leer los datos.\n";
        return false;
    }

    cargar(datos);
    return true;
}

static void avisoRechazo(string que, string motivo) {
    cout << "  Aviso: " << que << " rechazado, " << motivo << "\n";
}

// Cada componente valida lo suyo: se cargan en orden para que las
// referencias (carne, curso, grupo) ya existan cuando se necesitan
void SistemaMatricula::cargar(DatosIniciales& datos) {
    vaciar();

    int rechazados = 0;

    for (int i = 0; i < (int)datos.estudiantes.size(); i++) {
        if (!servicioEstudiantes.cargar(datos.estudiantes[i])) {
            avisoRechazo("estudiante " + datos.estudiantes[i].carne, "carne repetido o invalido");
            rechazados++;
        }
    }

    for (int i = 0; i < (int)datos.cursos.size(); i++) {
        if (!servicioPlan.cargarCurso(datos.cursos[i])) {
            avisoRechazo("curso " + datos.cursos[i].codigo, "codigo repetido");
            rechazados++;
        }
    }

    for (int i = 0; i < (int)datos.requisitos.size(); i++) {
        Requisito& requisito = datos.requisitos[i];

        if (servicioPlan.cargarRequisito(requisito.requisito, requisito.curso) != REQUISITO_AGREGADO) {
            avisoRechazo("requisito " + requisito.requisito + " -> " + requisito.curso, "curso inexistente, repetido o ciclo");
            rechazados++;
        }
    }

    for (int i = 0; i < (int)datos.notas.size(); i++) {
        RegistroNota& nota = datos.notas[i];

        if (!servicioEstudiantes.existe(nota.carne) || !servicioPlan.existe(nota.codigoCurso)
            || !servicioHistorial.cargar(nota)) {
            avisoRechazo("nota " + nota.carne + " " + nota.periodo + " " + nota.codigoCurso, "estudiante o curso inexistente, o repetida");
            rechazados++;
        }
    }

    for (int i = 0; i < (int)datos.citas.size(); i++) {
        ResultadoCita resultado = servicioCitas.cargar(datos.citas[i]);

        if (resultado == CITA_SIN_ESTUDIANTE) {
            avisoRechazo("cita de " + datos.citas[i].carne, "no existe el estudiante");
            rechazados++;
        } else if (resultado == CITA_YA_TIENE) {
            avisoRechazo("cita de " + datos.citas[i].carne, "ya tiene cita");
            rechazados++;
        } else if (resultado == CITA_HORA_OCUPADA) {
            avisoRechazo("cita de " + datos.citas[i].carne, "la hora ya esta ocupada");
            rechazados++;
        }
    }

    for (int i = 0; i < (int)datos.grupos.size(); i++) {
        Grupo& grupo = datos.grupos[i];

        if (!servicioMatricula.cargarGrupo(grupo)) {
            avisoRechazo("grupo " + grupo.codigoCurso + "-" + to_string(grupo.numero), "curso inexistente, repetido u otro periodo");
            rechazados++;
        }
    }

    for (int i = 0; i < (int)datos.matriculas.size(); i++) {
        Matricula& m = datos.matriculas[i];

        if (!servicioMatricula.cargarMatricula(m)) {
            avisoRechazo("matricula de " + m.carne + " en " + m.codigoCurso, "grupo o estudiante inexistente, sin cupo o repetida");
            rechazados++;
        }
    }

    for (int i = 0; i < (int)datos.espera.size(); i++) {
        EnEspera& e = datos.espera[i];

        if (!servicioMatricula.cargarEspera(e.codigoCurso, e.numeroGrupo, e.periodo, e.carne)) {
            avisoRechazo("espera de " + e.carne + " en " + e.codigoCurso, "grupo o estudiante inexistente, o repetido");
            rechazados++;
        }
    }

    vector<Grupo> grupos;
    servicioMatricula.grupos(grupos);

    cout << "  Estudiantes: " << servicioEstudiantes.cantidad()
         << "   Cursos: " << servicioPlan.cantidadCursos()
         << "   Requisitos: " << servicioPlan.cantidadRequisitos()
         << "   Notas: " << servicioHistorial.cantidad() << "\n";
    cout << "  Citas: " << servicioCitas.cantidad()
         << "   Grupos: " << grupos.size()
         << "   Datos rechazados: " << rechazados << "\n";
}

// Exporta el estado actual con la misma forma de los datos iniciales
void SistemaMatricula::exportar(DatosIniciales& datos) {
    servicioEstudiantes.todos(datos.estudiantes);
    servicioPlan.cursos(datos.cursos);

    vector<string> origen;
    vector<string> destino;
    servicioPlan.requisitos(origen, destino);

    for (int i = 0; i < (int)origen.size(); i++) {
        Requisito requisito;
        requisito.requisito = origen[i];
        requisito.curso = destino[i];
        datos.requisitos.push_back(requisito);
    }

    servicioHistorial.todas(datos.notas);
    servicioCitas.todas(datos.citas);
    servicioMatricula.grupos(datos.grupos);
    servicioMatricula.matriculas(datos.matriculas);

    vector<string> cursos;
    vector<int> numeros;
    vector<string> carnes;
    servicioMatricula.esperas(cursos, numeros, carnes);

    for (int i = 0; i < (int)cursos.size(); i++) {
        EnEspera registro;
        registro.codigoCurso = cursos[i];
        registro.numeroGrupo = numeros[i];
        registro.periodo = servicioMatricula.periodo();
        registro.carne = carnes[i];
        datos.espera.push_back(registro);
    }
}

// Operaciones que tocan a varios componentes

// No se borra a quien tiene historial o cursos matriculados: se perderia
// informacion de otros componentes. Si se puede, tambien se le quita la cita
// y se le saca de las listas de espera.
bool SistemaMatricula::eliminarEstudiante(string carne) {
    if (!servicioEstudiantes.existe(carne)) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return false;
    }

    if (servicioHistorial.tieneNotas(carne)) {
        cout << "  No se puede eliminar: tiene historial academico.\n";
        return false;
    }

    if (servicioMatricula.tieneMatriculasActivas(carne)) {
        cout << "  No se puede eliminar: tiene cursos matriculados. Debe retirarlos primero.\n";
        return false;
    }

    if (servicioCitas.cancelarDe(carne)) {
        cout << "  Se cancelo su cita de matricula.\n";
    }

    servicioMatricula.sacarDeTodasLasColas(carne);

    return servicioEstudiantes.eliminar(carne);
}

bool SistemaMatricula::eliminarCurso(string codigo) {
    if (!servicioPlan.existe(codigo)) {
        cout << "  No existe el curso " << codigo << ".\n";
        return false;
    }

    if (servicioMatricula.hayGruposDelCurso(codigo)) {
        cout << "  No se puede eliminar: tiene grupos abiertos.\n";
        return false;
    }

    if (servicioHistorial.hayNotasDelCurso(codigo)) {
        cout << "  No se puede eliminar: hay notas registradas de ese curso.\n";
        return false;
    }

    return servicioPlan.eliminarCurso(codigo);
}

// CU-02: cursos no aprobados con todos los requisitos aprobados
void SistemaMatricula::mostrarHabilitados(string carne) {
    if (!servicioEstudiantes.existe(carne)) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    vector<string> aprobados;
    servicioHistorial.aprobados(carne, aprobados);

    vector<string> habilitados;
    servicioPlan.habilitados(aprobados, habilitados);

    cout << "  " << servicioEstudiantes.nombreDe(carne) << " (" << carne << ")\n";
    cout << "  Cursos aprobados: " << aprobados.size() << "\n\n";

    if (habilitados.size() == 0) {
        cout << "  No tiene cursos habilitados.\n";
        return;
    }

    vector<Grupo> grupos;
    servicioMatricula.grupos(grupos);

    cout << "  Curso      Grupos abiertos   Nombre\n";
    cout << "  ----------------------------------------------------------\n";

    for (int i = 0; i < (int)habilitados.size(); i++) {
        int abiertos = 0;

        for (int j = 0; j < (int)grupos.size(); j++) {
            if (grupos[j].codigoCurso == habilitados[i]) {
                abiertos++;
            }
        }

        cout << "  ";
        columna(habilitados[i], 11);
        columna(to_string(abiertos), 18);
        cout << servicioPlan.nombreDe(habilitados[i]) << "\n";
    }
}

// CU-16 y RN-10: mayor promedio, cita mas temprana.
// Los estudiantes sin notas van al final, en orden de carne.
struct Candidato {
    string carne;
    double promedio;
    bool tieneNotas;
};

void SistemaMatricula::asignarCitasPorPromedio(long long inicio, int minutosEntreCitas) {
    if (minutosEntreCitas <= 0) {
        cout << "  Los minutos entre citas deben ser mayores que cero.\n";
        return;
    }

    vector<Estudiante> lista;
    servicioEstudiantes.todos(lista);

    if (lista.size() == 0) {
        cout << "  No hay estudiantes registrados.\n";
        return;
    }

    vector<Candidato> candidatos;

    for (int i = 0; i < (int)lista.size(); i++) {
        Candidato candidato;

        candidato.carne = lista[i].carne;
        candidato.tieneNotas = servicioHistorial.promedio(lista[i].carne, candidato.promedio);
        candidatos.push_back(candidato);
    }

    // stable_sort deja en orden de carne a los que empatan
    stable_sort(candidatos.begin(), candidatos.end(), [](const Candidato& a, const Candidato& b) {
        if (a.tieneNotas != b.tieneNotas) {
            return a.tieneNotas;
        }

        return a.promedio > b.promedio;
    });

    // Las citas anteriores se reemplazan
    servicioCitas.vaciar();

    cout << "  Hora               Carne       Promedio  Estudiante\n";
    cout << "  ----------------------------------------------------------\n";

    long long fechaHora = inicio;

    for (int i = 0; i < (int)candidatos.size(); i++) {
        CitaMatricula cita;

        cita.fechaHora = fechaHora;
        cita.carne = candidatos[i].carne;
        servicioCitas.cargar(cita);

        cout << "  ";
        imprimirFechaHora(fechaHora);
        cout << "   " << candidatos[i].carne << "   ";

        if (candidatos[i].tieneNotas) {
            cout << fixed << setprecision(1) << setw(6) << candidatos[i].promedio << "    ";
            cout.unsetf(ios::fixed);
        } else {
            cout << "  -       ";
        }

        cout << servicioEstudiantes.nombreDe(candidatos[i].carne) << "\n";

        fechaHora = sumarMinutos(fechaHora, minutosEntreCitas);
    }

    cout << "\n  Citas asignadas: " << servicioCitas.cantidad() << "\n";
}
