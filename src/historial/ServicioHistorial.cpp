// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del componente del historial academico

#include <iostream>
#include <iomanip>
#include "historial/ServicioHistorial.h"
#include "comun/Consola.h"
#include "comun/Colecciones.h"

using namespace std;

ServicioHistorial::ServicioHistorial(ServicioEstudiantes& estudiantes, ServicioPlan& plan)
    : estudiantes(estudiantes), plan(plan) {
}

bool ServicioHistorial::cargar(RegistroNota registro) {
    return arbol.insertar(registro);
}

// Registro de notas

bool ServicioHistorial::registrar(RegistroNota registro) {
    if (!estudiantes.existe(registro.carne)) {
        cout << "  No existe un estudiante con el carne " << registro.carne << ".\n";
        return false;
    }

    if (!plan.existe(registro.codigoCurso)) {
        cout << "  No existe el curso " << registro.codigoCurso << ".\n";
        return false;
    }

    if (!periodoValido(registro.periodo)) {
        cout << "  El periodo va con el formato AAAA-Cn.\n";
        return false;
    }

    if (registro.nota < 0 || registro.nota > 100) {
        cout << "  La nota va de 0 a 100.\n";
        return false;
    }

    if (!arbol.insertar(registro)) {
        cout << "  Ya hay una nota de " << registro.codigoCurso << " en " << registro.periodo
             << " para ese estudiante. Use corregir nota.\n";
        return false;
    }

    emitir(mensajeInsertar(COL_NOTAS, documentoDe(registro), "registrar nota " + registro.carne + " " + registro.codigoCurso));

    cout << "  Nota registrada.\n\n";
    arbol.resumenCambios();

    return true;
}

bool ServicioHistorial::corregir(string carne, string periodo, string codigoCurso, int nota) {
    if (nota < 0 || nota > 100) {
        cout << "  La nota va de 0 a 100.\n";
        return false;
    }

    RegistroNota* registro = arbol.buscar(carne, periodo, codigoCurso);

    if (registro == nullptr) {
        cout << "  No hay una nota de " << codigoCurso << " en " << periodo << " para " << carne << ".\n";
        return false;
    }

    int anterior = registro->nota;
    arbol.modificarNota(carne, periodo, codigoCurso, nota);

    Documento datos;
    datos.poner("nota", to_string(nota));
    emitir(mensajeActualizar(COL_NOTAS, filtroNota(carne, periodo, codigoCurso), datos, "corregir nota " + carne + " " + codigoCurso));

    cout << "  Nota corregida: " << anterior << " -> " << nota << "\n";
    return true;
}

bool ServicioHistorial::eliminar(string carne, string periodo, string codigoCurso) {
    if (!arbol.eliminar(carne, periodo, codigoCurso)) {
        cout << "  No hay una nota de " << codigoCurso << " en " << periodo << " para " << carne << ".\n";
        return false;
    }

    emitir(mensajeEliminar(COL_NOTAS, filtroNota(carne, periodo, codigoCurso), "eliminar nota " + carne + " " + codigoCurso));

    cout << "  Nota eliminada.\n\n";
    arbol.resumenCambios();

    return true;
}

// Vistas

void ServicioHistorial::imprimirNotas(vector<RegistroNota>& notas) {
    cout << "  Periodo   Curso     Nota  Estado      Nombre del curso\n";
    cout << "  ----------------------------------------------------------\n";

    for (int i = 0; i < (int)notas.size(); i++) {
        cout << "  " << notas[i].periodo << "   ";
        columna(notas[i].codigoCurso, 10);
        cout << setw(4) << notas[i].nota << "  ";
        columna(estaAprobado(notas[i]) ? "Aprobado" : "Reprobado", 12);
        cout << plan.nombreDe(notas[i].codigoCurso) << "\n";
    }
}

void ServicioHistorial::mostrarHistorial(string carne) {
    if (!estudiantes.existe(carne)) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    vector<RegistroNota> notas;
    int hojas = arbol.historial(carne, notas);

    cout << "  " << estudiantes.nombreDe(carne) << " (" << carne << ")\n\n";

    if (notas.size() == 0) {
        cout << "  No tiene notas registradas.\n";
        return;
    }

    imprimirNotas(notas);

    double resultado;
    promedio(carne, resultado);

    cout << "\n  Promedio: " << fixed << setprecision(1) << resultado << "\n";
    cout.unsetf(ios::fixed);
    cout << "  Hojas del arbol B+ leidas: " << hojas << " de " << arbol.contarHojas() << "\n";
}

void ServicioHistorial::mostrarRango(string carne, string desde, string hasta) {
    if (desde > hasta) {
        cout << "  El periodo inicial no puede ser mayor que el final.\n";
        return;
    }

    vector<RegistroNota> notas;
    int hojas = arbol.rangoPeriodos(carne, desde, hasta, notas);

    cout << "  Notas de " << carne << " del " << desde << " al " << hasta << "\n\n";

    if (notas.size() == 0) {
        cout << "  No hay notas en ese rango.\n";
        return;
    }

    imprimirNotas(notas);
    cout << "\n  Hojas del arbol B+ leidas: " << hojas << "\n";
}

void ServicioHistorial::mostrarNota(string carne, string periodo, string codigoCurso) {
    arbol.mostrarRuta(carne, periodo, codigoCurso);
    cout << "\n";

    RegistroNota* registro = arbol.buscar(carne, periodo, codigoCurso);

    if (registro == nullptr) {
        cout << "  No hay una nota de " << codigoCurso << " en " << periodo << " para " << carne << ".\n";
        return;
    }

    cout << "  Nota encontrada: " << registro->nota << (estaAprobado(*registro) ? " (aprobado)" : " (reprobado)") << "\n";
}

void ServicioHistorial::mostrarArbol() {
    arbol.mostrarNiveles();
    cout << "  Estructura: " << (arbol.estructuraCorrecta() ? "correcta" : "con errores") << "\n";
}

void ServicioHistorial::mostrarHojas() {
    arbol.mostrarHojas();
}

// Consultas

// RN-02: cursos aprobados
void ServicioHistorial::aprobados(string carne, vector<string>& salida) {
    vector<RegistroNota> notas;
    arbol.historial(carne, notas);

    for (int i = 0; i < (int)notas.size(); i++) {
        if (estaAprobado(notas[i])) {
            salida.push_back(notas[i].codigoCurso);
        }
    }
}

bool ServicioHistorial::aprobo(string carne, string codigoCurso) {
    vector<string> lista;
    aprobados(carne, lista);

    for (int i = 0; i < (int)lista.size(); i++) {
        if (lista[i] == codigoCurso) {
            return true;
        }
    }

    return false;
}

// RN-10: promedio
bool ServicioHistorial::promedio(string carne, double& resultado) {
    vector<RegistroNota> notas;
    arbol.historial(carne, notas);

    if (notas.size() == 0) {
        resultado = 0;
        return false;
    }

    int suma = 0;

    for (int i = 0; i < (int)notas.size(); i++) {
        suma += notas[i].nota;
    }

    resultado = (double)suma / notas.size();
    return true;
}

bool ServicioHistorial::tieneNotas(string carne) {
    vector<RegistroNota> notas;
    arbol.historial(carne, notas);

    return notas.size() > 0;
}

bool ServicioHistorial::hayNotasDelCurso(string codigoCurso) {
    vector<RegistroNota> notas;
    arbol.todos(notas);

    for (int i = 0; i < (int)notas.size(); i++) {
        if (notas[i].codigoCurso == codigoCurso) {
            return true;
        }
    }

    return false;
}

void ServicioHistorial::todas(vector<RegistroNota>& salida) {
    arbol.todos(salida);
}

int ServicioHistorial::cantidad() {
    return arbol.cantidad();
}

void ServicioHistorial::vaciar() {
    arbol.vaciar();
}
