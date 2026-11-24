// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de las funciones de la cita de matricula

#include <iostream>
#include <cstdio>
#include "citas/CitaMatricula.h"

using namespace std;

// Armar y validar

long long armarFechaHora(int anio, int mes, int dia, int hora, int minuto) {
    long long fechaHora = anio;

    fechaHora = fechaHora * 100 + mes;
    fechaHora = fechaHora * 100 + dia;
    fechaHora = fechaHora * 100 + hora;
    fechaHora = fechaHora * 100 + minuto;

    return fechaHora;
}

bool fechaHoraValida(int anio, int mes, int dia, int hora, int minuto) {
    if (anio < 2000 || anio > 2100) {
        return false;
    }

    if (mes < 1 || mes > 12) {
        return false;
    }

    int diasDelMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Febrero en anio bisiesto
    if (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0)) {
        diasDelMes[1] = 29;
    }

    if (dia < 1 || dia > diasDelMes[mes - 1]) {
        return false;
    }

    if (hora < 0 || hora > 23) {
        return false;
    }

    if (minuto < 0 || minuto > 59) {
        return false;
    }

    return true;
}

// Suma minutos y pasa al dia siguiente si hace falta
long long sumarMinutos(long long fechaHora, int minutos) {
    int minuto = fechaHora % 100;
    int hora = (fechaHora / 100) % 100;
    int dia = (fechaHora / 10000) % 100;
    int mes = (fechaHora / 1000000) % 100;
    int anio = fechaHora / 100000000;

    int total = hora * 60 + minuto + minutos;

    while (total >= 24 * 60) {
        total -= 24 * 60;
        dia++;

        if (!fechaHoraValida(anio, mes, dia, 0, 0)) {
            dia = 1;
            mes++;

            if (mes > 12) {
                mes = 1;
                anio++;
            }
        }
    }

    return armarFechaHora(anio, mes, dia, total / 60, total % 60);
}

// Imprimir

void imprimirDosDigitos(int numero) {
    if (numero < 10) {
        cout << "0";
    }

    cout << numero;
}

// Formato DD/MM/AAAA HH:MM
void imprimirFechaHora(long long fechaHora) {
    int minuto = fechaHora % 100;
    int hora = (fechaHora / 100) % 100;
    int dia = (fechaHora / 10000) % 100;
    int mes = (fechaHora / 1000000) % 100;
    int anio = fechaHora / 100000000;

    imprimirDosDigitos(dia);
    cout << "/";
    imprimirDosDigitos(mes);
    cout << "/" << anio << " ";
    imprimirDosDigitos(hora);
    cout << ":";
    imprimirDosDigitos(minuto);
}

// Formato corto DD/MM HH:MM
void imprimirDiaHora(long long fechaHora) {
    int minuto = fechaHora % 100;
    int hora = (fechaHora / 100) % 100;
    int dia = (fechaHora / 10000) % 100;
    int mes = (fechaHora / 1000000) % 100;

    imprimirDosDigitos(dia);
    cout << "/";
    imprimirDosDigitos(mes);
    cout << " ";
    imprimirDosDigitos(hora);
    cout << ":";
    imprimirDosDigitos(minuto);
}

// La cita ya se puede usar si su hora ya llego
bool estaHabilitada(CitaMatricula cita, long long horaActual) {
    return cita.fechaHora <= horaActual;
}

string textoFechaHora(long long fechaHora) {
    char texto[20];

    snprintf(texto, sizeof(texto), "%02d/%02d/%04d %02d:%02d", (int)((fechaHora / 10000) % 100),
             (int)((fechaHora / 1000000) % 100), (int)(fechaHora / 100000000), (int)((fechaHora / 100) % 100),
             (int)(fechaHora % 100));

    return texto;
}
