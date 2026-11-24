// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas del sistema integrado: reglas de matricula entre componentes

#include <iostream>
#include <sstream>
#include <vector>
#include "Pruebas.h"
#include "app/SistemaMatricula.h"

using namespace std;

// Apoyo

// Silenciar la pantalla
class Silencio {
public:
    Silencio() {
        anterior = cout.rdbuf(descarte.rdbuf());
    }

    ~Silencio() {
        cout.rdbuf(anterior);
    }

private:
    stringstream descarte;
    streambuf* anterior;
};

long long a(int hora, int minuto) {
    return armarFechaHora(2026, 12, 7, hora, minuto);
}

void cargarArchivos(SistemaMatricula& sistema) {
    Silencio silencio;
    FuenteArchivos archivos("datos/");
    sistema.cargarDatos(archivos);
}

int matriculadosEn(SistemaMatricula& sistema, string curso, int numero) {
    Grupo* grupo = sistema.matricula().buscarGrupo(curso, numero);

    if (grupo == nullptr) {
        return -1;
    }

    return grupo->matriculados;
}

// Carga desde archivos

void probarSistemaCarga() {
    cout << "\nSistema: carga de los datos iniciales\n";

    SistemaMatricula sistema;
    cargarArchivos(sistema);

    vector<Grupo> grupos;
    sistema.matricula().grupos(grupos);

    revisar(sistema.estudiantes().cantidad() == 14 && sistema.plan().cantidadCursos() == 9, "14 estudiantes y 9 cursos");
    revisar(sistema.plan().cantidadRequisitos() == 10 && sistema.historial().cantidad() == 39, "10 requisitos y 39 notas");
    revisar(sistema.citas().cantidad() == 12 && grupos.size() == 10, "12 citas y 10 grupos");

    // Cita guardada en el estudiante
    revisar(sistema.estudiantes().buscar("2026-0103")->cita == a(8, 0), "la cita de 0103 queda en su registro: 08:00");
    revisar(sistema.estudiantes().buscar("2026-0113")->cita == 0, "0113 no tiene cita");

    FuenteArchivos inexistente("no_existe/");
    bool cargo;

    {
        Silencio silencio;
        cargo = sistema.cargarDatos(inexistente);
    }

    revisar(!cargo, "una carpeta sin archivos no se puede cargar");
}

// Escenario de matricula del diseno

void probarSistemaEscenario() {
    cout << "\nSistema: escenario de matricula del diseno\n";

    SistemaMatricula sistema;
    cargarArchivos(sistema);

    ServicioMatricula& matricula = sistema.matricula();
    Silencio silencio;

    sistema.cambiarHoraActual(a(8, 15));
    ResultadoMatricula sofia = matricula.matricular("2026-0105", "INF-301", 1, sistema.horaActual());

    sistema.cambiarHoraActual(a(8, 30));
    ResultadoMatricula ana = matricula.matricular("2026-0101", "INF-301", 1, sistema.horaActual());
    ResultadoMatricula anaChoque = matricula.matricular("2026-0101", "INF-302", 1, sistema.horaActual());
    ResultadoMatricula anaOtroGrupo = matricula.matricular("2026-0101", "INF-302", 2, sistema.horaActual());

    sistema.cambiarHoraActual(a(8, 45));
    ResultadoMatricula jose = matricula.matricular("2026-0104", "INF-301", 1, sistema.horaActual());
    bool joseEspera = matricula.unirseAEspera("2026-0104", "INF-301", 1, sistema.horaActual());
    bool joseDosVeces = matricula.unirseAEspera("2026-0104", "INF-301", 1, sistema.horaActual());

    sistema.cambiarHoraActual(a(9, 0));
    ResultadoMatricula daniel = matricula.matricular("2026-0102", "INF-301", 1, sistema.horaActual());

    sistema.cambiarHoraActual(a(9, 10));
    bool anaRetira = matricula.retirar("2026-0101", "INF-301");

    revisar(sofia == MATRICULADO, "08:15 Sofia matricula INF-301 grupo 1");
    revisar(ana == MATRICULADO, "08:30 Ana matricula INF-301 grupo 1 y el grupo se llena");
    revisar(anaChoque == CHOQUE_HORARIO, "Ana no puede INF-302 grupo 1: choque el lunes (RN-04)");
    revisar(anaOtroGrupo == MATRICULADO, "Ana matricula INF-302 grupo 2 (jueves)");
    revisar(jose == GRUPO_LLENO && joseEspera, "08:45 Jose encuentra el grupo lleno y entra a la lista de espera");
    revisar(!joseDosVeces, "Jose no puede entrar dos veces a la misma cola (RN-06)");
    revisar(daniel == FALTAN_REQUISITOS, "09:00 Daniel no puede: le falta INF-201 (RN-02)");
    revisar(anaRetira, "09:10 Ana retira INF-301");
    revisar(matriculadosEn(sistema, "INF-301", 1) == 2 && matricula.tieneMatriculasActivas("2026-0104"),
            "el cupo pasa a Jose y el grupo vuelve a 2 de 2 (RN-07)");

    vector<string> cursos;
    vector<int> numeros;
    vector<string> carnes;
    matricula.esperas(cursos, numeros, carnes);
    revisar(carnes.size() == 0, "la lista de espera queda vacia");
}

// Las demas reglas

void probarSistemaReglas() {
    cout << "\nSistema: cita, cursos aprobados y lista de espera\n";

    SistemaMatricula sistema;
    cargarArchivos(sistema);

    ServicioMatricula& matricula = sistema.matricula();
    Silencio silencio;

    // RN-01: la cita de Daniel es a las 09:00
    sistema.cambiarHoraActual(a(8, 0));
    revisar(matricula.matricular("2026-0102", "INF-202", 1, sistema.horaActual()) == CITA_NO_HABILITADA, "a las 08:00 la cita de Daniel (09:00) no esta habilitada (RN-01)");
    revisar(matricula.matricular("2026-0113", "INF-101", 1, sistema.horaActual()) == SIN_CITA, "un estudiante sin cita no puede matricular");

    sistema.cambiarHoraActual(a(12, 0));

    // RN-03
    revisar(matricula.matricular("2026-0101", "INF-201", 1, sistema.horaActual()) == YA_APROBADO, "Ana no puede repetir INF-201, ya lo aprobo (RN-03)");
    revisar(matricula.matricular("2026-0105", "INF-301", 1, sistema.horaActual()) == MATRICULADO, "Sofia matricula INF-301");
    revisar(matricula.matricular("2026-0105", "INF-301", 1, sistema.horaActual()) == YA_MATRICULADO, "no puede matricular dos veces el mismo curso (RN-03)");
    revisar(matricula.matricular("2026-0105", "INF-999", 1, sistema.horaActual()) == SIN_GRUPO, "un grupo que no existe se rechaza");
    revisar(matricula.matricular("2026-0999", "INF-301", 1, sistema.horaActual()) == SIN_ESTUDIANTE, "un carne que no existe se rechaza");

    // RN-07
    matricula.matricular("2026-0101", "INF-301", 1, sistema.horaActual());
    matricula.matricular("2026-0104", "INF-301", 1, sistema.horaActual());
    matricula.unirseAEspera("2026-0104", "INF-301", 1, sistema.horaActual());

    // Ana espera otro cupo
    matricula.retirar("2026-0101", "INF-301");
    revisar(matricula.unirseAEspera("2026-0101", "INF-301", 1, sistema.horaActual()), "Ana vuelve a la lista de espera");

    // Ana matricula otro curso con choque
    matricula.matricular("2026-0101", "INF-302", 1, sistema.horaActual());
    matricula.modificarCupo("INF-301", 1, 3);

    revisar(matriculadosEn(sistema, "INF-301", 1) == 2, "al subir el cupo, Ana no recibe el cupo porque choca con INF-302");
    revisar(matricula.tieneMatriculasActivas("2026-0101"), "Ana sigue con INF-302");

    vector<string> cursos;
    vector<int> numeros;
    vector<string> carnes;
    matricula.esperas(cursos, numeros, carnes);
    revisar(carnes.size() == 0, "y sale de la lista de espera");

    revisar(!matricula.modificarCupo("INF-301", 1, 1), "el cupo no puede quedar por debajo de los matriculados");
    revisar(!matricula.cerrarGrupo("INF-301", 1), "no se cierra un grupo con matriculados");
    revisar(matricula.cerrarGrupo("INF-401", 1), "si se cierra un grupo vacio");
}

// Operaciones que tocan a varios componentes

void probarSistemaBorrados() {
    cout << "\nSistema: borrados que cruzan componentes\n";

    SistemaMatricula sistema;
    cargarArchivos(sistema);

    Silencio silencio;

    revisar(!sistema.eliminarEstudiante("2026-0101"), "no se elimina a Ana: tiene historial");

    Estudiante nuevo;
    nuevo.carne = "2026-0200";
    nuevo.nombre = "Estudiante Nuevo";
    nuevo.correo = "nuevo@correo.com";

    sistema.estudiantes().registrar(nuevo);
    sistema.citas().asignar(a(15, 0), "2026-0200");

    revisar(sistema.citas().cantidad() == 13, "el estudiante nuevo tiene cita");
    revisar(sistema.eliminarEstudiante("2026-0200"), "se elimina al estudiante nuevo");
    revisar(sistema.citas().cantidad() == 12 && !sistema.estudiantes().existe("2026-0200"), "y su cita tambien se cancela");

    revisar(!sistema.eliminarCurso("INF-301"), "no se elimina INF-301: tiene grupos");

    Curso curso;
    curso.codigo = "MAT-201";
    curso.nombre = "Calculo";
    curso.creditos = 4;

    sistema.plan().agregarCurso(curso);
    sistema.plan().agregarRequisito("MAT-101", "MAT-201");

    revisar(sistema.eliminarCurso("MAT-201") && sistema.plan().cantidadRequisitos() == 10, "se elimina un curso sin grupos ni notas, con su requisito");
}

// CU-16: citas por promedio

void probarSistemaCitasPorPromedio() {
    cout << "\nSistema: asignacion de citas por promedio\n";

    SistemaMatricula sistema;
    cargarArchivos(sistema);

    vector<CitaMatricula> originales;
    sistema.citas().todas(originales);

    {
        Silencio silencio;
        sistema.asignarCitasPorPromedio(a(8, 0), 15);
    }

    vector<CitaMatricula> nuevas;
    sistema.citas().todas(nuevas);

    // Citas del archivo
    bool iguales = nuevas.size() == 14;

    for (int i = 0; i < 12 && iguales; i++) {
        if (nuevas[i].carne != originales[i].carne || nuevas[i].fechaHora != originales[i].fechaHora) {
            iguales = false;
        }
    }

    revisar(iguales, "las 12 primeras citas coinciden con el orden por promedio del archivo");
    revisar(nuevas[12].carne == "2026-0113" && nuevas[13].carne == "2026-0114", "los dos estudiantes sin notas van al final");
    revisar(nuevas[13].fechaHora == a(11, 15), "la ultima cita es a las 11:15");
    revisar(sistema.estudiantes().buscar("2026-0114")->cita == a(11, 15), "y queda en el registro del estudiante");

    {
        Silencio silencio;
        sistema.asignarCitasPorPromedio(armarFechaHora(2026, 12, 7, 23, 30), 30);
    }

    revisar(sistema.estudiantes().buscar("2026-0105")->cita == armarFechaHora(2026, 12, 8, 0, 0), "si pasa de medianoche sigue al dia siguiente");
}

// Exportar y volver a cargar

void probarSistemaExportar() {
    cout << "\nSistema: exportar y volver a cargar\n";

    SistemaMatricula sistema;
    cargarArchivos(sistema);

    {
        Silencio silencio;
        sistema.cambiarHoraActual(a(12, 0));
        sistema.matricula().matricular("2026-0105", "INF-301", 1, sistema.horaActual());
        sistema.matricula().matricular("2026-0101", "INF-301", 1, sistema.horaActual());
        sistema.matricula().matricular("2026-0104", "INF-301", 1, sistema.horaActual());
        sistema.matricula().unirseAEspera("2026-0104", "INF-301", 1, sistema.horaActual());
    }

    DatosIniciales datos;
    sistema.exportar(datos);

    SistemaMatricula copia;

    {
        Silencio silencio;
        copia.cargar(datos);
    }

    vector<Matricula> matriculas;
    copia.matricula().matriculas(matriculas);

    vector<string> cursos;
    vector<int> numeros;
    vector<string> carnes;
    copia.matricula().esperas(cursos, numeros, carnes);

    revisar(copia.estudiantes().cantidad() == 14 && copia.historial().cantidad() == 39 && copia.citas().cantidad() == 12,
            "la copia tiene los mismos estudiantes, notas y citas");
    revisar(copia.plan().cantidadRequisitos() == 10, "y los mismos requisitos");
    revisar(matriculas.size() == 2 && matriculadosEn(copia, "INF-301", 1) == 2, "las 2 matriculas y el cupo se conservan");
    revisar(carnes.size() == 1 && carnes[0] == "2026-0104", "la lista de espera se conserva");
}
