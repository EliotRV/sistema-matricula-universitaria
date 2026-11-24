// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Conversion entre entidades y documentos de la nube

#include "comun/Colecciones.h"

using namespace std;

// Texto de fechas y horas

static string dosDigitos(int numero) {
    return (numero < 10 ? "0" : "") + to_string(numero);
}

string textoFecha(long long fechaHora) {
    int dia = (fechaHora / 10000) % 100;
    int mes = (fechaHora / 1000000) % 100;
    int anio = fechaHora / 100000000;

    return to_string(anio) + "-" + dosDigitos(mes) + "-" + dosDigitos(dia);
}

string textoHora(long long fechaHora) {
    return dosDigitos((fechaHora / 100) % 100) + ":" + dosDigitos(fechaHora % 100);
}

string textoHoraDelDia(int horaHHMM) {
    return dosDigitos(horaHHMM / 100) + ":" + dosDigitos(horaHHMM % 100);
}

// Entidad -> documento

Documento documentoDe(Estudiante estudiante) {
    Documento documento;

    documento.poner("carne", estudiante.carne);
    documento.poner("nombre", estudiante.nombre);
    documento.poner("correo", estudiante.correo);

    return documento;
}

Documento documentoDe(Curso curso) {
    Documento documento;

    documento.poner("codigo", curso.codigo);
    documento.poner("nombre", curso.nombre);
    documento.poner("creditos", to_string(curso.creditos));

    return documento;
}

Documento documentoDe(Requisito requisito) {
    Documento documento;

    documento.poner("requisito", requisito.requisito);
    documento.poner("curso", requisito.curso);

    return documento;
}

Documento documentoDe(RegistroNota nota) {
    Documento documento;

    documento.poner("carne", nota.carne);
    documento.poner("periodo", nota.periodo);
    documento.poner("curso", nota.codigoCurso);
    documento.poner("nota", to_string(nota.nota));

    return documento;
}

Documento documentoDe(CitaMatricula cita) {
    Documento documento;

    documento.poner("carne", cita.carne);
    documento.poner("fecha", textoFecha(cita.fechaHora));
    documento.poner("hora", textoHora(cita.fechaHora));

    return documento;
}

Documento documentoDe(Grupo grupo) {
    Documento documento;

    documento.poner("curso", grupo.codigoCurso);
    documento.poner("grupo", to_string(grupo.numero));
    documento.poner("periodo", grupo.periodo);
    documento.poner("dia", nombreDeDia(grupo.dia));
    documento.poner("inicio", textoHoraDelDia(grupo.horaInicio));
    documento.poner("fin", textoHoraDelDia(grupo.horaFin));
    documento.poner("cupo", to_string(grupo.cupo));

    return documento;
}

Documento documentoDe(Matricula matricula) {
    Documento documento;

    documento.poner("carne", matricula.carne);
    documento.poner("curso", matricula.codigoCurso);
    documento.poner("grupo", to_string(matricula.numeroGrupo));
    documento.poner("periodo", matricula.periodo);
    documento.poner("estado", matricula.activa ? "activa" : "retirada");

    return documento;
}

// La llegada ordena la cola al volver a cargarla
Documento documentoDe(EnEspera espera, long long llegada) {
    Documento documento;

    documento.poner("carne", espera.carne);
    documento.poner("curso", espera.codigoCurso);
    documento.poner("grupo", to_string(espera.numeroGrupo));
    documento.poner("periodo", espera.periodo);
    documento.poner("llegada", to_string(llegada));

    return documento;
}

// Documento -> entidad

bool leerDocumento(Documento& documento, Estudiante& estudiante) {
    estudiante.carne = documento.valor("carne");
    estudiante.nombre = documento.valor("nombre");
    estudiante.correo = documento.valor("correo");
    estudiante.cita = 0;

    return carneValido(estudiante.carne) && estudiante.nombre != "";
}

bool leerDocumento(Documento& documento, Curso& curso) {
    curso.codigo = documento.valor("codigo");
    curso.nombre = documento.valor("nombre");
    curso.creditos = aNumero(documento.valor("creditos"));

    return curso.codigo != "" && curso.nombre != "" && curso.creditos > 0;
}

bool leerDocumento(Documento& documento, Requisito& requisito) {
    requisito.requisito = documento.valor("requisito");
    requisito.curso = documento.valor("curso");

    return requisito.requisito != "" && requisito.curso != "";
}

bool leerDocumento(Documento& documento, RegistroNota& nota) {
    nota.carne = documento.valor("carne");
    nota.periodo = documento.valor("periodo");
    nota.codigoCurso = documento.valor("curso");
    nota.nota = aNumero(documento.valor("nota"));

    return nota.carne != "" && periodoValido(nota.periodo) && nota.codigoCurso != "" && nota.nota >= 0 && nota.nota <= 100;
}

bool leerDocumento(Documento& documento, CitaMatricula& cita) {
    string partes[3];

    if (separar(documento.valor("fecha"), '-', partes, 3) != 3) {
        return false;
    }

    int anio = aNumero(partes[0]);
    int mes = aNumero(partes[1]);
    int dia = aNumero(partes[2]);
    int hora = leerHora(documento.valor("hora"));

    if (hora < 0 || !fechaHoraValida(anio, mes, dia, hora / 100, hora % 100)) {
        return false;
    }

    cita.fechaHora = armarFechaHora(anio, mes, dia, hora / 100, hora % 100);
    cita.carne = documento.valor("carne");

    return cita.carne != "";
}

bool leerDocumento(Documento& documento, Grupo& grupo) {
    grupo.codigoCurso = documento.valor("curso");
    grupo.numero = aNumero(documento.valor("grupo"));
    grupo.periodo = documento.valor("periodo");
    grupo.dia = numeroDeDia(documento.valor("dia"));
    grupo.horaInicio = leerHora(documento.valor("inicio"));
    grupo.horaFin = leerHora(documento.valor("fin"));
    grupo.cupo = aNumero(documento.valor("cupo"));
    grupo.matriculados = 0;

    return grupo.codigoCurso != "" && grupo.numero > 0 && periodoValido(grupo.periodo) && grupo.dia > 0
           && grupo.horaInicio >= 0 && grupo.horaFin > grupo.horaInicio && grupo.cupo > 0;
}

bool leerDocumento(Documento& documento, Matricula& matricula) {
    matricula.carne = documento.valor("carne");
    matricula.codigoCurso = documento.valor("curso");
    matricula.numeroGrupo = aNumero(documento.valor("grupo"));
    matricula.periodo = documento.valor("periodo");
    matricula.activa = documento.valor("estado") == "activa";

    string estado = documento.valor("estado");

    return matricula.carne != "" && matricula.codigoCurso != "" && matricula.numeroGrupo > 0
           && periodoValido(matricula.periodo) && (estado == "activa" || estado == "retirada");
}

bool leerDocumento(Documento& documento, EnEspera& espera) {
    espera.carne = documento.valor("carne");
    espera.codigoCurso = documento.valor("curso");
    espera.numeroGrupo = aNumero(documento.valor("grupo"));
    espera.periodo = documento.valor("periodo");

    return espera.carne != "" && espera.codigoCurso != "" && espera.numeroGrupo > 0 && periodoValido(espera.periodo);
}

// Filtros

Documento filtroEstudiante(string carne) {
    Documento filtro;
    filtro.poner("carne", carne);
    return filtro;
}

Documento filtroCurso(string codigo) {
    Documento filtro;
    filtro.poner("codigo", codigo);
    return filtro;
}

Documento filtroRequisito(string requisito, string curso) {
    Documento filtro;
    filtro.poner("requisito", requisito);
    filtro.poner("curso", curso);
    return filtro;
}

Documento filtroNota(string carne, string periodo, string codigoCurso) {
    Documento filtro;
    filtro.poner("carne", carne);
    filtro.poner("periodo", periodo);
    filtro.poner("curso", codigoCurso);
    return filtro;
}

Documento filtroCita(string carne) {
    Documento filtro;
    filtro.poner("carne", carne);
    return filtro;
}

Documento filtroGrupo(string codigoCurso, int numero, string periodo) {
    Documento filtro;
    filtro.poner("curso", codigoCurso);
    filtro.poner("grupo", to_string(numero));
    filtro.poner("periodo", periodo);
    return filtro;
}

Documento filtroMatriculaActiva(string carne, string codigoCurso, int numero, string periodo) {
    Documento filtro = filtroGrupo(codigoCurso, numero, periodo);
    filtro.poner("carne", carne);
    filtro.poner("estado", "activa");
    return filtro;
}

Documento filtroEspera(string carne, string codigoCurso, int numero, string periodo) {
    Documento filtro = filtroGrupo(codigoCurso, numero, periodo);
    filtro.poner("carne", carne);
    return filtro;
}
