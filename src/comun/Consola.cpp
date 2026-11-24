// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de la lectura por consola

#include <iostream>
#include "comun/Consola.h"
#include "comun/CargadorArchivos.h"
#include "citas/CitaMatricula.h"
#include "historial/RegistroNota.h"

using namespace std;

static bool cerrada = false;

bool entradaCerrada() {
    return cerrada;
}

// Leer una linea completa
static bool leerLinea(string& linea) {
    if (cerrada || !getline(cin, linea)) {
        cerrada = true;
        linea = "";
        return false;
    }

    linea = recortar(linea);
    return true;
}

// Numero entero
static bool convertirEntero(string texto, int& numero) {
    if (texto.length() == 0 || texto.length() > 9) {
        return false;
    }

    int inicio = 0;
    bool negativo = false;

    if (texto[0] == '-') {
        negativo = true;
        inicio = 1;

        if (texto.length() == 1) {
            return false;
        }
    }

    numero = 0;

    for (int i = inicio; i < (int)texto.length(); i++) {
        if (texto[i] < '0' || texto[i] > '9') {
            return false;
        }

        numero = numero * 10 + (texto[i] - '0');
    }

    if (negativo) {
        numero = -numero;
    }

    return true;
}

int leerEntero(string mensaje) {
    while (true) {
        cout << "  " << mensaje << ": ";

        string linea;

        if (!leerLinea(linea)) {
            cout << "\n";
            return 0;
        }

        int numero;

        if (convertirEntero(linea, numero)) {
            return numero;
        }

        cout << "  Escriba un numero.\n";
    }
}

int leerOpcion() {
    return leerEntero("Opcion");
}

string leerTexto(string mensaje) {
    cout << "  " << mensaje << ": ";

    string linea;
    leerLinea(linea);

    return linea;
}

string leerCarne() {
    return leerTexto("Carne (ej. 2026-0101)");
}

string leerCodigoCurso() {
    string codigo = leerTexto("Codigo del curso (ej. INF-301)");

    // Los codigos se guardan en mayuscula
    for (int i = 0; i < (int)codigo.length(); i++) {
        if (codigo[i] >= 'a' && codigo[i] <= 'z') {
            codigo[i] = codigo[i] - 'a' + 'A';
        }
    }

    return codigo;
}

bool leerSiNo(string mensaje) {
    while (true) {
        string respuesta = leerTexto(mensaje + " (s/n)");

        if (respuesta == "s" || respuesta == "S") {
            return true;
        }

        if (respuesta == "n" || respuesta == "N" || entradaCerrada()) {
            return false;
        }

        cout << "  Responda s o n.\n";
    }
}

bool leerFechaHora(string mensaje, long long& fechaHora) {
    cout << "  " << mensaje << "\n";

    string fecha = leerTexto("  Fecha (DD/MM/AAAA)");
    string partes[3];

    if (separar(fecha, '/', partes, 3) != 3) {
        cout << "  La fecha va con el formato DD/MM/AAAA.\n";
        return false;
    }

    int dia = aNumero(partes[0]);
    int mes = aNumero(partes[1]);
    int anio = aNumero(partes[2]);

    int hora = leerHora(leerTexto("  Hora (HH:MM)"));

    if (hora < 0) {
        cout << "  La hora va con el formato HH:MM.\n";
        return false;
    }

    if (!fechaHoraValida(anio, mes, dia, hora / 100, hora % 100)) {
        cout << "  La fecha o la hora no es valida.\n";
        return false;
    }

    fechaHora = armarFechaHora(anio, mes, dia, hora / 100, hora % 100);
    return true;
}

bool leerHoraDelDia(string mensaje, int& horaHHMM) {
    horaHHMM = leerHora(leerTexto(mensaje + " (HH:MM)"));

    if (horaHHMM < 0) {
        cout << "  La hora va con el formato HH:MM.\n";
        return false;
    }

    return true;
}

bool leerPeriodo(string mensaje, string& periodo) {
    periodo = leerTexto(mensaje + " (ej. 2026-C3)");

    if (periodo.length() == 7 && periodo[5] == 'c') {
        periodo[5] = 'C';
    }

    if (!periodoValido(periodo)) {
        cout << "  El periodo va con el formato AAAA-Cn (n de 1 a 3).\n";
        return false;
    }

    return true;
}

void pausar() {
    if (cerrada) {
        return;
    }

    cout << "\n  Presione ENTER para continuar...";

    string linea;
    leerLinea(linea);
}

void titulo(string texto) {
    cout << "\n==========================================================\n";
    cout << "  " << texto << "\n";
    cout << "==========================================================\n";
}

void columna(string texto, int ancho) {
    cout << texto;

    for (int i = texto.length(); i < ancho; i++) {
        cout << " ";
    }
}
