// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas del arbol B+ del historial academico

#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
#include "Pruebas.h"
#include "historial/ArbolBPlusHistorial.h"
#include "comun/CargadorArchivos.h"

using namespace std;

// Apoyo

RegistroNota notaDe(string carne, string periodo, string curso, int nota) {
    RegistroNota registro;

    registro.carne = carne;
    registro.periodo = periodo;
    registro.codigoCurso = curso;
    registro.nota = nota;

    return registro;
}

// Notas de los estudiantes 0101 a 0105
void cargarNotasDelDiseno(ArbolBPlusHistorial& arbol, vector<RegistroNota>& notas) {
    vector<RegistroNota> todas;
    int errores = 0;

    leerNotas("datos/notas.txt", todas, errores);

    for (int i = 0; i < (int)todas.size(); i++) {
        if (todas[i].carne <= "2026-0105") {
            notas.push_back(todas[i]);
            arbol.insertar(todas[i]);
        }
    }
}

// Casos del diseno

void probarBPlusBasico() {
    cout << "\nArbol B+: insercion, busqueda y division\n";

    ArbolBPlusHistorial arbol;
    vector<RegistroNota> notas;

    revisar(arbol.esVacio() && arbol.estructuraCorrecta(), "arbol nuevo vacio");

    cargarNotasDelDiseno(arbol, notas);

    // Forma del arbol con las notas del archivo
    revisar(arbol.cantidad() == 22 && notas.size() == 22, "22 registros de los estudiantes 0101 a 0105");
    revisar(arbol.altura() == 3 && arbol.contarHojas() == 11, "3 niveles y 11 hojas a la mitad (insercion en orden)");
    revisar(arbol.estructuraCorrecta(), "estructura correcta");

    cout << "\n";
    arbol.mostrarNiveles();
    cout << "\n";

    revisar(!arbol.insertar(notaDe("2026-0101", "2026-C1", "INF-101", 50)), "rechaza una clave repetida");

    RegistroNota* nota = arbol.buscar("2026-0104", "2026-C3", "INF-201");
    revisar(nota != nullptr && nota->nota == 83, "encuentra la nota de 0104 en INF-201 (83)");
    revisar(arbol.buscar("2026-0104", "2026-C3", "INF-202") == nullptr, "no encuentra un curso que 0104 no llevo");

    revisar(arbol.modificarNota("2026-0104", "2026-C3", "INF-201", 86), "corrige la nota de 0104 en INF-201");
    revisar(arbol.buscar("2026-0104", "2026-C3", "INF-201")->nota == 86, "la nota nueva quedo guardada");
    revisar(!arbol.modificarNota("2026-0199", "2026-C1", "INF-101", 90), "no modifica un registro que no existe");

    vector<RegistroNota> historial;
    int hojas = arbol.historial("2026-0102", historial);

    // Historial de Daniel
    bool ordenado = historial.size() == 5
        && historial[0].codigoCurso == "INF-101" && historial[1].codigoCurso == "MAT-101" && historial[1].nota == 60
        && historial[2].periodo == "2026-C2" && historial[3].codigoCurso == "MAT-101" && historial[3].nota == 74
        && historial[4].codigoCurso == "INF-202";

    revisar(ordenado, "historial de 0102: 5 notas en orden de periodo y curso");
    // Historial de un estudiante
    revisar(hojas == 4, "el historial se lee en 4 hojas enlazadas");

    vector<RegistroNota> rango;
    arbol.rangoPeriodos("2026-0101", "2026-C2", "2026-C3", rango);
    revisar(rango.size() == 3 && rango[0].codigoCurso == "INF-102" && rango[2].codigoCurso == "INF-202", "notas de 0101 entre C2 y C3: 3");

    rango.clear();
    arbol.rangoPeriodos("2026-0103", "2026-C1", "2026-C1", rango);
    revisar(rango.size() == 0, "0103 no tiene notas en C1");

    historial.clear();
    arbol.historial("2026-0199", historial);
    revisar(historial.size() == 0, "un carne sin notas tiene historial vacio");
}

void probarBPlusEliminacion() {
    cout << "\nArbol B+: eliminacion con prestamos y uniones\n";

    ArbolBPlusHistorial arbol;
    vector<RegistroNota> notas;

    cargarNotasDelDiseno(arbol, notas);

    revisar(!arbol.eliminar("2026-0101", "2026-C1", "INF-999"), "no elimina un registro que no existe");

    // Borrar en orden
    bool correcta = true;
    bool encontrados = true;
    int nivelesBajados = 0;
    int alturaAnterior = arbol.altura();

    for (int i = 0; i < (int)notas.size(); i++) {
        RegistroNota registro = notas[i];

        if (!arbol.eliminar(registro.carne, registro.periodo, registro.codigoCurso)) {
            encontrados = false;
        }

        if (arbol.buscar(registro.carne, registro.periodo, registro.codigoCurso) != nullptr) {
            encontrados = false;
        }

        if (!arbol.estructuraCorrecta()) {
            correcta = false;
        }

        if (arbol.altura() < alturaAnterior) {
            nivelesBajados++;
        }

        alturaAnterior = arbol.altura();
    }

    revisar(encontrados, "elimina los 22 registros y ya no los encuentra");
    revisar(correcta, "la estructura queda correcta despues de cada borrado");
    revisar(nivelesBajados == 3, "el arbol baja de 3 niveles a vacio");
    revisar(arbol.esVacio() && arbol.cantidad() == 0, "queda vacio");

    // Borrado de atras hacia adelante
    cargarNotasDelDiseno(arbol, notas);
    correcta = true;

    for (int i = (int)notas.size() - 1; i >= 0; i--) {
        arbol.eliminar(notas[i].carne, notas[i].periodo, notas[i].codigoCurso);

        if (!arbol.estructuraCorrecta()) {
            correcta = false;
        }
    }

    revisar(correcta && arbol.esVacio(), "borrando en orden inverso tambien queda correcto");

    // Borrados puntuales
    notas.clear();
    cargarNotasDelDiseno(arbol, notas);

    arbol.eliminar("2026-0103", "2026-C2", "INF-101");
    cout << "  Tras borrar 0103 C2 INF-101:\n";
    arbol.resumenCambios();

    arbol.eliminar("2026-0103", "2026-C2", "MAT-101");
    cout << "  Tras borrar 0103 C2 MAT-101:\n";
    arbol.resumenCambios();

    revisar(arbol.estructuraCorrecta() && arbol.cantidad() == 20, "quedan 20 registros y la estructura es correcta");
}

// Operaciones al azar comparadas con std::map

void probarBPlusAlAzar() {
    cout << "\nArbol B+: 80 000 operaciones al azar contra std::map\n";

    ArbolBPlusHistorial arbol;
    map<string, int> esperado;
    srand(1781450);

    const string PERIODOS[6] = { "2025-C1", "2025-C2", "2025-C3", "2026-C1", "2026-C2", "2026-C3" };
    const string CURSOS[9] = { "INF-101", "MAT-101", "INF-102", "INF-201", "INF-202", "INF-301", "INF-302", "INF-303", "INF-401" };

    bool todoBien = true;
    bool estructuraBien = true;

    for (int i = 0; i < 80000; i++) {
        string carne = "2026-" + to_string(1000 + rand() % 300);
        string periodo = PERIODOS[rand() % 6];
        string curso = CURSOS[rand() % 9];
        string clave = armarClaveNota(carne, periodo, curso);
        int nota = rand() % 101;
        int operacion = rand() % 5;

        if (operacion <= 1) {
            bool insertado = arbol.insertar(notaDe(carne, periodo, curso, nota));
            bool deberia = esperado.count(clave) == 0;

            if (insertado != deberia) {
                todoBien = false;
            }

            if (deberia) {
                esperado[clave] = nota;
            }
        } else if (operacion <= 3) {
            bool eliminado = arbol.eliminar(carne, periodo, curso);

            if (eliminado != (esperado.erase(clave) == 1)) {
                todoBien = false;
            }
        } else {
            bool modificado = arbol.modificarNota(carne, periodo, curso, nota);

            if (modificado != (esperado.count(clave) == 1)) {
                todoBien = false;
            }

            if (modificado) {
                esperado[clave] = nota;
            }
        }

        if (i % 100 == 0 && !arbol.estructuraCorrecta()) {
            estructuraBien = false;
        }
    }

    revisar(todoBien, "insertar, eliminar y modificar responden igual que std::map");
    revisar(estructuraBien && arbol.estructuraCorrecta(), "la estructura se mantiene correcta durante todas las operaciones");

    vector<RegistroNota> lista;
    arbol.todos(lista);

    bool iguales = lista.size() == esperado.size();
    int posicion = 0;

    for (auto par : esperado) {
        if (!iguales) {
            break;
        }

        if (claveDe(lista[posicion]) != par.first || lista[posicion].nota != par.second) {
            iguales = false;
        }

        posicion++;
    }

    revisar(iguales, "las hojas enlazadas tienen los mismos datos en orden");

    cout << "  Al final: " << arbol.cantidad() << " registros, " << arbol.altura() << " niveles, "
         << arbol.contarHojas() << " hojas\n";
}

void probarBPlusGrande() {
    cout << "\nArbol B+: 100 000 registros\n";

    ArbolBPlusHistorial arbol;
    const string CURSOS[10] = { "INF-101", "MAT-101", "INF-102", "INF-201", "INF-202", "INF-301", "INF-302", "INF-303", "INF-401", "MAT-201" };
    bool todos = true;

    // Muchas notas
    for (int estudiante = 0; estudiante < 5000; estudiante++) {
        string carne = "2026-" + to_string(10000 + estudiante);

        for (int c = 0; c < 10; c++) {
            if (!arbol.insertar(notaDe(carne, "2026-C1", CURSOS[c], 70 + c))) {
                todos = false;
            }

            if (!arbol.insertar(notaDe(carne, "2026-C2", CURSOS[c], 80 + c))) {
                todos = false;
            }
        }
    }

    revisar(todos && arbol.cantidad() == 100000, "inserta los 100 000");
    revisar(arbol.estructuraCorrecta(), "estructura correcta");

    // Altura maxima del arbol
    revisar(arbol.altura() <= 17, "la altura no pasa de 17");

    vector<RegistroNota> historial;
    int hojas = arbol.historial("2026-12500", historial);

    revisar(historial.size() == 20, "el historial de un estudiante trae sus 20 notas");
    revisar(hojas <= 12, "y se lee en pocas hojas, sin recorrer las demas");

    cout << "  " << arbol.altura() << " niveles, " << arbol.contarHojas() << " hojas; historial de 20 notas en "
         << hojas << " hojas\n";

    for (int estudiante = 0; estudiante < 5000; estudiante++) {
        string carne = "2026-" + to_string(10000 + estudiante);

        for (int c = 0; c < 10; c++) {
            arbol.eliminar(carne, "2026-C1", CURSOS[c]);
            arbol.eliminar(carne, "2026-C2", CURSOS[c]);
        }
    }

    revisar(arbol.esVacio() && arbol.estructuraCorrecta(), "al borrar todo queda vacio");
}
