// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion de la lectura de archivos

#include <iostream>
#include <fstream>
#include "comun/CargadorArchivos.h"

using namespace std;

// Funciones de apoyo

// Quitar espacios de los extremos
string recortar(string texto) {
    int inicio = 0;
    int fin = texto.length() - 1;

    while (inicio <= fin && (texto[inicio] == ' ' || texto[inicio] == '\t' || texto[inicio] == '\r')) {
        inicio++;
    }

    while (fin >= inicio && (texto[fin] == ' ' || texto[fin] == '\t' || texto[fin] == '\r')) {
        fin--;
    }

    return texto.substr(inicio, fin - inicio + 1);
}

// Separar la linea en campos
int separar(string linea, char separador, string campos[], int maximo) {
    int cantidad = 0;
    string campo = "";

    for (int i = 0; i < (int)linea.length(); i++) {
        if (linea[i] == separador) {
            if (cantidad < maximo) {
                campos[cantidad] = recortar(campo);
            }

            cantidad++;
            campo = "";
        } else {
            campo = campo + linea[i];
        }
    }

    if (cantidad < maximo) {
        campos[cantidad] = recortar(campo);
    }

    cantidad++;
    return cantidad;
}

// Texto de digitos a numero
int aNumero(string texto) {
    if (texto.length() == 0 || texto.length() > 4) {
        return -1;
    }

    int numero = 0;

    for (int i = 0; i < (int)texto.length(); i++) {
        if (texto[i] < '0' || texto[i] > '9') {
            return -1;
        }

        numero = numero * 10 + (texto[i] - '0');
    }

    return numero;
}

// Lineas vacias y comentarios
bool esLineaDeDatos(string linea) {
    return linea.length() > 0 && linea[0] != '#';
}

void avisarLinea(string ruta, int numeroLinea, string motivo) {
    cout << "  Aviso: " << ruta << ", linea " << numeroLinea << ": " << motivo << "\n";
}

// Lectura generica de un archivo

const int MAX_CAMPOS = 8;

struct Fila {
    int numeroLinea;
    string campos[MAX_CAMPOS];
};

bool leerFilas(string ruta, int cantidadCampos, vector<Fila>& filas, int& lineasConError) {
    ifstream archivo(ruta);
    lineasConError = 0;

    if (!archivo.is_open()) {
        return false;
    }

    string linea;
    int numeroLinea = 0;

    while (getline(archivo, linea)) {
        numeroLinea++;
        linea = recortar(linea);

        if (!esLineaDeDatos(linea)) {
            continue;
        }

        Fila fila;
        fila.numeroLinea = numeroLinea;

        int cantidad = separar(linea, ';', fila.campos, MAX_CAMPOS);

        if (cantidad != cantidadCampos) {
            avisarLinea(ruta, numeroLinea, "se esperaban " + to_string(cantidadCampos) + " campos");
            lineasConError++;
            continue;
        }

        filas.push_back(fila);
    }

    archivo.close();
    return true;
}

// Avisar una fila con error
void filaConError(string ruta, Fila& fila, string motivo, int& lineasConError) {
    avisarLinea(ruta, fila.numeroLinea, motivo);
    lineasConError++;
}

int leerHora(string texto) {
    string partes[2];

    if (separar(texto, ':', partes, 2) != 2 || partes[0].length() != 2 || partes[1].length() != 2) {
        return -1;
    }

    int horas = aNumero(partes[0]);
    int minutos = aNumero(partes[1]);

    if (horas < 0 || horas > 23 || minutos < 0 || minutos > 59) {
        return -1;
    }

    return horas * 100 + minutos;
}

// Estudiantes: carne;nombre;correo

bool leerEstudiantes(string ruta, vector<Estudiante>& estudiantes, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 3, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;

        if (campos[0] == "" || campos[1] == "") {
            filaConError(ruta, filas[i], "falta el carne o el nombre", lineasConError);
            continue;
        }

        Estudiante estudiante;

        estudiante.carne = campos[0];
        estudiante.nombre = campos[1];
        estudiante.correo = campos[2];

        estudiantes.push_back(estudiante);
    }

    return true;
}

// Citas: AAAA-MM-DD;HH:MM;carne

bool leerCitas(string ruta, vector<CitaMatricula>& citas, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 3, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;
        string fecha[3];
        string hora[2];

        if (separar(campos[0], '-', fecha, 3) != 3 || separar(campos[1], ':', hora, 2) != 2) {
            filaConError(ruta, filas[i], "la fecha va AAAA-MM-DD y la hora HH:MM", lineasConError);
            continue;
        }

        int anio = aNumero(fecha[0]);
        int mes = aNumero(fecha[1]);
        int dia = aNumero(fecha[2]);
        int horas = aNumero(hora[0]);
        int minutos = aNumero(hora[1]);

        if (!fechaHoraValida(anio, mes, dia, horas, minutos)) {
            filaConError(ruta, filas[i], "fecha u hora invalida", lineasConError);
            continue;
        }

        if (campos[2] == "") {
            filaConError(ruta, filas[i], "falta el carne", lineasConError);
            continue;
        }

        CitaMatricula cita;

        cita.fechaHora = armarFechaHora(anio, mes, dia, horas, minutos);
        cita.carne = campos[2];

        citas.push_back(cita);
    }

    return true;
}

// Cursos: codigo;nombre;creditos

bool leerCursos(string ruta, vector<Curso>& cursos, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 3, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;
        int creditos = aNumero(campos[2]);

        if (campos[0] == "" || campos[1] == "") {
            filaConError(ruta, filas[i], "falta el codigo o el nombre", lineasConError);
            continue;
        }

        if (creditos <= 0 || creditos > 12) {
            filaConError(ruta, filas[i], "los creditos van de 1 a 12", lineasConError);
            continue;
        }

        Curso curso;

        curso.codigo = campos[0];
        curso.nombre = campos[1];
        curso.creditos = creditos;

        cursos.push_back(curso);
    }

    return true;
}

// Requisitos: requisito;curso

bool leerRequisitos(string ruta, vector<Requisito>& requisitos, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 2, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;

        if (campos[0] == "" || campos[1] == "") {
            filaConError(ruta, filas[i], "falta uno de los dos cursos", lineasConError);
            continue;
        }

        if (campos[0] == campos[1]) {
            filaConError(ruta, filas[i], "un curso no puede ser requisito de si mismo", lineasConError);
            continue;
        }

        Requisito requisito;

        requisito.requisito = campos[0];
        requisito.curso = campos[1];

        requisitos.push_back(requisito);
    }

    return true;
}

// Grupos

bool leerGrupos(string ruta, vector<Grupo>& grupos, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 7, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;

        int numero = aNumero(campos[1]);
        int dia = numeroDeDia(campos[3]);
        int inicio = leerHora(campos[4]);
        int fin = leerHora(campos[5]);
        int cupo = aNumero(campos[6]);

        if (campos[0] == "" || numero <= 0) {
            filaConError(ruta, filas[i], "falta el curso o el numero de grupo", lineasConError);
            continue;
        }

        if (!periodoValido(campos[2])) {
            filaConError(ruta, filas[i], "el periodo va AAAA-Cn", lineasConError);
            continue;
        }

        if (dia == 0) {
            filaConError(ruta, filas[i], "dia invalido", lineasConError);
            continue;
        }

        if (inicio < 0 || fin < 0 || inicio >= fin) {
            filaConError(ruta, filas[i], "horario invalido", lineasConError);
            continue;
        }

        if (cupo <= 0) {
            filaConError(ruta, filas[i], "el cupo debe ser mayor que cero", lineasConError);
            continue;
        }

        Grupo grupo;

        grupo.codigoCurso = campos[0];
        grupo.numero = numero;
        grupo.periodo = campos[2];
        grupo.dia = dia;
        grupo.horaInicio = inicio;
        grupo.horaFin = fin;
        grupo.cupo = cupo;
        grupo.matriculados = 0;

        grupos.push_back(grupo);
    }

    return true;
}

// Notas: carne;periodo;curso;nota

bool leerNotas(string ruta, vector<RegistroNota>& notas, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 4, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;
        int nota = aNumero(campos[3]);

        if (campos[0] == "" || campos[2] == "") {
            filaConError(ruta, filas[i], "falta el carne o el curso", lineasConError);
            continue;
        }

        if (!periodoValido(campos[1])) {
            filaConError(ruta, filas[i], "el periodo va AAAA-Cn", lineasConError);
            continue;
        }

        if (nota < 0 || nota > 100) {
            filaConError(ruta, filas[i], "la nota va de 0 a 100", lineasConError);
            continue;
        }

        RegistroNota registro;

        registro.carne = campos[0];
        registro.periodo = campos[1];
        registro.codigoCurso = campos[2];
        registro.nota = nota;

        notas.push_back(registro);
    }

    return true;
}

// Matriculas: carne;curso;grupo;periodo;estado

bool leerMatriculas(string ruta, vector<Matricula>& matriculas, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 5, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;
        int numero = aNumero(campos[2]);

        if (campos[0] == "" || campos[1] == "" || numero <= 0) {
            filaConError(ruta, filas[i], "falta el carne, el curso o el grupo", lineasConError);
            continue;
        }

        if (!periodoValido(campos[3])) {
            filaConError(ruta, filas[i], "el periodo va AAAA-Cn", lineasConError);
            continue;
        }

        if (campos[4] != "activa" && campos[4] != "retirada") {
            filaConError(ruta, filas[i], "el estado es activa o retirada", lineasConError);
            continue;
        }

        Matricula matricula;

        matricula.carne = campos[0];
        matricula.codigoCurso = campos[1];
        matricula.numeroGrupo = numero;
        matricula.periodo = campos[3];
        matricula.activa = campos[4] == "activa";

        matriculas.push_back(matricula);
    }

    return true;
}

// Lista de espera: curso;grupo;periodo;carne

bool leerEspera(string ruta, vector<EnEspera>& espera, int& lineasConError) {
    vector<Fila> filas;

    if (!leerFilas(ruta, 4, filas, lineasConError)) {
        return false;
    }

    for (int i = 0; i < (int)filas.size(); i++) {
        string* campos = filas[i].campos;
        int numero = aNumero(campos[1]);

        if (campos[0] == "" || numero <= 0 || campos[3] == "") {
            filaConError(ruta, filas[i], "falta el curso, el grupo o el carne", lineasConError);
            continue;
        }

        if (!periodoValido(campos[2])) {
            filaConError(ruta, filas[i], "el periodo va AAAA-Cn", lineasConError);
            continue;
        }

        EnEspera registro;

        registro.codigoCurso = campos[0];
        registro.numeroGrupo = numero;
        registro.periodo = campos[2];
        registro.carne = campos[3];

        espera.push_back(registro);
    }

    return true;
}
