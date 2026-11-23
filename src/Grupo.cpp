// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de las funciones de grupo y horario

#include <iostream>
#include "Grupo.h"

using namespace std;

const string DIAS[7] = { "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo" };

// Pasa el texto a minusculas para comparar sin importar mayusculas
string enMinusculas(string texto) {
    for (int i = 0; i < (int)texto.length(); i++) {
        if (texto[i] >= 'A' && texto[i] <= 'Z') {
            texto[i] = texto[i] - 'A' + 'a';
        }
    }

    return texto;
}

int numeroDeDia(string texto) {
    texto = enMinusculas(texto);

    for (int i = 0; i < 7; i++) {
        if (enMinusculas(DIAS[i]) == texto) {
            return i + 1;
        }
    }

    return 0;
}

string nombreDeDia(int dia) {
    if (dia < 1 || dia > 7) {
        return "?";
    }

    return DIAS[dia - 1];
}

bool horaValida(int horaHHMM) {
    int horas = horaHHMM / 100;
    int minutos = horaHHMM % 100;

    return horaHHMM >= 0 && horas <= 23 && minutos <= 59;
}

bool hayChoque(Grupo a, Grupo b) {
    if (a.periodo != b.periodo || a.dia != b.dia) {
        return false;
    }

    // Se traslapan si cada uno empieza antes de que termine el otro
    return a.horaInicio < b.horaFin && b.horaInicio < a.horaFin;
}

bool tieneCupo(Grupo grupo) {
    return grupo.matriculados < grupo.cupo;
}

// Imprime una hora HHMM como HH:MM
void imprimirHora(int horaHHMM) {
    int horas = horaHHMM / 100;
    int minutos = horaHHMM % 100;

    if (horas < 10) {
        cout << "0";
    }

    cout << horas << ":";

    if (minutos < 10) {
        cout << "0";
    }

    cout << minutos;
}

void imprimirHorario(Grupo grupo) {
    cout << nombreDeDia(grupo.dia) << " ";
    imprimirHora(grupo.horaInicio);
    cout << "-";
    imprimirHora(grupo.horaFin);
}
