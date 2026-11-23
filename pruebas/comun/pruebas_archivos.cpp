// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas de la lectura de archivos

#include <iostream>
#include <vector>
#include "Pruebas.h"
#include "comun/CargadorArchivos.h"

using namespace std;

// Lectura de archivos

void probarArchivos() {
    cout << "\nLectura de archivos\n";

    vector<Estudiante> estudiantes;
    vector<CitaMatricula> citas;
    int errores = 0;

    revisar(leerEstudiantes("datos/estudiantes.txt", estudiantes, errores), "abre estudiantes.txt");
    revisar(estudiantes.size() == 14 && errores == 0, "lee 14 estudiantes sin errores");
    revisar(estudiantes[0].carne == "2026-0101" && estudiantes[0].nombre == "Ana Mora Solis", "primer estudiante correcto");

    revisar(leerCitas("datos/citas.txt", citas, errores), "abre citas.txt");
    revisar(citas.size() == 12 && errores == 0, "lee 12 citas sin errores");
    revisar(citas[0].fechaHora == 202612070800LL && citas[0].carne == "2026-0103", "primera cita correcta");

    estudiantes.clear();
    citas.clear();

    revisar(!leerEstudiantes("no_existe.txt", estudiantes, errores), "archivo inexistente devuelve false");

    cout << "  (los avisos que siguen son de los archivos con errores)\n";

    revisar(leerEstudiantes("pruebas/datos_malos/estudiantes_malos.txt", estudiantes, errores), "abre estudiantes_malos.txt");
    revisar(estudiantes.size() == 2 && errores == 3, "2 estudiantes buenos y 3 lineas con error");

    revisar(leerCitas("pruebas/datos_malos/citas_malas.txt", citas, errores), "abre citas_malas.txt");
    revisar(citas.size() == 2 && errores == 5, "2 citas buenas y 5 lineas con error");
}

// Archivos de cursos, requisitos, grupos, notas, matriculas y espera

void probarArchivosDelPlan() {
    cout << "\nLectura de los archivos del plan de estudios\n";

    vector<Curso> cursos;
    vector<Requisito> requisitos;
    vector<Grupo> grupos;
    vector<RegistroNota> notas;
    vector<Matricula> matriculas;
    vector<EnEspera> espera;
    int errores = 0;

    revisar(leerCursos("datos/cursos.txt", cursos, errores) && cursos.size() == 9 && errores == 0, "lee 9 cursos sin errores");
    revisar(cursos[5].codigo == "INF-301" && cursos[5].creditos == 4, "INF-301 con 4 creditos");

    revisar(leerRequisitos("datos/requisitos.txt", requisitos, errores) && requisitos.size() == 10 && errores == 0, "lee 10 requisitos sin errores");
    revisar(requisitos[0].requisito == "INF-101" && requisitos[0].curso == "INF-102", "INF-101 es requisito de INF-102");

    revisar(leerGrupos("datos/grupos.txt", grupos, errores) && grupos.size() == 10 && errores == 0, "lee 10 grupos sin errores");
    revisar(grupos[5].codigoCurso == "INF-301" && grupos[5].dia == 1 && grupos[5].horaInicio == 1800 && grupos[5].horaFin == 2100 && grupos[5].cupo == 2, "INF-301 grupo 1: lunes 18:00-21:00, cupo 2");

    revisar(leerNotas("datos/notas.txt", notas, errores) && notas.size() == 39 && errores == 0, "lee 39 notas sin errores");
    revisar(notas[6].carne == "2026-0102" && notas[6].codigoCurso == "MAT-101" && notas[6].nota == 60, "Daniel reprobo MAT-101 con 60");

    revisar(leerMatriculas("datos/matriculas.txt", matriculas, errores) && matriculas.size() == 0, "matriculas.txt empieza vacio");
    revisar(leerEspera("datos/espera.txt", espera, errores) && espera.size() == 0, "espera.txt empieza vacio");

    cout << "  (los avisos que siguen son de los archivos con errores)\n";

    grupos.clear();
    notas.clear();
    requisitos.clear();

    revisar(leerGrupos("pruebas/datos_malos/grupos_malos.txt", grupos, errores) && grupos.size() == 2 && errores == 5, "2 grupos buenos y 5 lineas con error");
    revisar(grupos[1].dia == 6, "acepta el dia en minuscula (sabado)");

    revisar(leerNotas("pruebas/datos_malos/notas_malas.txt", notas, errores) && notas.size() == 2 && errores == 4, "2 notas buenas y 4 lineas con error");

    revisar(leerRequisitos("pruebas/datos_malos/requisitos_malos.txt", requisitos, errores) && requisitos.size() == 2 && errores == 3, "2 requisitos buenos y 3 lineas con error");
}

// Funciones de grupo, horario y periodo

Grupo grupoDe(string periodo, int dia, int inicio, int fin) {
    Grupo grupo;

    grupo.codigoCurso = "INF-000";
    grupo.numero = 1;
    grupo.periodo = periodo;
    grupo.dia = dia;
    grupo.horaInicio = inicio;
    grupo.horaFin = fin;
    grupo.cupo = 2;
    grupo.matriculados = 0;

    return grupo;
}

void probarDominio() {
    cout << "\nGrupos, horarios y periodos\n";

    Grupo lunesNoche = grupoDe("2027-C1", 1, 1800, 2100);

    revisar(hayChoque(lunesNoche, grupoDe("2027-C1", 1, 1800, 2100)), "mismo horario: choque");
    revisar(hayChoque(lunesNoche, grupoDe("2027-C1", 1, 2000, 2200)), "se traslapan una hora: choque");
    revisar(!hayChoque(lunesNoche, grupoDe("2027-C1", 1, 2100, 2300)), "uno empieza cuando el otro termina: sin choque");
    revisar(!hayChoque(lunesNoche, grupoDe("2027-C1", 4, 1800, 2100)), "otro dia: sin choque");
    revisar(!hayChoque(lunesNoche, grupoDe("2027-C2", 1, 1800, 2100)), "otro periodo: sin choque");

    revisar(tieneCupo(lunesNoche), "grupo vacio tiene cupo");
    lunesNoche.matriculados = 2;
    revisar(!tieneCupo(lunesNoche), "grupo con 2 de 2 no tiene cupo");

    revisar(numeroDeDia("Miercoles") == 3 && numeroDeDia("DOMINGO") == 7 && numeroDeDia("feriado") == 0, "nombres de dia a numero");
    revisar(leerHora("08:30") == 830 && leerHora("24:00") == -1 && leerHora("8:30") == -1, "lectura de horas HH:MM");

    revisar(periodoValido("2026-C3") && !periodoValido("2026-C4") && !periodoValido("26-C1"), "periodos validos e invalidos");
    revisar(armarClaveNota("2026-0101", "2026-C1", "INF-101") == "2026-0101|2026-C1|INF-101", "clave compuesta de la nota");

    // El orden de las claves sigue carne, luego periodo, luego curso
    revisar(armarClaveNota("2026-0101", "2026-C3", "INF-202") < armarClaveNota("2026-0102", "2026-C1", "INF-101"), "la clave ordena primero por carne");
    revisar(armarClaveNota("2026-0101", "2026-C1", "MAT-101") < armarClaveNota("2026-0101", "2026-C2", "INF-102"), "luego por periodo");
}
