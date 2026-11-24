// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de la lectura desde la nube

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "nube/FuenteNube.h"
#include "comun/Colecciones.h"

using namespace std;

FuenteNube::FuenteNube(ClienteNube& cliente) : cliente(cliente) {
    vacia = false;
}

string FuenteNube::nombre() {
    return "la base de datos en la nube";
}

bool FuenteNube::estabaVacia() {
    return vacia;
}

bool FuenteNube::consultar(string coleccion, vector<Documento>& documentos) {
    if (!cliente.consultar(coleccion, documentos)) {
        cout << "  No se pudo leer " << coleccion << ": " << cliente.ultimoError() << "\n";
        return false;
    }

    return true;
}

// Convierte cada documento con su funcion; cuenta los que no sirven
template <typename Entidad>
void convertir(vector<Documento>& documentos, vector<Entidad>& salida, int& malos) {
    for (int i = 0; i < (int)documentos.size(); i++) {
        Entidad entidad;

        if (leerDocumento(documentos[i], entidad)) {
            salida.push_back(entidad);
        } else {
            malos++;
        }
    }
}

bool FuenteNube::leer(DatosIniciales& datos) {
    vacia = false;

    vector<Documento> estudiantes;
    vector<Documento> cursos;
    vector<Documento> requisitos;
    vector<Documento> notas;
    vector<Documento> citas;
    vector<Documento> grupos;
    vector<Documento> matriculas;
    vector<Documento> espera;

    // Una consulta por coleccion; si una falla no se usa nada
    if (!consultar(COL_ESTUDIANTES, estudiantes) || !consultar(COL_CURSOS, cursos)
        || !consultar(COL_REQUISITOS, requisitos) || !consultar(COL_NOTAS, notas)
        || !consultar(COL_CITAS, citas) || !consultar(COL_GRUPOS, grupos)
        || !consultar(COL_MATRICULAS, matriculas) || !consultar(COL_ESPERA, espera)) {
        return false;
    }

    if (estudiantes.size() == 0 || cursos.size() == 0) {
        cout << "  La nube todavia no tiene datos del sistema.\n";
        vacia = true;
        return false;
    }

    // La cola se reconstruye en orden de llegada
    stable_sort(espera.begin(), espera.end(), [](const Documento& a, const Documento& b) {
        return atoll(a.valor("llegada").c_str()) < atoll(b.valor("llegada").c_str());
    });

    int malos = 0;

    convertir(estudiantes, datos.estudiantes, malos);
    convertir(cursos, datos.cursos, malos);
    convertir(requisitos, datos.requisitos, malos);
    convertir(notas, datos.notas, malos);
    convertir(citas, datos.citas, malos);
    convertir(grupos, datos.grupos, malos);
    convertir(matriculas, datos.matriculas, malos);
    convertir(espera, datos.espera, malos);

    if (malos > 0) {
        cout << "  Documentos con datos incompletos: " << malos << "\n";
    }

    return true;
}
