// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas de la cola de espera

#include <iostream>
#include <cstdlib>
#include <deque>
#include <algorithm>
#include <vector>
#include "Pruebas.h"
#include "matricula/ColaEspera.h"

using namespace std;

void probarColaBasica() {
    cout << "\nCola de espera\n";

    ColaEspera cola;
    string carne;

    revisar(cola.esVacia() && cola.frente() == "", "cola nueva vacia");
    revisar(!cola.desencolar(carne), "no se puede desencolar de una cola vacia");

    revisar(cola.encolar("2026-0104") && cola.encolar("2026-0102") && cola.encolar("2026-0107"), "encola 0104, 0102 y 0107");
    revisar(!cola.encolar("2026-0102"), "rechaza a un estudiante que ya esta en la cola");
    revisar(cola.cantidad() == 3 && cola.frente() == "2026-0104", "3 en la cola, 0104 al frente");
    revisar(cola.posicionDe("2026-0107") == 3 && cola.posicionDe("2026-0199") == 0, "posiciones en la cola");

    cout << "  ";
    cola.mostrar();
    cout << "\n";

    revisar(cola.sacar("2026-0102"), "saca a 0102 del medio");
    revisar(cola.cantidad() == 2 && !cola.contiene("2026-0102"), "quedan 2 y 0102 ya no esta");

    revisar(cola.desencolar(carne) && carne == "2026-0104", "sale primero 0104 (el primero que llego)");
    revisar(cola.desencolar(carne) && carne == "2026-0107", "luego 0107");
    revisar(cola.esVacia(), "la cola queda vacia");

    // Sacar al ultimo
    cola.encolar("2026-0101");
    cola.encolar("2026-0105");
    cola.sacar("2026-0105");
    cola.encolar("2026-0103");

    vector<string> lista;
    cola.listar(lista);
    revisar(lista.size() == 2 && lista[0] == "2026-0101" && lista[1] == "2026-0103", "al sacar al ultimo se puede seguir encolando bien");
}

// Operaciones al azar comparadas con std::deque

void probarColaAlAzar() {
    cout << "\nCola de espera: 30 000 operaciones al azar contra std::deque\n";

    ColaEspera cola;
    deque<string> esperado;
    srand(2024);

    bool todoBien = true;

    for (int i = 0; i < 30000; i++) {
        string carne = "2026-0" + to_string(100 + rand() % 80);
        int operacion = rand() % 4;

        if (operacion <= 1) {
            bool estaba = find(esperado.begin(), esperado.end(), carne) != esperado.end();

            if (cola.encolar(carne) == estaba) {
                todoBien = false;
            }

            if (!estaba) {
                esperado.push_back(carne);
            }
        } else if (operacion == 2) {
            string sale;
            bool salio = cola.desencolar(sale);

            if (salio != !esperado.empty()) {
                todoBien = false;
            }

            if (salio) {
                if (sale != esperado.front()) {
                    todoBien = false;
                }

                esperado.pop_front();
            }
        } else {
            auto posicion = find(esperado.begin(), esperado.end(), carne);
            bool estaba = posicion != esperado.end();

            if (cola.sacar(carne) != estaba) {
                todoBien = false;
            }

            if (estaba) {
                esperado.erase(posicion);
            }
        }

        if (cola.cantidad() != (int)esperado.size()) {
            todoBien = false;
        }
    }

    vector<string> lista;
    cola.listar(lista);

    revisar(todoBien, "encolar, desencolar y sacar responden igual que std::deque");
    revisar(lista == vector<string>(esperado.begin(), esperado.end()), "el contenido final es el mismo y en el mismo orden");
}
