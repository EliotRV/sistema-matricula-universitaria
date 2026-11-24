// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de la tabla hash extensible de estudiantes

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <set>
#include "estudiantes/TablaHashEstudiantes.h"

using namespace std;

// Constructor y destructor

// Inicia con una entrada y una cubeta
TablaHashEstudiantes::TablaHashEstudiantes() {
    profundidad = 0;
    total = 0;
    directorio = new Cubeta*[1];
    directorio[0] = crearCubeta(0);
    reiniciarCambios();
}

TablaHashEstudiantes::~TablaHashEstudiantes() {
    liberar();
}

// Liberar cada cubeta una sola vez
void TablaHashEstudiantes::liberar() {
    int tamano = tamanoDirectorio();

    for (int i = tamano - 1; i >= 0; i--) {
        if (i < (1 << directorio[i]->profundidadLocal)) {
            delete directorio[i];
        }
    }

    delete[] directorio;
    directorio = nullptr;
}

void TablaHashEstudiantes::vaciar() {
    liberar();

    profundidad = 0;
    total = 0;
    directorio = new Cubeta*[1];
    directorio[0] = crearCubeta(0);
}

Cubeta* TablaHashEstudiantes::crearCubeta(int profundidadLocal) {
    Cubeta* nueva = new Cubeta;

    nueva->profundidadLocal = profundidadLocal;
    nueva->cantidad = 0;

    return nueva;
}

void TablaHashEstudiantes::reiniciarCambios() {
    divisiones = 0;
    duplicaciones = 0;
    uniones = 0;
    reducciones = 0;
}

// Funcion hash

unsigned long long TablaHashEstudiantes::valorHash(string carne) {
    unsigned long long valor = 0;

    for (int i = 0; i < (int)carne.length(); i++) {
        if (carne[i] >= '0' && carne[i] <= '9') {
            valor = valor * 10 + (carne[i] - '0');
        }
    }

    return valor;
}

// Indice en el directorio
int TablaHashEstudiantes::indiceDe(string carne) {
    unsigned long long mascara = (1ULL << profundidad) - 1;
    return (int)(valorHash(carne) & mascara);
}

int TablaHashEstudiantes::posicionEnCubeta(Cubeta* cubeta, string carne) {
    for (int i = 0; i < cubeta->cantidad; i++) {
        if (cubeta->registros[i].carne == carne) {
            return i;
        }
    }

    return -1;
}

// Busqueda

Estudiante* TablaHashEstudiantes::buscar(string carne) {
    Cubeta* cubeta = directorio[indiceDe(carne)];
    int posicion = posicionEnCubeta(cubeta, carne);

    if (posicion == -1) {
        return nullptr;
    }

    return &cubeta->registros[posicion];
}

// Insercion

bool TablaHashEstudiantes::insertar(Estudiante estudiante) {
    reiniciarCambios();

    if (buscar(estudiante.carne) != nullptr) {
        return false;
    }

    // Dividir la cubeta llena y reintentar
    while (true) {
        Cubeta* cubeta = directorio[indiceDe(estudiante.carne)];

        if (cubeta->cantidad < CAPACIDAD_CUBETA) {
            cubeta->registros[cubeta->cantidad] = estudiante;
            cubeta->cantidad++;
            total++;
            return true;
        }

        if (!dividirCubeta(indiceDe(estudiante.carne))) {
            return false;
        }
    }
}

// Duplicar el directorio
void TablaHashEstudiantes::duplicarDirectorio() {
    int tamano = tamanoDirectorio();
    Cubeta** nuevo = new Cubeta*[tamano * 2];

    for (int i = 0; i < tamano; i++) {
        nuevo[i] = directorio[i];
        nuevo[i + tamano] = directorio[i];
    }

    delete[] directorio;
    directorio = nuevo;
    profundidad++;
    duplicaciones++;
}

bool TablaHashEstudiantes::dividirCubeta(int indice) {
    Cubeta* cubeta = directorio[indice];

    // Duplicar el directorio si hace falta
    if (cubeta->profundidadLocal == profundidad) {
        if (profundidad == PROFUNDIDAD_MAXIMA) {
            return false;
        }

        duplicarDirectorio();
    }

    cubeta->profundidadLocal++;
    Cubeta* nueva = crearCubeta(cubeta->profundidadLocal);

    // Apuntar a la cubeta nueva
    int bit = 1 << (cubeta->profundidadLocal - 1);
    int tamano = tamanoDirectorio();

    for (int i = indice & (bit - 1); i < tamano; i += bit) {
        if ((i & bit) != 0) {
            directorio[i] = nueva;
        }
    }

    // Se redistribuyen los registros con el mismo bit
    int quedan = 0;

    for (int i = 0; i < cubeta->cantidad; i++) {
        Estudiante registro = cubeta->registros[i];

        if ((valorHash(registro.carne) & bit) != 0) {
            nueva->registros[nueva->cantidad] = registro;
            nueva->cantidad++;
        } else {
            cubeta->registros[quedan] = registro;
            quedan++;
        }
    }

    cubeta->cantidad = quedan;
    divisiones++;

    return true;
}

// Modificar: el carne es la clave y no cambia

bool TablaHashEstudiantes::modificar(string carne, string nombre, string correo) {
    Estudiante* estudiante = buscar(carne);

    if (estudiante == nullptr) {
        return false;
    }

    estudiante->nombre = nombre;
    estudiante->correo = correo;

    return true;
}

// Eliminacion

bool TablaHashEstudiantes::eliminar(string carne) {
    reiniciarCambios();

    int indice = indiceDe(carne);
    Cubeta* cubeta = directorio[indice];
    int posicion = posicionEnCubeta(cubeta, carne);

    if (posicion == -1) {
        return false;
    }

    // El ultimo registro ocupa el lugar del borrado
    cubeta->registros[posicion] = cubeta->registros[cubeta->cantidad - 1];
    cubeta->cantidad--;
    total--;

    unirCubetas(indice);

    // Reducir el directorio
    if (uniones > 0) {
        reducirDirectorio();
    }

    return true;
}

// Unir la cubeta con su hermana
void TablaHashEstudiantes::unirCubetas(int indice) {
    while (true) {
        Cubeta* cubeta = directorio[indice];

        if (cubeta->profundidadLocal == 0) {
            return;
        }

        int bit = 1 << (cubeta->profundidadLocal - 1);
        Cubeta* hermana = directorio[indice ^ bit];

        if (hermana->profundidadLocal != cubeta->profundidadLocal) {
            return;
        }

        if (cubeta->cantidad + hermana->cantidad > CAPACIDAD_CUBETA) {
            return;
        }

        // Conservar la cubeta del bit en 0
        Cubeta* queda = cubeta;
        Cubeta* sale = hermana;

        if ((indice & bit) != 0) {
            queda = hermana;
            sale = cubeta;
        }

        for (int i = 0; i < sale->cantidad; i++) {
            queda->registros[queda->cantidad] = sale->registros[i];
            queda->cantidad++;
        }

        queda->profundidadLocal--;

        // Apuntar a la cubeta que queda
        int tamano = tamanoDirectorio();

        for (int i = indice & (bit - 1); i < tamano; i += bit) {
            directorio[i] = queda;
        }

        delete sale;
        uniones++;
    }
}

// Reducir el directorio a la mitad
void TablaHashEstudiantes::reducirDirectorio() {
    while (profundidad > 0) {
        int tamano = tamanoDirectorio();

        for (int i = 0; i < tamano; i++) {
            if (directorio[i]->profundidadLocal == profundidad) {
                return;
            }
        }

        int mitad = tamano / 2;
        Cubeta** nuevo = new Cubeta*[mitad];

        for (int i = 0; i < mitad; i++) {
            nuevo[i] = directorio[i];
        }

        delete[] directorio;
        directorio = nuevo;
        profundidad--;
        reducciones++;
    }
}

// Recorrido

void TablaHashEstudiantes::listar(vector<Estudiante>& salida) {
    int tamano = tamanoDirectorio();

    for (int i = 0; i < tamano; i++) {
        Cubeta* cubeta = directorio[i];

        if (i < (1 << cubeta->profundidadLocal)) {
            for (int j = 0; j < cubeta->cantidad; j++) {
                salida.push_back(cubeta->registros[j]);
            }
        }
    }

    sort(salida.begin(), salida.end(), [](const Estudiante& a, const Estudiante& b) {
        return a.carne < b.carne;
    });
}

// Datos de la tabla

int TablaHashEstudiantes::cantidad() {
    return total;
}

bool TablaHashEstudiantes::esVacia() {
    return total == 0;
}

int TablaHashEstudiantes::profundidadGlobal() {
    return profundidad;
}

int TablaHashEstudiantes::tamanoDirectorio() {
    return 1 << profundidad;
}

int TablaHashEstudiantes::contarCubetas() {
    int cubetas = 0;
    int tamano = tamanoDirectorio();

    for (int i = 0; i < tamano; i++) {
        if (i < (1 << directorio[i]->profundidadLocal)) {
            cubetas++;
        }
    }

    return cubetas;
}

double TablaHashEstudiantes::factorDeCarga() {
    return (double)total / (contarCubetas() * CAPACIDAD_CUBETA);
}

// Revisar las reglas del hashing extensible
bool TablaHashEstudiantes::estructuraCorrecta() {
    int tamano = tamanoDirectorio();
    set<Cubeta*> vistas;
    int suma = 0;

    for (int i = 0; i < tamano; i++) {
        Cubeta* cubeta = directorio[i];
        int local = cubeta->profundidadLocal;

        if (local > profundidad || cubeta->cantidad < 0 || cubeta->cantidad > CAPACIDAD_CUBETA) {
            return false;
        }

        int base = i & ((1 << local) - 1);

        if (directorio[base] != cubeta) {
            return false;
        }

        if (i != base) {
            continue;
        }

        if (vistas.count(cubeta) > 0) {
            return false;
        }

        vistas.insert(cubeta);
        suma += cubeta->cantidad;

        for (int j = 0; j < cubeta->cantidad; j++) {
            unsigned long long hash = valorHash(cubeta->registros[j].carne);

            if ((int)(hash & ((1ULL << local) - 1)) != base) {
                return false;
            }
        }
    }

    return suma == total;
}

// Vista interna

void TablaHashEstudiantes::imprimirBits(int valor, int bits) {
    if (bits == 0) {
        cout << "-";
        return;
    }

    for (int i = bits - 1; i >= 0; i--) {
        cout << ((valor >> i) & 1);
    }
}

void TablaHashEstudiantes::mostrarDirectorio() {
    int tamano = tamanoDirectorio();

    cout << "  Profundidad global: " << profundidad
         << "   Entradas: " << tamano
         << "   Cubetas: " << contarCubetas()
         << "   Estudiantes: " << total << "\n";
    cout << "  Factor de carga: " << fixed << setprecision(2) << factorDeCarga()
         << " (capacidad " << CAPACIDAD_CUBETA << " por cubeta)\n\n";
    cout.unsetf(ios::fixed);

    // Mostrar solo las primeras entradas
    int mostrar = tamano;

    if (mostrar > 32) {
        mostrar = 32;
    }

    for (int i = 0; i < mostrar; i++) {
        Cubeta* cubeta = directorio[i];
        int local = cubeta->profundidadLocal;
        int base = i & ((1 << local) - 1);

        cout << "  ";
        imprimirBits(i, profundidad);
        cout << " -> ";

        if (i != base) {
            cout << "misma cubeta que ";
            imprimirBits(base, profundidad);
            cout << "\n";
            continue;
        }

        cout << "[p=" << local << "] ";

        if (cubeta->cantidad == 0) {
            cout << "(vacia)";
        }

        for (int j = 0; j < cubeta->cantidad; j++) {
            cout << cubeta->registros[j].carne << "  ";
        }

        cout << "\n";
    }

    if (mostrar < tamano) {
        cout << "  ... (" << tamano - mostrar << " entradas mas)\n";
    }
}

void TablaHashEstudiantes::mostrarBusqueda(string carne) {
    unsigned long long hash = valorHash(carne);
    int indice = indiceDe(carne);
    Cubeta* cubeta = directorio[indice];

    cout << "  Hash de " << carne << ": " << hash << "\n";
    cout << "  Ultimos " << profundidad << " bits: ";
    imprimirBits(indice, profundidad);
    cout << " -> entrada " << indice << " del directorio\n";
    cout << "  Cubeta [p=" << cubeta->profundidadLocal << "]: ";

    if (cubeta->cantidad == 0) {
        cout << "(vacia)";
    }

    for (int j = 0; j < cubeta->cantidad; j++) {
        cout << cubeta->registros[j].carne << "  ";
    }

    cout << "\n";
}

void TablaHashEstudiantes::resumenCambios() {
    cout << "  Divisiones de cubeta: " << divisiones
         << "   Duplicaciones del directorio: " << duplicaciones << "\n";
    cout << "  Uniones de cubeta: " << uniones
         << "   Reducciones del directorio: " << reducciones << "\n";
}
