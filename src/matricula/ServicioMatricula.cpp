// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del componente de matricula

#include <iostream>
#include "matricula/ServicioMatricula.h"
#include "comun/Consola.h"

using namespace std;

ServicioMatricula::ServicioMatricula(ServicioEstudiantes& estudiantes, ServicioPlan& plan,
                                     ServicioHistorial& historial, ServicioCitas& citas)
    : estudiantes(estudiantes), plan(plan), historial(historial), citas(citas) {
    primero = nullptr;
    periodoActual = "2027-C1";
}

ServicioMatricula::~ServicioMatricula() {
    vaciar();
}

void ServicioMatricula::vaciar() {
    while (primero != nullptr) {
        NodoGrupo* siguiente = primero->siguiente;
        delete primero;
        primero = siguiente;
    }

    lista.clear();
}

string ServicioMatricula::periodo() {
    return periodoActual;
}

void ServicioMatricula::cambiarPeriodo(string periodo) {
    periodoActual = periodo;
}

// Busquedas

NodoGrupo* ServicioMatricula::buscarNodo(string codigoCurso, int numero) {
    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        Grupo& grupo = actual->grupo;

        if (grupo.codigoCurso == codigoCurso && grupo.numero == numero && grupo.periodo == periodoActual) {
            return actual;
        }
    }

    return nullptr;
}

Grupo* ServicioMatricula::buscarGrupo(string codigoCurso, int numero) {
    NodoGrupo* nodo = buscarNodo(codigoCurso, numero);

    if (nodo == nullptr) {
        return nullptr;
    }

    return &nodo->grupo;
}

// Posicion de la matricula activa del estudiante en ese curso, -1 si no hay
int ServicioMatricula::buscarMatriculaActiva(string carne, string codigoCurso) {
    for (int i = 0; i < (int)lista.size(); i++) {
        if (lista[i].activa && lista[i].carne == carne && lista[i].codigoCurso == codigoCurso
            && lista[i].periodo == periodoActual) {
            return i;
        }
    }

    return -1;
}

// Carga de datos

bool ServicioMatricula::cargarGrupo(Grupo grupo) {
    if (!plan.existe(grupo.codigoCurso) || grupo.periodo != periodoActual) {
        return false;
    }

    if (buscarNodo(grupo.codigoCurso, grupo.numero) != nullptr) {
        return false;
    }

    NodoGrupo* nuevo = new NodoGrupo;

    nuevo->grupo = grupo;
    nuevo->grupo.matriculados = 0;
    nuevo->siguiente = nullptr;

    // Al final, para conservar el orden de registro
    if (primero == nullptr) {
        primero = nuevo;
    } else {
        NodoGrupo* actual = primero;

        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }

        actual->siguiente = nuevo;
    }

    return true;
}

bool ServicioMatricula::cargarMatricula(Matricula matricula) {
    if (matricula.periodo != periodoActual || !estudiantes.existe(matricula.carne)) {
        return false;
    }

    NodoGrupo* nodo = buscarNodo(matricula.codigoCurso, matricula.numeroGrupo);

    if (nodo == nullptr) {
        return false;
    }

    if (matricula.activa) {
        if (!tieneCupo(nodo->grupo) || buscarMatriculaActiva(matricula.carne, matricula.codigoCurso) >= 0) {
            return false;
        }

        nodo->grupo.matriculados++;
    }

    lista.push_back(matricula);
    return true;
}

bool ServicioMatricula::cargarEspera(string codigoCurso, int numero, string periodo, string carne) {
    if (periodo != periodoActual || !estudiantes.existe(carne)) {
        return false;
    }

    NodoGrupo* nodo = buscarNodo(codigoCurso, numero);

    return nodo != nullptr && nodo->espera.encolar(carne);
}

// Reglas de matricula (RN-01 a RN-04)

ResultadoMatricula ServicioMatricula::validar(string carne, NodoGrupo* nodo, bool revisarCita, long long horaActual,
                                              vector<string>& faltan, string& choqueCon) {
    Estudiante* estudiante = estudiantes.buscar(carne);

    if (estudiante == nullptr) {
        return SIN_ESTUDIANTE;
    }

    if (nodo == nullptr) {
        return SIN_GRUPO;
    }

    // RN-01: la cita ya debe estar habilitada
    if (revisarCita) {
        if (estudiante->cita == 0) {
            return SIN_CITA;
        }

        if (!citas.habilitada(carne, horaActual)) {
            return CITA_NO_HABILITADA;
        }
    }

    string curso = nodo->grupo.codigoCurso;

    // RN-03: no se repite un curso aprobado ni se matriculan dos grupos del mismo curso
    if (historial.aprobo(carne, curso)) {
        return YA_APROBADO;
    }

    if (buscarMatriculaActiva(carne, curso) >= 0) {
        return YA_MATRICULADO;
    }

    // RN-02: todos los requisitos aprobados (grafo + historial)
    vector<string> aprobados;
    historial.aprobados(carne, aprobados);
    plan.requisitosFaltantes(curso, aprobados, faltan);

    if (faltan.size() > 0) {
        return FALTAN_REQUISITOS;
    }

    // RN-04: sin choque de horario con lo que ya tiene matriculado
    for (int i = 0; i < (int)lista.size(); i++) {
        if (!lista[i].activa || lista[i].carne != carne || lista[i].periodo != periodoActual) {
            continue;
        }

        NodoGrupo* otro = buscarNodo(lista[i].codigoCurso, lista[i].numeroGrupo);

        if (otro != nullptr && hayChoque(otro->grupo, nodo->grupo)) {
            choqueCon = otro->grupo.codigoCurso + " grupo " + to_string(otro->grupo.numero);
            return CHOQUE_HORARIO;
        }
    }

    return MATRICULADO;
}

void ServicioMatricula::explicar(ResultadoMatricula resultado, string carne, NodoGrupo* nodo,
                                 vector<string>& faltan, string choqueCon) {
    switch (resultado) {
        case SIN_ESTUDIANTE:
            cout << "  No existe un estudiante con el carne " << carne << ".\n";
            break;

        case SIN_GRUPO:
            cout << "  No existe ese grupo en el periodo " << periodoActual << ".\n";
            break;

        case SIN_CITA:
            cout << "  " << estudiantes.nombreDe(carne) << " no tiene cita de matricula.\n";
            break;

        case CITA_NO_HABILITADA:
            cout << "  Su cita todavia no esta habilitada: es el ";
            imprimirFechaHora(estudiantes.buscar(carne)->cita);
            cout << ".\n";
            break;

        case YA_APROBADO:
            cout << "  Ya aprobo " << nodo->grupo.codigoCurso << ".\n";
            break;

        case YA_MATRICULADO:
            cout << "  Ya tiene matriculado un grupo de " << nodo->grupo.codigoCurso << ".\n";
            break;

        case FALTAN_REQUISITOS:
            cout << "  Le faltan requisitos de " << nodo->grupo.codigoCurso << ": ";

            for (int i = 0; i < (int)faltan.size(); i++) {
                cout << faltan[i] << (i < (int)faltan.size() - 1 ? ", " : "");
            }

            cout << ".\n";
            break;

        case CHOQUE_HORARIO:
            cout << "  Choque de horario con " << choqueCon << ".\n";
            break;

        case GRUPO_LLENO:
            cout << "  El grupo esta lleno.\n";
            break;

        case MATRICULADO:
            break;
    }
}

// Deja la matricula activa y saca al estudiante de las colas de ese curso
void ServicioMatricula::registrar(string carne, NodoGrupo* nodo) {
    Matricula matricula;

    matricula.carne = carne;
    matricula.codigoCurso = nodo->grupo.codigoCurso;
    matricula.numeroGrupo = nodo->grupo.numero;
    matricula.periodo = periodoActual;
    matricula.activa = true;

    lista.push_back(matricula);
    nodo->grupo.matriculados++;

    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        if (actual->grupo.codigoCurso == matricula.codigoCurso) {
            actual->espera.sacar(carne);
        }
    }
}

// Matricular (CU-04)

ResultadoMatricula ServicioMatricula::matricular(string carne, string codigoCurso, int numero, long long horaActual) {
    NodoGrupo* nodo = buscarNodo(codigoCurso, numero);
    vector<string> faltan;
    string choqueCon;

    ResultadoMatricula resultado = validar(carne, nodo, true, horaActual, faltan, choqueCon);

    if (resultado != MATRICULADO) {
        explicar(resultado, carne, nodo, faltan, choqueCon);
        return resultado;
    }

    // RN-05: si no hay cupo se puede pasar a la lista de espera
    if (!tieneCupo(nodo->grupo)) {
        cout << "  El grupo esta lleno (" << nodo->grupo.matriculados << " de " << nodo->grupo.cupo << ").\n";
        return GRUPO_LLENO;
    }

    registrar(carne, nodo);

    cout << "  Matricula lista: " << estudiantes.nombreDe(carne) << " en " << codigoCurso
         << " grupo " << numero << " (";
    imprimirHorario(nodo->grupo);
    cout << "). Cupo " << nodo->grupo.matriculados << " de " << nodo->grupo.cupo << ".\n";

    return MATRICULADO;
}

// Lista de espera (CU-07, RN-06)

bool ServicioMatricula::unirseAEspera(string carne, string codigoCurso, int numero, long long horaActual) {
    NodoGrupo* nodo = buscarNodo(codigoCurso, numero);
    vector<string> faltan;
    string choqueCon;

    ResultadoMatricula resultado = validar(carne, nodo, true, horaActual, faltan, choqueCon);

    if (resultado != MATRICULADO) {
        explicar(resultado, carne, nodo, faltan, choqueCon);
        return false;
    }

    if (tieneCupo(nodo->grupo)) {
        cout << "  El grupo tiene cupo: puede matricular directamente.\n";
        return false;
    }

    if (!nodo->espera.encolar(carne)) {
        cout << "  Ya esta en la lista de espera de ese grupo.\n";
        return false;
    }

    cout << "  " << estudiantes.nombreDe(carne) << " entra a la lista de espera de " << codigoCurso
         << " grupo " << numero << " en la posicion " << nodo->espera.posicionDe(carne) << ".\n";
    cout << "  Cola: ";
    nodo->espera.mostrar();
    cout << "\n";

    return true;
}

bool ServicioMatricula::salirDeEspera(string carne, string codigoCurso, int numero) {
    NodoGrupo* nodo = buscarNodo(codigoCurso, numero);

    if (nodo == nullptr) {
        cout << "  No existe ese grupo en el periodo " << periodoActual << ".\n";
        return false;
    }

    if (!nodo->espera.sacar(carne)) {
        cout << "  " << carne << " no esta en la lista de espera de ese grupo.\n";
        return false;
    }

    cout << "  Sale de la lista de espera. Cola: ";
    nodo->espera.mostrar();
    cout << "\n";

    return true;
}

// Retirar (CU-08) y asignar el cupo liberado (CU-09)

bool ServicioMatricula::retirar(string carne, string codigoCurso) {
    int posicion = buscarMatriculaActiva(carne, codigoCurso);

    if (posicion < 0) {
        cout << "  " << carne << " no tiene matriculado " << codigoCurso << " en " << periodoActual << ".\n";
        return false;
    }

    NodoGrupo* nodo = buscarNodo(codigoCurso, lista[posicion].numeroGrupo);

    lista[posicion].activa = false;
    nodo->grupo.matriculados--;

    cout << "  " << estudiantes.nombreDe(carne) << " retira " << codigoCurso << " grupo " << nodo->grupo.numero
         << ". Cupo " << nodo->grupo.matriculados << " de " << nodo->grupo.cupo << ".\n";

    asignarCupoLiberado(nodo);
    return true;
}

// RN-07: el cupo pasa al primero de la cola que siga cumpliendo las reglas;
// los que ya no cumplen salen de la cola
void ServicioMatricula::asignarCupoLiberado(NodoGrupo* nodo) {
    while (tieneCupo(nodo->grupo) && !nodo->espera.esVacia()) {
        string carne;
        nodo->espera.desencolar(carne);

        vector<string> faltan;
        string choqueCon;
        ResultadoMatricula resultado = validar(carne, nodo, false, 0, faltan, choqueCon);

        if (resultado == MATRICULADO) {
            registrar(carne, nodo);

            cout << "  El cupo pasa al primero de la lista de espera: " << carne << " "
                 << estudiantes.nombreDe(carne) << " (matricula lista).\n";
            continue;
        }

        cout << "  " << carne << " sale de la lista de espera porque ya no cumple:\n";
        explicar(resultado, carne, nodo, faltan, choqueCon);
    }

    if (nodo->espera.esVacia()) {
        cout << "  Lista de espera de " << nodo->grupo.codigoCurso << " grupo " << nodo->grupo.numero << ": vacia.\n";
    }
}

// Grupos (CU-14)

bool ServicioMatricula::abrirGrupo(Grupo grupo) {
    grupo.periodo = periodoActual;
    grupo.matriculados = 0;

    if (!plan.existe(grupo.codigoCurso)) {
        cout << "  No existe el curso " << grupo.codigoCurso << ".\n";
        return false;
    }

    if (grupo.numero <= 0 || grupo.cupo <= 0) {
        cout << "  El numero de grupo y el cupo deben ser mayores que cero.\n";
        return false;
    }

    if (grupo.dia < 1 || grupo.dia > 7 || grupo.horaInicio >= grupo.horaFin) {
        cout << "  El dia o el horario no es valido.\n";
        return false;
    }

    if (!cargarGrupo(grupo)) {
        cout << "  Ya existe el grupo " << grupo.numero << " de " << grupo.codigoCurso << ".\n";
        return false;
    }

    cout << "  Grupo abierto: " << grupo.codigoCurso << " grupo " << grupo.numero << " (";
    imprimirHorario(grupo);
    cout << "), cupo " << grupo.cupo << ".\n";

    return true;
}

bool ServicioMatricula::modificarCupo(string codigoCurso, int numero, int cupo) {
    NodoGrupo* nodo = buscarNodo(codigoCurso, numero);

    if (nodo == nullptr) {
        cout << "  No existe ese grupo en el periodo " << periodoActual << ".\n";
        return false;
    }

    if (cupo < nodo->grupo.matriculados || cupo <= 0) {
        cout << "  El cupo no puede ser menor que los " << nodo->grupo.matriculados << " matriculados.\n";
        return false;
    }

    nodo->grupo.cupo = cupo;
    cout << "  Cupo actualizado: " << nodo->grupo.matriculados << " de " << cupo << ".\n";

    // Si se agregaron campos, la lista de espera avanza
    asignarCupoLiberado(nodo);
    return true;
}

bool ServicioMatricula::cerrarGrupo(string codigoCurso, int numero) {
    NodoGrupo* anterior = nullptr;
    NodoGrupo* actual = primero;

    while (actual != nullptr && !(actual->grupo.codigoCurso == codigoCurso && actual->grupo.numero == numero
                                  && actual->grupo.periodo == periodoActual)) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == nullptr) {
        cout << "  No existe ese grupo en el periodo " << periodoActual << ".\n";
        return false;
    }

    if (actual->grupo.matriculados > 0 || !actual->espera.esVacia()) {
        cout << "  No se puede cerrar: tiene estudiantes matriculados o en espera.\n";
        return false;
    }

    if (anterior == nullptr) {
        primero = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    delete actual;
    cout << "  Grupo cerrado.\n";

    return true;
}

// Vistas

void ServicioMatricula::imprimirGrupo(Grupo& grupo) {
    cout << "  ";
    columna(grupo.codigoCurso, 9);
    columna(to_string(grupo.numero), 4);

    columna(nombreDeDia(grupo.dia), 10);
    imprimirHora(grupo.horaInicio);
    cout << "-";
    imprimirHora(grupo.horaFin);
    cout << "   ";
    columna(to_string(grupo.matriculados) + "/" + to_string(grupo.cupo), 7);
}

void ServicioMatricula::mostrarGrupos() {
    if (primero == nullptr) {
        cout << "  No hay grupos abiertos.\n";
        return;
    }

    cout << "  Grupos del periodo " << periodoActual << "\n\n";
    cout << "  Curso    Grp Dia       Horario       Cupo   Espera  Curso\n";
    cout << "  ----------------------------------------------------------------\n";

    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        imprimirGrupo(actual->grupo);
        columna(to_string(actual->espera.cantidad()), 8);
        cout << plan.nombreDe(actual->grupo.codigoCurso) << "\n";
    }
}

void ServicioMatricula::mostrarListasEspera() {
    bool alguna = false;

    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        if (actual->espera.esVacia()) {
            continue;
        }

        alguna = true;
        cout << "  " << actual->grupo.codigoCurso << " grupo " << actual->grupo.numero << ": ";
        actual->espera.mostrar();
        cout << "\n";
    }

    if (!alguna) {
        cout << "  No hay estudiantes en listas de espera.\n";
    }
}

void ServicioMatricula::mostrarMatriculaDe(string carne) {
    if (!estudiantes.existe(carne)) {
        cout << "  No existe un estudiante con el carne " << carne << ".\n";
        return;
    }

    cout << "  " << estudiantes.nombreDe(carne) << " (" << carne << ") - periodo " << periodoActual << "\n\n";

    int cursos = 0;
    int creditos = 0;

    for (int i = 0; i < (int)lista.size(); i++) {
        Matricula& matricula = lista[i];

        if (!matricula.activa || matricula.carne != carne || matricula.periodo != periodoActual) {
            continue;
        }

        NodoGrupo* nodo = buscarNodo(matricula.codigoCurso, matricula.numeroGrupo);

        cout << "  " << matricula.codigoCurso << " grupo " << matricula.numeroGrupo << "  ";

        if (nodo != nullptr) {
            imprimirHorario(nodo->grupo);
        }

        cout << "  " << plan.nombreDe(matricula.codigoCurso) << "\n";

        Curso* curso = plan.buscar(matricula.codigoCurso);

        if (curso != nullptr) {
            creditos += curso->creditos;
        }

        cursos++;
    }

    if (cursos == 0) {
        cout << "  No tiene cursos matriculados.\n";
    } else {
        cout << "\n  Cursos: " << cursos << ", creditos: " << creditos << "\n";
    }

    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        int posicion = actual->espera.posicionDe(carne);

        if (posicion > 0) {
            cout << "  En espera de " << actual->grupo.codigoCurso << " grupo " << actual->grupo.numero
                 << ": posicion " << posicion << " de " << actual->espera.cantidad() << "\n";
        }
    }
}

// Consultas para los demas componentes

bool ServicioMatricula::tieneMatriculasActivas(string carne) {
    for (int i = 0; i < (int)lista.size(); i++) {
        if (lista[i].activa && lista[i].carne == carne) {
            return true;
        }
    }

    return false;
}

bool ServicioMatricula::hayGruposDelCurso(string codigoCurso) {
    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        if (actual->grupo.codigoCurso == codigoCurso) {
            return true;
        }
    }

    return false;
}

void ServicioMatricula::sacarDeTodasLasColas(string carne) {
    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        actual->espera.sacar(carne);
    }
}

void ServicioMatricula::grupos(vector<Grupo>& salida) {
    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        salida.push_back(actual->grupo);
    }
}

void ServicioMatricula::matriculas(vector<Matricula>& salida) {
    for (int i = 0; i < (int)lista.size(); i++) {
        salida.push_back(lista[i]);
    }
}

void ServicioMatricula::esperas(vector<string>& cursos, vector<int>& numeros, vector<string>& carnes) {
    for (NodoGrupo* actual = primero; actual != nullptr; actual = actual->siguiente) {
        vector<string> enCola;
        actual->espera.listar(enCola);

        for (int i = 0; i < (int)enCola.size(); i++) {
            cursos.push_back(actual->grupo.codigoCurso);
            numeros.push_back(actual->grupo.numero);
            carnes.push_back(enCola[i]);
        }
    }
}
