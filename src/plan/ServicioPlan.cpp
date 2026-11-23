// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del componente del plan de estudios

#include <iostream>
#include "plan/ServicioPlan.h"

using namespace std;

bool ServicioPlan::cargarCurso(Curso curso) {
    return grafo.agregarCurso(curso);
}

ResultadoRequisito ServicioPlan::cargarRequisito(string requisito, string curso) {
    return grafo.agregarRequisito(requisito, curso);
}

// Cursos

bool ServicioPlan::agregarCurso(Curso curso) {
    if (curso.codigo == "" || curso.nombre == "") {
        cout << "  Falta el codigo o el nombre.\n";
        return false;
    }

    if (curso.creditos <= 0 || curso.creditos > 12) {
        cout << "  Los creditos van de 1 a 12.\n";
        return false;
    }

    if (!grafo.agregarCurso(curso)) {
        cout << "  Ya existe el curso " << curso.codigo << ".\n";
        return false;
    }

    cout << "  Curso agregado: " << curso.codigo << "  " << curso.nombre << "\n";
    return true;
}

bool ServicioPlan::modificarCurso(string codigo, string nombre, int creditos) {
    if (nombre == "" || creditos <= 0 || creditos > 12) {
        cout << "  Falta el nombre o los creditos no van de 1 a 12.\n";
        return false;
    }

    if (!grafo.modificarCurso(codigo, nombre, creditos)) {
        cout << "  No existe el curso " << codigo << ".\n";
        return false;
    }

    cout << "  Curso actualizado.\n";
    return true;
}

bool ServicioPlan::eliminarCurso(string codigo) {
    vector<string> antes;
    grafo.requisitosDe(codigo, antes);

    vector<string> habilitaba;
    grafo.habilitaA(codigo, habilitaba);

    if (!grafo.eliminarCurso(codigo)) {
        cout << "  No existe el curso " << codigo << ".\n";
        return false;
    }

    cout << "  Curso eliminado junto con " << antes.size() + habilitaba.size() << " requisitos.\n";
    return true;
}

// Requisitos (RN-08: no puede haber ciclos)

bool ServicioPlan::agregarRequisito(string requisito, string curso) {
    ResultadoRequisito resultado = grafo.agregarRequisito(requisito, curso);

    switch (resultado) {
        case REQUISITO_AGREGADO:
            cout << "  " << requisito << " ahora es requisito de " << curso << ".\n";
            return true;

        case CURSO_NO_EXISTE:
            cout << "  Uno de los dos cursos no existe.\n";
            break;

        case REQUISITO_REPETIDO:
            cout << "  " << requisito << " ya es requisito de " << curso << ".\n";
            break;

        case MISMO_CURSO:
            cout << "  Un curso no puede ser requisito de si mismo.\n";
            break;

        case FORMA_CICLO:
            cout << "  No se puede: " << curso << " ya es requisito (directo o indirecto) de "
                 << requisito << " y se formaria un ciclo.\n";
            break;
    }

    return false;
}

bool ServicioPlan::quitarRequisito(string requisito, string curso) {
    if (!grafo.quitarRequisito(requisito, curso)) {
        cout << "  " << requisito << " no es requisito de " << curso << ".\n";
        return false;
    }

    cout << "  Requisito quitado.\n";
    return true;
}

// Vistas

void ServicioPlan::mostrarPlan() {
    grafo.mostrarOrden();
}

void ServicioPlan::mostrarListas() {
    grafo.mostrarListas();
}

void ServicioPlan::mostrarCurso(string codigo) {
    Curso* curso = grafo.buscarCurso(codigo);

    if (curso == nullptr) {
        cout << "  No existe el curso " << codigo << ".\n";
        return;
    }

    vector<string> requisitos;
    vector<string> habilita;

    grafo.requisitosDe(codigo, requisitos);
    grafo.habilitaA(codigo, habilita);

    cout << "  " << curso->codigo << "  " << curso->nombre << " (" << curso->creditos << " creditos)\n";
    cout << "  Requisitos: ";

    if (requisitos.size() == 0) {
        cout << "ninguno";
    }

    for (int i = 0; i < (int)requisitos.size(); i++) {
        cout << requisitos[i] << (i < (int)requisitos.size() - 1 ? ", " : "");
    }

    cout << "\n  Es requisito de: ";

    if (habilita.size() == 0) {
        cout << "ninguno";
    }

    for (int i = 0; i < (int)habilita.size(); i++) {
        cout << habilita[i] << (i < (int)habilita.size() - 1 ? ", " : "");
    }

    cout << "\n";
}

// Consultas

bool ServicioPlan::existe(string codigo) {
    return grafo.buscarCurso(codigo) != nullptr;
}

Curso* ServicioPlan::buscar(string codigo) {
    return grafo.buscarCurso(codigo);
}

string ServicioPlan::nombreDe(string codigo) {
    Curso* curso = grafo.buscarCurso(codigo);

    if (curso == nullptr) {
        return "";
    }

    return curso->nombre;
}

// Requisitos del curso que no estan en la lista de aprobados (RN-02)
void ServicioPlan::requisitosFaltantes(string codigo, vector<string>& aprobados, vector<string>& faltan) {
    vector<string> requisitos;
    grafo.requisitosDe(codigo, requisitos);

    for (int i = 0; i < (int)requisitos.size(); i++) {
        bool aprobado = false;

        for (int j = 0; j < (int)aprobados.size(); j++) {
            if (aprobados[j] == requisitos[i]) {
                aprobado = true;
            }
        }

        if (!aprobado) {
            faltan.push_back(requisitos[i]);
        }
    }
}

void ServicioPlan::habilitados(vector<string>& aprobados, vector<string>& salida) {
    grafo.cursosHabilitados(aprobados, salida);
}

void ServicioPlan::cursos(vector<Curso>& salida) {
    grafo.cursos(salida);
}

// Todas las aristas: origen[i] es requisito de destino[i]
void ServicioPlan::requisitos(vector<string>& origen, vector<string>& destino) {
    vector<Curso> lista;
    grafo.cursos(lista);

    for (int i = 0; i < (int)lista.size(); i++) {
        vector<string> habilita;
        grafo.habilitaA(lista[i].codigo, habilita);

        for (int j = 0; j < (int)habilita.size(); j++) {
            origen.push_back(lista[i].codigo);
            destino.push_back(habilita[j]);
        }
    }
}

int ServicioPlan::cantidadCursos() {
    return grafo.cantidadCursos();
}

int ServicioPlan::cantidadRequisitos() {
    return grafo.cantidadRequisitos();
}

void ServicioPlan::vaciar() {
    grafo.vaciar();
}
