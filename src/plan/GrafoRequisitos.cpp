// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del grafo de requisitos

#include <iostream>
#include "plan/GrafoRequisitos.h"

using namespace std;

// Constructor y destructor

GrafoRequisitos::GrafoRequisitos() {
    primero = nullptr;
    ultimo = nullptr;
    totalCursos = 0;
    totalRequisitos = 0;
}

GrafoRequisitos::~GrafoRequisitos() {
    vaciar();
}

void GrafoRequisitos::vaciar() {
    Vertice* actual = primero;

    while (actual != nullptr) {
        Vertice* siguiente = actual->siguiente;

        liberarLista(actual->habilita);
        liberarLista(actual->requisitos);
        delete actual;

        actual = siguiente;
    }

    primero = nullptr;
    ultimo = nullptr;
    totalCursos = 0;
    totalRequisitos = 0;
}

// Listas de adyacencia

void GrafoRequisitos::agregarAlFinal(NodoAdyacencia*& lista, Vertice* vertice) {
    NodoAdyacencia* nuevo = new NodoAdyacencia;

    nuevo->vertice = vertice;
    nuevo->siguiente = nullptr;

    if (lista == nullptr) {
        lista = nuevo;
        return;
    }

    NodoAdyacencia* actual = lista;

    while (actual->siguiente != nullptr) {
        actual = actual->siguiente;
    }

    actual->siguiente = nuevo;
}

bool GrafoRequisitos::quitarDeLista(NodoAdyacencia*& lista, Vertice* vertice) {
    NodoAdyacencia* anterior = nullptr;
    NodoAdyacencia* actual = lista;

    while (actual != nullptr && actual->vertice != vertice) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == nullptr) {
        return false;
    }

    if (anterior == nullptr) {
        lista = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    delete actual;
    return true;
}

bool GrafoRequisitos::estaEnLista(NodoAdyacencia* lista, Vertice* vertice) {
    while (lista != nullptr) {
        if (lista->vertice == vertice) {
            return true;
        }

        lista = lista->siguiente;
    }

    return false;
}

void GrafoRequisitos::liberarLista(NodoAdyacencia*& lista) {
    while (lista != nullptr) {
        NodoAdyacencia* siguiente = lista->siguiente;
        delete lista;
        lista = siguiente;
    }
}

// Cursos

Vertice* GrafoRequisitos::buscarVertice(string codigo) {
    Vertice* actual = primero;

    while (actual != nullptr) {
        if (actual->curso.codigo == codigo) {
            return actual;
        }

        actual = actual->siguiente;
    }

    return nullptr;
}

bool GrafoRequisitos::agregarCurso(Curso curso) {
    if (curso.codigo == "" || buscarVertice(curso.codigo) != nullptr) {
        return false;
    }

    Vertice* nuevo = new Vertice;

    nuevo->curso = curso;
    nuevo->habilita = nullptr;
    nuevo->requisitos = nullptr;
    nuevo->siguiente = nullptr;
    nuevo->visitado = false;
    nuevo->pendientes = 0;

    // Agregar al final
    if (primero == nullptr) {
        primero = nuevo;
    } else {
        ultimo->siguiente = nuevo;
    }

    ultimo = nuevo;
    totalCursos++;

    return true;
}

Curso* GrafoRequisitos::buscarCurso(string codigo) {
    Vertice* vertice = buscarVertice(codigo);

    if (vertice == nullptr) {
        return nullptr;
    }

    return &vertice->curso;
}

// Modificar el curso
bool GrafoRequisitos::modificarCurso(string codigo, string nombre, int creditos) {
    Curso* curso = buscarCurso(codigo);

    if (curso == nullptr) {
        return false;
    }

    curso->nombre = nombre;
    curso->creditos = creditos;

    return true;
}

// Quitar las aristas del curso
bool GrafoRequisitos::eliminarCurso(string codigo) {
    Vertice* anterior = nullptr;
    Vertice* vertice = primero;

    while (vertice != nullptr && vertice->curso.codigo != codigo) {
        anterior = vertice;
        vertice = vertice->siguiente;
    }

    if (vertice == nullptr) {
        return false;
    }

    // Aristas que salen
    NodoAdyacencia* arista = vertice->habilita;

    while (arista != nullptr) {
        quitarDeLista(arista->vertice->requisitos, vertice);
        totalRequisitos--;
        arista = arista->siguiente;
    }

    // Sus requisitos ya no lo habilitan
    arista = vertice->requisitos;

    while (arista != nullptr) {
        quitarDeLista(arista->vertice->habilita, vertice);
        totalRequisitos--;
        arista = arista->siguiente;
    }

    liberarLista(vertice->habilita);
    liberarLista(vertice->requisitos);

    if (anterior == nullptr) {
        primero = vertice->siguiente;
    } else {
        anterior->siguiente = vertice->siguiente;
    }

    if (ultimo == vertice) {
        ultimo = anterior;
    }

    delete vertice;
    totalCursos--;

    return true;
}

// Requisitos

void GrafoRequisitos::desmarcar() {
    Vertice* actual = primero;

    while (actual != nullptr) {
        actual->visitado = false;
        actual = actual->siguiente;
    }
}

// Recorrido en profundidad
bool GrafoRequisitos::buscarCamino(Vertice* actual, Vertice* destino) {
    // Caso trivial: llegamos al destino
    if (actual == destino) {
        return true;
    }

    actual->visitado = true;
    NodoAdyacencia* arista = actual->habilita;

    while (arista != nullptr) {
        if (!arista->vertice->visitado && buscarCamino(arista->vertice, destino)) {
            return true;
        }

        arista = arista->siguiente;
    }

    return false;
}

bool GrafoRequisitos::hayCamino(string desde, string hasta) {
    Vertice* origen = buscarVertice(desde);
    Vertice* destino = buscarVertice(hasta);

    if (origen == nullptr || destino == nullptr) {
        return false;
    }

    desmarcar();
    return buscarCamino(origen, destino);
}

// Revisar si se forma un ciclo
ResultadoRequisito GrafoRequisitos::agregarRequisito(string requisito, string curso) {
    Vertice* origen = buscarVertice(requisito);
    Vertice* destino = buscarVertice(curso);

    if (origen == nullptr || destino == nullptr) {
        return CURSO_NO_EXISTE;
    }

    if (origen == destino) {
        return MISMO_CURSO;
    }

    if (estaEnLista(origen->habilita, destino)) {
        return REQUISITO_REPETIDO;
    }

    desmarcar();

    if (buscarCamino(destino, origen)) {
        return FORMA_CICLO;
    }

    agregarAlFinal(origen->habilita, destino);
    agregarAlFinal(destino->requisitos, origen);
    totalRequisitos++;

    return REQUISITO_AGREGADO;
}

bool GrafoRequisitos::quitarRequisito(string requisito, string curso) {
    Vertice* origen = buscarVertice(requisito);
    Vertice* destino = buscarVertice(curso);

    if (origen == nullptr || destino == nullptr) {
        return false;
    }

    if (!quitarDeLista(origen->habilita, destino)) {
        return false;
    }

    quitarDeLista(destino->requisitos, origen);
    totalRequisitos--;

    return true;
}

bool GrafoRequisitos::esRequisito(string requisito, string curso) {
    Vertice* origen = buscarVertice(requisito);
    Vertice* destino = buscarVertice(curso);

    return origen != nullptr && destino != nullptr && estaEnLista(origen->habilita, destino);
}

// Consultas

void GrafoRequisitos::requisitosDe(string codigo, vector<string>& salida) {
    Vertice* vertice = buscarVertice(codigo);

    if (vertice == nullptr) {
        return;
    }

    for (NodoAdyacencia* arista = vertice->requisitos; arista != nullptr; arista = arista->siguiente) {
        salida.push_back(arista->vertice->curso.codigo);
    }
}

void GrafoRequisitos::habilitaA(string codigo, vector<string>& salida) {
    Vertice* vertice = buscarVertice(codigo);

    if (vertice == nullptr) {
        return;
    }

    for (NodoAdyacencia* arista = vertice->habilita; arista != nullptr; arista = arista->siguiente) {
        salida.push_back(arista->vertice->curso.codigo);
    }
}

void GrafoRequisitos::cursos(vector<Curso>& salida) {
    for (Vertice* actual = primero; actual != nullptr; actual = actual->siguiente) {
        salida.push_back(actual->curso);
    }
}

// Algoritmo de Kahn
bool GrafoRequisitos::ordenTopologico(vector<string>& salida) {
    vector<Vertice*> listos;

    for (Vertice* actual = primero; actual != nullptr; actual = actual->siguiente) {
        actual->pendientes = 0;

        for (NodoAdyacencia* arista = actual->requisitos; arista != nullptr; arista = arista->siguiente) {
            actual->pendientes++;
        }

        if (actual->pendientes == 0) {
            listos.push_back(actual);
        }
    }

    // Cola de cursos listos
    int frente = 0;

    while (frente < (int)listos.size()) {
        Vertice* actual = listos[frente];
        frente++;

        salida.push_back(actual->curso.codigo);

        for (NodoAdyacencia* arista = actual->habilita; arista != nullptr; arista = arista->siguiente) {
            arista->vertice->pendientes--;

            if (arista->vertice->pendientes == 0) {
                listos.push_back(arista->vertice);
            }
        }
    }

    // Ciclo si falto algun curso
    return (int)salida.size() == totalCursos;
}

bool GrafoRequisitos::contiene(vector<string>& lista, string codigo) {
    for (int i = 0; i < (int)lista.size(); i++) {
        if (lista[i] == codigo) {
            return true;
        }
    }

    return false;
}

void GrafoRequisitos::cursosHabilitados(vector<string>& aprobados, vector<string>& salida) {
    for (Vertice* actual = primero; actual != nullptr; actual = actual->siguiente) {
        if (contiene(aprobados, actual->curso.codigo)) {
            continue;
        }

        bool cumple = true;

        for (NodoAdyacencia* arista = actual->requisitos; arista != nullptr && cumple; arista = arista->siguiente) {
            if (!contiene(aprobados, arista->vertice->curso.codigo)) {
                cumple = false;
            }
        }

        if (cumple) {
            salida.push_back(actual->curso.codigo);
        }
    }
}

// Vista interna

void GrafoRequisitos::imprimirLista(NodoAdyacencia* lista) {
    if (lista == nullptr) {
        cout << "-";
        return;
    }

    while (lista != nullptr) {
        cout << lista->vertice->curso.codigo;

        if (lista->siguiente != nullptr) {
            cout << ", ";
        }

        lista = lista->siguiente;
    }
}

void GrafoRequisitos::mostrarListas() {
    if (primero == nullptr) {
        cout << "  El plan de estudios esta vacio.\n";
        return;
    }

    cout << "  Curso      Requisitos           Habilita a\n";
    cout << "  ------------------------------------------------------\n";

    for (Vertice* actual = primero; actual != nullptr; actual = actual->siguiente) {
        string codigo = actual->curso.codigo;

        cout << "  " << codigo;

        for (int i = codigo.length(); i < 11; i++) {
            cout << " ";
        }

        // Columna de requisitos
        vector<string> requisitos;
        requisitosDe(codigo, requisitos);

        string columna = "-";

        if (requisitos.size() > 0) {
            columna = "";

            for (int i = 0; i < (int)requisitos.size(); i++) {
                columna = columna + requisitos[i];

                if (i < (int)requisitos.size() - 1) {
                    columna = columna + ", ";
                }
            }
        }

        cout << columna;

        for (int i = columna.length(); i < 21; i++) {
            cout << " ";
        }

        imprimirLista(actual->habilita);
        cout << "\n";
    }

    cout << "\n  Cursos: " << totalCursos << ", requisitos: " << totalRequisitos << "\n";
}

void GrafoRequisitos::mostrarOrden() {
    vector<string> orden;

    if (!ordenTopologico(orden)) {
        cout << "  El plan tiene un ciclo, no hay orden valido.\n";
        return;
    }

    cout << "  Orden valido de la carrera:\n";

    for (int i = 0; i < (int)orden.size(); i++) {
        Curso* curso = buscarCurso(orden[i]);

        cout << "  " << i + 1 << ". " << orden[i] << "  " << curso->nombre << "\n";
    }
}

int GrafoRequisitos::cantidadCursos() {
    return totalCursos;
}

int GrafoRequisitos::cantidadRequisitos() {
    return totalRequisitos;
}

bool GrafoRequisitos::esVacio() {
    return totalCursos == 0;
}
