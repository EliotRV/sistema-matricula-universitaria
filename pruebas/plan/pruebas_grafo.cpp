// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas del grafo de requisitos

#include <iostream>
#include <cstdlib>
#include <vector>
#include "Pruebas.h"
#include "plan/GrafoRequisitos.h"
#include "comun/CargadorArchivos.h"

using namespace std;

// Apoyo

// Carga los 9 cursos y 10 requisitos de los archivos de datos
void cargarPlan(GrafoRequisitos& grafo) {
    vector<Curso> cursos;
    vector<Requisito> requisitos;
    int errores = 0;

    leerCursos("datos/cursos.txt", cursos, errores);
    leerRequisitos("datos/requisitos.txt", requisitos, errores);

    for (int i = 0; i < (int)cursos.size(); i++) {
        grafo.agregarCurso(cursos[i]);
    }

    for (int i = 0; i < (int)requisitos.size(); i++) {
        grafo.agregarRequisito(requisitos[i].requisito, requisitos[i].curso);
    }
}

Curso cursoDe(string codigo) {
    Curso curso;

    curso.codigo = codigo;
    curso.nombre = "Curso " + codigo;
    curso.creditos = 4;

    return curso;
}

string unir(vector<string>& lista) {
    string texto = "";

    for (int i = 0; i < (int)lista.size(); i++) {
        if (i > 0) {
            texto = texto + ", ";
        }

        texto = texto + lista[i];
    }

    return texto;
}

// Casos del diseno

void probarGrafoBasico() {
    cout << "\nGrafo de requisitos: plan de estudios\n";

    GrafoRequisitos grafo;

    revisar(grafo.esVacio(), "grafo nuevo vacio");

    cargarPlan(grafo);

    revisar(grafo.cantidadCursos() == 9 && grafo.cantidadRequisitos() == 10, "9 cursos y 10 requisitos");
    revisar(!grafo.agregarCurso(cursoDe("INF-101")), "rechaza un curso repetido");

    cout << "\n";
    grafo.mostrarListas();
    cout << "\n";

    vector<string> orden;
    revisar(grafo.ordenTopologico(orden), "hay un orden valido (no hay ciclos)");

    // El mismo orden que muestra el diseno del Entregable 1
    revisar(unir(orden) == "INF-101, MAT-101, INF-102, INF-201, INF-202, INF-301, INF-302, INF-303, INF-401", "orden topologico igual al del diseno");

    vector<string> lista;
    grafo.requisitosDe("INF-201", lista);
    revisar(unir(lista) == "INF-102, MAT-101", "requisitos de INF-201: INF-102 y MAT-101");

    lista.clear();
    grafo.habilitaA("INF-102", lista);
    revisar(unir(lista) == "INF-201, INF-202, INF-302", "INF-102 habilita INF-201, INF-202 e INF-302");

    revisar(grafo.hayCamino("INF-101", "INF-401"), "hay camino de INF-101 a INF-401 (requisito indirecto)");
    revisar(!grafo.hayCamino("INF-401", "INF-101"), "no hay camino de INF-401 a INF-101");

    // Cursos habilitados con el historial del diseno
    vector<string> ana = { "INF-101", "MAT-101", "INF-102", "INF-201", "INF-202" };
    lista.clear();
    grafo.cursosHabilitados(ana, lista);
    revisar(unir(lista) == "INF-301, INF-302, INF-303", "habilitados de Ana: INF-301, INF-302, INF-303");

    // Daniel reprobo MAT-101 en C1 pero la aprobo en C2
    vector<string> daniel = { "INF-101", "INF-102", "MAT-101", "INF-202" };
    lista.clear();
    grafo.cursosHabilitados(daniel, lista);
    revisar(unir(lista) == "INF-201, INF-302, INF-303", "habilitados de Daniel: INF-201, INF-302, INF-303");

    vector<string> nuevo;
    lista.clear();
    grafo.cursosHabilitados(nuevo, lista);
    revisar(unir(lista) == "INF-101, MAT-101", "un estudiante nuevo solo tiene habilitados los cursos sin requisitos");
}

void probarGrafoCambios() {
    cout << "\nGrafo de requisitos: ciclos, cambios y borrados\n";

    GrafoRequisitos grafo;
    cargarPlan(grafo);

    revisar(grafo.agregarRequisito("INF-401", "INF-101") == FORMA_CICLO, "rechaza INF-401 -> INF-101 porque forma un ciclo");
    revisar(grafo.agregarRequisito("INF-301", "INF-201") == FORMA_CICLO, "rechaza INF-301 -> INF-201 (ciclo de dos)");
    revisar(grafo.agregarRequisito("INF-101", "INF-101") == MISMO_CURSO, "rechaza un curso como requisito de si mismo");
    revisar(grafo.agregarRequisito("INF-101", "INF-102") == REQUISITO_REPETIDO, "rechaza un requisito repetido");
    revisar(grafo.agregarRequisito("INF-999", "INF-102") == CURSO_NO_EXISTE, "rechaza un curso que no existe");
    revisar(grafo.cantidadRequisitos() == 10, "los rechazos no cambian el grafo");

    revisar(grafo.agregarCurso(cursoDe("MAT-201")), "agrega MAT-201");
    revisar(grafo.agregarRequisito("MAT-101", "MAT-201") == REQUISITO_AGREGADO, "MAT-101 pasa a ser requisito de MAT-201");
    revisar(grafo.cantidadCursos() == 10 && grafo.cantidadRequisitos() == 11, "10 cursos y 11 requisitos");

    revisar(grafo.modificarCurso("MAT-201", "Calculo", 3), "modifica MAT-201");
    revisar(grafo.buscarCurso("MAT-201")->nombre == "Calculo" && grafo.buscarCurso("MAT-201")->creditos == 3, "el nombre y los creditos nuevos quedaron");
    revisar(!grafo.modificarCurso("MAT-999", "Nada", 1), "no modifica un curso que no existe");

    revisar(grafo.quitarRequisito("INF-202", "INF-303"), "quita INF-202 como requisito de INF-303");
    revisar(!grafo.esRequisito("INF-202", "INF-303") && grafo.cantidadRequisitos() == 10, "ya no es requisito y quedan 10");
    revisar(!grafo.quitarRequisito("INF-202", "INF-303"), "no se puede quitar dos veces");

    // INF-302 tiene 2 requisitos y habilita a INF-401: se van 3 aristas
    revisar(grafo.eliminarCurso("INF-302"), "elimina INF-302");
    revisar(grafo.cantidadCursos() == 9 && grafo.cantidadRequisitos() == 7, "quedan 9 cursos y 7 requisitos");

    vector<string> lista;
    grafo.requisitosDe("INF-401", lista);
    revisar(lista.size() == 1 && lista[0] == "INF-301", "INF-401 ya solo pide INF-301");

    lista.clear();
    grafo.habilitaA("INF-102", lista);
    revisar(unir(lista) == "INF-201, INF-202", "INF-102 ya no habilita INF-302");

    revisar(!grafo.eliminarCurso("INF-302"), "no elimina un curso que ya no existe");

    lista.clear();
    revisar(grafo.ordenTopologico(lista) && lista.size() == 9, "sigue habiendo un orden valido con los 9 cursos");

    // Se borra el primero y el ultimo de la lista de vertices
    revisar(grafo.eliminarCurso("INF-101") && grafo.eliminarCurso("MAT-201"), "elimina el primer y el ultimo curso");
    revisar(grafo.agregarCurso(cursoDe("INF-501")), "despues se puede seguir agregando al final");
    revisar(grafo.cantidadCursos() == 8, "quedan 8 cursos");
}

// Aristas al azar: el grafo nunca debe quedar con un ciclo

void probarGrafoAlAzar() {
    cout << "\nGrafo de requisitos: 5000 requisitos al azar entre 60 cursos\n";

    GrafoRequisitos grafo;
    srand(301);

    for (int i = 0; i < 60; i++) {
        grafo.agregarCurso(cursoDe("C-" + to_string(100 + i)));
    }

    bool rechazosBien = true;
    int agregados = 0;
    int ciclos = 0;

    for (int i = 0; i < 5000; i++) {
        string a = "C-" + to_string(100 + rand() % 60);
        string b = "C-" + to_string(100 + rand() % 60);

        ResultadoRequisito resultado = grafo.agregarRequisito(a, b);

        if (resultado == REQUISITO_AGREGADO) {
            agregados++;
        }

        // Solo se rechaza por ciclo si de verdad ya habia camino de b a a
        if (resultado == FORMA_CICLO) {
            ciclos++;

            if (!grafo.hayCamino(b, a)) {
                rechazosBien = false;
            }
        }

        // De vez en cuando se quita un requisito al azar
        if (i % 7 == 0) {
            string c = "C-" + to_string(100 + rand() % 60);
            string d = "C-" + to_string(100 + rand() % 60);

            if (grafo.quitarRequisito(c, d)) {
                agregados--;
            }
        }
    }

    vector<string> orden;
    bool hayOrden = grafo.ordenTopologico(orden);

    // En el orden, cada requisito debe aparecer antes que su curso
    bool respetaRequisitos = hayOrden;

    for (int i = 0; i < (int)orden.size() && respetaRequisitos; i++) {
        vector<string> requisitos;
        grafo.requisitosDe(orden[i], requisitos);

        for (int j = 0; j < (int)requisitos.size(); j++) {
            bool antes = false;

            for (int k = 0; k < i; k++) {
                if (orden[k] == requisitos[j]) {
                    antes = true;
                }
            }

            if (!antes) {
                respetaRequisitos = false;
            }
        }
    }

    revisar(rechazosBien, "cada rechazo por ciclo corresponde a un camino que ya existia");
    revisar(hayOrden && orden.size() == 60, "el grafo sigue sin ciclos: el orden incluye los 60 cursos");
    revisar(respetaRequisitos, "en el orden cada requisito va antes que su curso");
    revisar(grafo.cantidadRequisitos() == agregados, "la cuenta de requisitos coincide");

    cout << "  Requisitos en el grafo: " << grafo.cantidadRequisitos() << ", rechazados por ciclo: " << ciclos << "\n";
}
