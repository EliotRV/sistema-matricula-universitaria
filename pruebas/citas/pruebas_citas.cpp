// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas del arbol rojo-negro de citas

#include <iostream>
#include <cstdlib>
#include <set>
#include <vector>
#include "Pruebas.h"
#include "citas/ArbolRNCitas.h"

using namespace std;

// Apoyo

CitaMatricula citaDe(int hora, int minuto, string carne) {
    CitaMatricula cita;

    cita.fechaHora = armarFechaHora(2026, 12, 7, hora, minuto);
    cita.carne = carne;

    return cita;
}

// Citas cada 15 minutos
void llenar(ArbolRNCitas& arbol, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        int minutos = 8 * 60 + i * 15;
        arbol.insertar(citaDe(minutos / 60, minutos % 60, "2026-01" + to_string(i)));
    }
}

long long hora(int horas, int minutos) {
    return armarFechaHora(2026, 12, 7, horas, minutos);
}

// Insercion y busqueda

void probarInsercion() {
    cout << "\nInsercion y busqueda\n";

    ArbolRNCitas arbol;

    revisar(arbol.esVacio() && arbol.propiedadesCorrectas(), "arbol nuevo vacio");

    revisar(arbol.insertar(citaDe(8, 0, "2026-0103")), "inserta 08:00");
    revisar(arbol.insertar(citaDe(8, 15, "2026-0105")), "inserta 08:15");
    revisar(arbol.insertar(citaDe(8, 30, "2026-0101")), "inserta 08:30");
    revisar(arbol.insertar(citaDe(8, 45, "2026-0104")), "inserta 08:45");
    revisar(arbol.insertar(citaDe(9, 0, "2026-0102")), "inserta 09:00");

    revisar(!arbol.insertar(citaDe(8, 30, "2026-0199")), "rechaza la hora repetida");
    revisar(arbol.contarNodos() == 5, "quedan 5 citas");
    revisar(arbol.propiedadesCorrectas(), "propiedades correctas");

    cout << "\n";
    arbol.mostrarArbol();
    cout << "\n";

    CitaMatricula* cita = arbol.buscar(hora(8, 45));
    revisar(cita != nullptr && cita->carne == "2026-0104", "busca 08:45 y es de 2026-0104");
    revisar(arbol.buscar(hora(8, 50)) == nullptr, "08:50 no existe");

    cita = arbol.buscarPorCarne("2026-0102");
    revisar(cita != nullptr && cita->fechaHora == hora(9, 0), "la cita de 2026-0102 es a las 09:00");
    revisar(arbol.buscarPorCarne("2026-0999") == nullptr, "2026-0999 no tiene cita");

    vector<CitaMatricula> citas;
    arbol.enOrden(citas);
    revisar(citas.size() == 5 && citas[0].fechaHora == hora(8, 0) && citas[4].fechaHora == hora(9, 0), "recorrido en orden");

    citas.clear();
    arbol.rango(hora(8, 15), hora(8, 45), citas);
    revisar(citas.size() == 3 && citas[0].fechaHora == hora(8, 15) && citas[2].fechaHora == hora(8, 45), "rango 08:15 a 08:45 trae 3");

    citas.clear();
    arbol.rango(hora(8, 20), hora(8, 25), citas);
    revisar(citas.size() == 0, "rango sin citas trae 0");

    citas.clear();
    arbol.rango(hora(7, 0), hora(12, 0), citas);
    revisar(citas.size() == 5, "rango amplio trae las 5");
}

// Eliminacion caso por caso

void probarEliminacion() {
    cout << "\nEliminacion\n";

    {
        ArbolRNCitas arbol;
        llenar(arbol, 5);

        revisar(!arbol.eliminar(hora(8, 50)), "eliminar una cita que no existe devuelve false");
        revisar(arbol.contarNodos() == 5, "el arbol no cambia");

        revisar(arbol.eliminar(hora(9, 0)) && arbol.propiedadesCorrectas(), "elimina hoja roja (09:00)");
        revisar(arbol.buscar(hora(9, 0)) == nullptr && arbol.contarNodos() == 4, "09:00 ya no esta");

        revisar(arbol.eliminar(hora(8, 45)) && arbol.propiedadesCorrectas(), "elimina nodo negro con un hijo rojo (08:45)");
        revisar(arbol.eliminar(hora(8, 0)) && arbol.propiedadesCorrectas(), "elimina hoja negra (08:00)");
        revisar(arbol.eliminar(hora(8, 15)) && arbol.propiedadesCorrectas(), "elimina la raiz (08:15)");
        revisar(arbol.eliminar(hora(8, 30)) && arbol.esVacio(), "elimina el ultimo y queda vacio");
        revisar(!arbol.eliminar(hora(8, 30)), "eliminar en arbol vacio devuelve false");
    }

    {
        ArbolRNCitas arbol;
        llenar(arbol, 12);

        revisar(arbol.eliminar(hora(8, 45)) && arbol.propiedadesCorrectas(), "elimina la raiz con dos hijos (08:45 de 12)");
        revisar(arbol.eliminar(hora(9, 45)) && arbol.propiedadesCorrectas(), "elimina nodo interno con dos hijos (09:45)");
        revisar(arbol.contarNodos() == 10, "quedan 10 citas");

        vector<CitaMatricula> citas;
        arbol.enOrden(citas);

        bool ordenado = true;
        for (int i = 1; i < (int)citas.size(); i++) {
            if (citas[i - 1].fechaHora >= citas[i].fechaHora) {
                ordenado = false;
            }
        }

        revisar(ordenado, "el recorrido sigue ordenado");
    }

    // Eliminar cada posicion
    bool todoBien = true;

    for (int cantidad = 1; cantidad <= 40; cantidad++) {
        for (int posicion = 0; posicion < cantidad; posicion++) {
            ArbolRNCitas arbol;
            llenar(arbol, cantidad);

            int minutos = 8 * 60 + posicion * 15;

            if (!arbol.eliminar(hora(minutos / 60, minutos % 60))) {
                todoBien = false;
            }

            if (!arbol.propiedadesCorrectas() || arbol.contarNodos() != cantidad - 1) {
                todoBien = false;
            }
        }
    }

    revisar(todoBien, "eliminar cada posicion en arboles de 1 a 40 citas");
}

// Operaciones al azar comparadas con un set

void probarAlAzar() {
    cout << "\nOperaciones al azar\n";

    srand(2026);

    bool propiedades = true;
    bool mismosDatos = true;
    bool mismosResultados = true;
    long long operaciones = 0;

    for (int ronda = 0; ronda < 300; ronda++) {
        ArbolRNCitas arbol;
        set<long long> referencia;
        int claves = 5 + rand() % 200;

        for (int i = 0; i < 600; i++) {
            long long clave = 202612070000LL + rand() % claves;
            int operacion = rand() % 3;
            operaciones++;

            if (operacion == 0) {
                CitaMatricula cita;
                cita.fechaHora = clave;
                cita.carne = "x";

                bool insertado = arbol.insertar(cita);
                bool esperado = referencia.insert(clave).second;

                if (insertado != esperado) {
                    mismosResultados = false;
                }
            } else if (operacion == 1) {
                bool eliminado = arbol.eliminar(clave);
                bool esperado = referencia.erase(clave) == 1;

                if (eliminado != esperado) {
                    mismosResultados = false;
                }
            } else {
                bool encontrado = arbol.buscar(clave) != nullptr;
                bool esperado = referencia.count(clave) == 1;

                if (encontrado != esperado) {
                    mismosResultados = false;
                }
            }

            if (!arbol.propiedadesCorrectas()) {
                propiedades = false;
            }
        }

        // El recorrido tiene que coincidir con el set
        vector<CitaMatricula> citas;
        arbol.enOrden(citas);

        if (citas.size() != referencia.size()) {
            mismosDatos = false;
        } else {
            int i = 0;
            for (long long clave : referencia) {
                if (citas[i].fechaHora != clave) {
                    mismosDatos = false;
                }
                i++;
            }
        }

        // Vaciar eliminando una por una
        for (long long clave : referencia) {
            if (!arbol.eliminar(clave) || !arbol.propiedadesCorrectas()) {
                propiedades = false;
            }
        }

        if (!arbol.esVacio()) {
            mismosDatos = false;
        }
    }

    cout << "  " << operaciones << " operaciones en 300 arboles\n";

    revisar(mismosResultados, "insertar, eliminar y buscar devuelven lo mismo que el set");
    revisar(propiedades, "propiedades correctas despues de cada operacion");
    revisar(mismosDatos, "el contenido coincide con el set y se puede vaciar");
}

// Altura con muchas citas en orden

void probarAltura() {
    cout << "\nAltura\n";

    ArbolRNCitas arbol;

    for (int i = 0; i < 1000; i++) {
        CitaMatricula cita;
        cita.fechaHora = 202612070000LL + i;
        cita.carne = "x";
        arbol.insertar(cita);
    }

    cout << "  1000 citas insertadas en orden: altura " << arbol.altura() << "\n";

    // Altura maxima de un rojo-negro
    revisar(arbol.altura() <= 19 && arbol.propiedadesCorrectas(), "la altura no pasa de 19");
}

// Reprogramar (modificar la hora de una cita)

void probarReprogramar() {
    cout << "\nReprogramar citas\n";

    ArbolRNCitas arbol;
    llenar(arbol, 12);

    string carne = arbol.buscar(hora(8, 45))->carne;

    revisar(arbol.reprogramar(hora(8, 45), hora(11, 0)), "reprograma la cita de las 08:45 a las 11:00");
    revisar(arbol.buscar(hora(8, 45)) == nullptr, "a las 08:45 ya no hay cita");
    revisar(arbol.buscar(hora(11, 0)) != nullptr && arbol.buscar(hora(11, 0))->carne == carne, "a las 11:00 esta la misma cita");
    revisar(arbol.contarNodos() == 12 && arbol.propiedadesCorrectas(), "siguen 12 citas y las propiedades se cumplen");

    revisar(!arbol.reprogramar(hora(9, 0), hora(9, 15)), "no reprograma a una hora ocupada");
    revisar(!arbol.reprogramar(hora(7, 0), hora(7, 30)), "no reprograma una cita que no existe");
    revisar(arbol.buscar(hora(9, 0)) != nullptr, "la cita de las 09:00 sigue en su lugar");
}
