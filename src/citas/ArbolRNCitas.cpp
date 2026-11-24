// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del arbol rojo-negro de citas

#include <iostream>
#include "citas/ArbolRNCitas.h"

using namespace std;

// Constructor y destructor

ArbolRNCitas::ArbolRNCitas() {
    raiz = nullptr;
    reiniciarCambios();
}

ArbolRNCitas::~ArbolRNCitas() {
    liberar(raiz);
}

void ArbolRNCitas::liberar(NodoRN* nodo) {
    if (nodo == nullptr) {
        return;
    }

    liberar(nodo->izquierdo);
    liberar(nodo->derecho);

    delete nodo;
}

void ArbolRNCitas::vaciar() {
    liberar(raiz);
    raiz = nullptr;
}

NodoRN* ArbolRNCitas::crearNodo(CitaMatricula cita) {
    NodoRN* nuevoNodo = new NodoRN;

    nuevoNodo->cita = cita;
    nuevoNodo->rojo = true; // Nuevo nodo es rojo

    nuevoNodo->izquierdo = nullptr;
    nuevoNodo->derecho = nullptr;
    nuevoNodo->padre = nullptr;

    return nuevoNodo;
}

// Colores

// Los nulos cuentan como negros
bool ArbolRNCitas::esRojo(NodoRN* nodo) {
    return nodo != nullptr && nodo->rojo;
}

// Cambiar el color
void ArbolRNCitas::pintar(NodoRN* nodo, bool rojo) {
    if (nodo == nullptr) {
        return;
    }

    if (nodo->rojo != rojo) {
        nodo->rojo = rojo;
        recoloraciones++;
    }
}

void ArbolRNCitas::reiniciarCambios() {
    rotacionesIzquierda = 0;
    rotacionesDerecha = 0;
    recoloraciones = 0;
}

// Rotaciones

void ArbolRNCitas::rotarIzquierda(NodoRN* nodo) {
    NodoRN* derecho = nodo->derecho;

    rotacionesIzquierda++;

    nodo->derecho = derecho->izquierdo;

    if (derecho->izquierdo != nullptr) {
        derecho->izquierdo->padre = nodo;
    }

    derecho->padre = nodo->padre;

    if (nodo->padre == nullptr) {
        raiz = derecho;
    } else if (nodo == nodo->padre->izquierdo) {
        nodo->padre->izquierdo = derecho;
    } else {
        nodo->padre->derecho = derecho;
    }

    derecho->izquierdo = nodo;
    nodo->padre = derecho;
}

void ArbolRNCitas::rotarDerecha(NodoRN* nodo) {
    NodoRN* izquierdo = nodo->izquierdo;

    rotacionesDerecha++;

    nodo->izquierdo = izquierdo->derecho;

    if (izquierdo->derecho != nullptr) {
        izquierdo->derecho->padre = nodo;
    }

    izquierdo->padre = nodo->padre;

    if (nodo->padre == nullptr) {
        raiz = izquierdo;
    } else if (nodo == nodo->padre->izquierdo) {
        nodo->padre->izquierdo = izquierdo;
    } else {
        nodo->padre->derecho = izquierdo;
    }

    izquierdo->derecho = nodo;
    nodo->padre = izquierdo;
}

// Insertar, false si ya hay una cita a esa hora

bool ArbolRNCitas::insertar(CitaMatricula cita) {
    reiniciarCambios();

    NodoRN* actual = raiz;
    NodoRN* padre = nullptr;

    // Buscar donde va el nuevo nodo
    while (actual != nullptr) {
        padre = actual;

        if (cita.fechaHora < actual->cita.fechaHora) {
            actual = actual->izquierdo;
        } else if (cita.fechaHora > actual->cita.fechaHora) {
            actual = actual->derecho;
        } else {
            return false;
        }
    }

    NodoRN* nuevoNodo = crearNodo(cita);
    nuevoNodo->padre = padre;

    if (padre == nullptr) {
        raiz = nuevoNodo;
    } else if (cita.fechaHora < padre->cita.fechaHora) {
        padre->izquierdo = nuevoNodo;
    } else {
        padre->derecho = nuevoNodo;
    }

    corregirInsercion(nuevoNodo);
    return true;
}

// Corregir despues de insertar

void ArbolRNCitas::corregirInsercion(NodoRN* nodo) {
    while (nodo != raiz && esRojo(nodo->padre)) {
        NodoRN* padre = nodo->padre;
        NodoRN* abuelo = padre->padre;
        bool padreIzquierdo = (padre == abuelo->izquierdo);
        NodoRN* tio;

        if (padreIzquierdo) {
            tio = abuelo->derecho;
        } else {
            tio = abuelo->izquierdo;
        }

        // Caso 1: tio rojo
        if (esRojo(tio)) {
            pintar(padre, false);
            pintar(tio, false);
            pintar(abuelo, true);

            nodo = abuelo;

        } else {
            // Caso 2: triangulo
            if (padreIzquierdo && nodo == padre->derecho) {
                rotarIzquierda(padre);
                nodo = padre;
                padre = nodo->padre;
            } else if (!padreIzquierdo && nodo == padre->izquierdo) {
                rotarDerecha(padre);
                nodo = padre;
                padre = nodo->padre;
            }

            // Caso 3: linea, se rota el abuelo
            if (padreIzquierdo) {
                rotarDerecha(abuelo);
            } else {
                rotarIzquierda(abuelo);
            }

            pintar(padre, false);
            pintar(abuelo, true);
        }
    }

    // La raiz siempre es negra
    pintar(raiz, false);
}

// Buscar

NodoRN* ArbolRNCitas::buscarNodo(long long fechaHora) {
    NodoRN* actual = raiz;

    while (actual != nullptr) {
        if (fechaHora == actual->cita.fechaHora) {
            return actual;
        }

        if (fechaHora < actual->cita.fechaHora) {
            actual = actual->izquierdo;
        } else {
            actual = actual->derecho;
        }
    }

    return nullptr;
}

// Buscar por fecha y hora, nullptr si no existe
CitaMatricula* ArbolRNCitas::buscar(long long fechaHora) {
    NodoRN* nodo = buscarNodo(fechaHora);

    if (nodo == nullptr) {
        return nullptr;
    }

    return &nodo->cita;
}

// Buscar por carne recorriendo el arbol
CitaMatricula* ArbolRNCitas::buscarPorCarne(string carne) {
    NodoRN* nodo = buscarPorCarne(raiz, carne);

    if (nodo == nullptr) {
        return nullptr;
    }

    return &nodo->cita;
}

NodoRN* ArbolRNCitas::buscarPorCarne(NodoRN* nodo, string carne) {
    if (nodo == nullptr) {
        return nullptr;
    }

    if (nodo->cita.carne == carne) {
        return nodo;
    }

    NodoRN* encontrado = buscarPorCarne(nodo->izquierdo, carne);

    if (encontrado != nullptr) {
        return encontrado;
    }

    return buscarPorCarne(nodo->derecho, carne);
}

// Muestra los nodos que se comparan al buscar
void ArbolRNCitas::mostrarCamino(long long fechaHora) {
    NodoRN* actual = raiz;
    int comparaciones = 0;

    cout << "  Camino: ";

    while (actual != nullptr && actual->cita.fechaHora != fechaHora) {
        imprimirNodo(actual);
        cout << " -> ";
        comparaciones++;

        if (fechaHora < actual->cita.fechaHora) {
            actual = actual->izquierdo;
        } else {
            actual = actual->derecho;
        }
    }

    if (actual != nullptr) {
        imprimirNodo(actual);
        comparaciones++;
    } else {
        cout << "nulo";
    }

    cout << "\n  Comparaciones: " << comparaciones << "\n";
}

// Eliminar

NodoRN* ArbolRNCitas::minimo(NodoRN* nodo) {
    while (nodo->izquierdo != nullptr) {
        nodo = nodo->izquierdo;
    }

    return nodo;
}

// Pone el subarbol nuevo en el lugar del viejo
void ArbolRNCitas::reemplazar(NodoRN* viejo, NodoRN* nuevo) {
    if (viejo->padre == nullptr) {
        raiz = nuevo;
    } else if (viejo == viejo->padre->izquierdo) {
        viejo->padre->izquierdo = nuevo;
    } else {
        viejo->padre->derecho = nuevo;
    }

    if (nuevo != nullptr) {
        nuevo->padre = viejo->padre;
    }
}

// Reprogramar: sacar y volver a insertar
bool ArbolRNCitas::reprogramar(long long fechaHora, long long nuevaFechaHora) {
    CitaMatricula* encontrada = buscar(fechaHora);

    if (encontrada == nullptr || buscar(nuevaFechaHora) != nullptr) {
        return false;
    }

    CitaMatricula cita = *encontrada;

    eliminar(fechaHora);

    int izquierdasAlBorrar = rotacionesIzquierda;
    int derechasAlBorrar = rotacionesDerecha;
    int coloresAlBorrar = recoloraciones;

    cita.fechaHora = nuevaFechaHora;
    insertar(cita);

    rotacionesIzquierda += izquierdasAlBorrar;
    rotacionesDerecha += derechasAlBorrar;
    recoloraciones += coloresAlBorrar;

    return true;
}

// Eliminar por fecha y hora
bool ArbolRNCitas::eliminar(long long fechaHora) {
    reiniciarCambios();

    NodoRN* nodo = buscarNodo(fechaHora);

    if (nodo == nullptr) {
        return false;
    }

    // Nodo que sube al hueco y su padre
    NodoRN* hijo;
    NodoRN* padreHijo;
    bool seQuitoNegro = !nodo->rojo;

    if (nodo->izquierdo == nullptr) {
        // Sin hijos o solo hijo derecho
        hijo = nodo->derecho;
        padreHijo = nodo->padre;
        reemplazar(nodo, nodo->derecho);

    } else if (nodo->derecho == nullptr) {
        // Solo hijo izquierdo
        hijo = nodo->izquierdo;
        padreHijo = nodo->padre;
        reemplazar(nodo, nodo->izquierdo);

    } else {
        // Dos hijos: sube el sucesor
        NodoRN* sucesor = minimo(nodo->derecho);

        seQuitoNegro = !sucesor->rojo;
        hijo = sucesor->derecho;

        if (sucesor->padre == nodo) {
            padreHijo = sucesor;
        } else {
            padreHijo = sucesor->padre;
            reemplazar(sucesor, sucesor->derecho);

            sucesor->derecho = nodo->derecho;
            sucesor->derecho->padre = sucesor;
        }

        reemplazar(nodo, sucesor);

        sucesor->izquierdo = nodo->izquierdo;
        sucesor->izquierdo->padre = sucesor;
        sucesor->rojo = nodo->rojo;
    }

    delete nodo;

    // Corregir si salio un negro
    if (seQuitoNegro) {
        corregirEliminacion(hijo, padreHijo);
    }

    return true;
}

// Corregir despues de eliminar

void ArbolRNCitas::corregirEliminacion(NodoRN* nodo, NodoRN* padre) {
    while (nodo != raiz && !esRojo(nodo)) {

        if (nodo == padre->izquierdo) {
            NodoRN* hermano = padre->derecho;

            // Caso 1: hermano rojo
            if (esRojo(hermano)) {
                pintar(hermano, false);
                pintar(padre, true);
                rotarIzquierda(padre);
                hermano = padre->derecho;
            }

            // Caso 2: hermano negro con hijos negros
            if (!esRojo(hermano->izquierdo) && !esRojo(hermano->derecho)) {
                pintar(hermano, true);
                nodo = padre;
                padre = nodo->padre;

            } else {
                // Caso 3: hijo cercano del hermano rojo
                if (!esRojo(hermano->derecho)) {
                    pintar(hermano->izquierdo, false);
                    pintar(hermano, true);
                    rotarDerecha(hermano);
                    hermano = padre->derecho;
                }

                // Caso 4: hijo lejano del hermano rojo
                pintar(hermano, padre->rojo);
                pintar(padre, false);
                pintar(hermano->derecho, false);
                rotarIzquierda(padre);
                nodo = raiz;
            }

        } else {
            NodoRN* hermano = padre->izquierdo;

            // Caso 1
            if (esRojo(hermano)) {
                pintar(hermano, false);
                pintar(padre, true);
                rotarDerecha(padre);
                hermano = padre->izquierdo;
            }

            // Caso 2
            if (!esRojo(hermano->izquierdo) && !esRojo(hermano->derecho)) {
                pintar(hermano, true);
                nodo = padre;
                padre = nodo->padre;

            } else {
                // Caso 3
                if (!esRojo(hermano->izquierdo)) {
                    pintar(hermano->derecho, false);
                    pintar(hermano, true);
                    rotarIzquierda(hermano);
                    hermano = padre->izquierdo;
                }

                // Caso 4
                pintar(hermano, padre->rojo);
                pintar(padre, false);
                pintar(hermano->izquierdo, false);
                rotarDerecha(padre);
                nodo = raiz;
            }
        }
    }

    // El nodo que sube queda negro
    pintar(nodo, false);
}

// Resumen de la ultima insercion o eliminacion

void ArbolRNCitas::resumenCambios() {
    cout << "  Rotaciones: izquierda " << rotacionesIzquierda
         << ", derecha " << rotacionesDerecha << "\n";
    cout << "  Recoloraciones: " << recoloraciones << "\n";
}

// Recorridos

void ArbolRNCitas::enOrden(vector<CitaMatricula>& salida) {
    enOrden(raiz, salida);
}

void ArbolRNCitas::enOrden(NodoRN* nodo, vector<CitaMatricula>& salida) {
    if (nodo == nullptr) {
        return;
    }

    enOrden(nodo->izquierdo, salida);
    salida.push_back(nodo->cita);
    enOrden(nodo->derecho, salida);
}

// Citas en un rango
void ArbolRNCitas::rango(long long desde, long long hasta, vector<CitaMatricula>& salida) {
    rango(raiz, desde, hasta, salida);
}

void ArbolRNCitas::rango(NodoRN* nodo, long long desde, long long hasta, vector<CitaMatricula>& salida) {
    if (nodo == nullptr) {
        return;
    }

    // Bajar solo donde puede haber citas del rango
    if (desde < nodo->cita.fechaHora) {
        rango(nodo->izquierdo, desde, hasta, salida);
    }

    if (desde <= nodo->cita.fechaHora && nodo->cita.fechaHora <= hasta) {
        salida.push_back(nodo->cita);
    }

    if (nodo->cita.fechaHora < hasta) {
        rango(nodo->derecho, desde, hasta, salida);
    }
}

// Mostrar el arbol

void ArbolRNCitas::imprimirNodo(NodoRN* nodo) {
    imprimirDiaHora(nodo->cita.fechaHora);

    if (nodo->rojo) {
        cout << " (R)";
    } else {
        cout << " (N)";
    }
}

void ArbolRNCitas::mostrarArbol() {
    if (raiz == nullptr) {
        cout << "  El arbol esta vacio.\n";
        return;
    }

    cout << "  (raiz a la izquierda, hijo derecho arriba, hijo izquierdo abajo)\n\n";
    mostrarNodo(raiz, 0);
}

void ArbolRNCitas::mostrarNodo(NodoRN* nodo, int nivel) {
    if (nodo == nullptr) {
        return;
    }

    mostrarNodo(nodo->derecho, nivel + 1);

    cout << "  ";

    for (int i = 0; i < nivel; i++) {
        cout << "                  ";
    }

    imprimirNodo(nodo);
    cout << "\n";

    mostrarNodo(nodo->izquierdo, nivel + 1);
}

// Propiedades del arbol

bool ArbolRNCitas::propiedadesCorrectas() {
    if (raiz == nullptr) {
        return true;
    }

    if (raiz->rojo || raiz->padre != nullptr) {
        return false;
    }

    return sinRojoConHijoRojo(raiz) && alturaNegra(raiz) != -1
        && ordenCorrecto() && padresCorrectos(raiz);
}

void ArbolRNCitas::verificarPropiedades() {
    if (raiz == nullptr) {
        cout << "  El arbol esta vacio.\n";
        return;
    }

    cout << "  Propiedades:\n";

    cout << "    La raiz es negra: ";
    if (!raiz->rojo) {
        cout << "si\n";
    } else {
        cout << "no\n";
    }

    cout << "    Ningun rojo tiene hijo rojo: ";
    if (sinRojoConHijoRojo(raiz)) {
        cout << "si\n";
    } else {
        cout << "no\n";
    }

    int negros = alturaNegra(raiz);

    cout << "    Mismos negros en todos los caminos: ";
    if (negros != -1) {
        cout << "si (negros por camino: " << negros << ")\n";
    } else {
        cout << "no\n";
    }

    cout << "    Citas ordenadas por fecha y hora: ";
    if (ordenCorrecto()) {
        cout << "si\n";
    } else {
        cout << "no\n";
    }

    cout << "    Nodos: " << contarNodos() << ", altura: " << altura() << "\n";
}

bool ArbolRNCitas::sinRojoConHijoRojo(NodoRN* nodo) {
    if (nodo == nullptr) {
        return true;
    }

    if (nodo->rojo && (esRojo(nodo->izquierdo) || esRojo(nodo->derecho))) {
        return false;
    }

    return sinRojoConHijoRojo(nodo->izquierdo) && sinRojoConHijoRojo(nodo->derecho);
}

// Negros por camino, -1 si no coinciden
int ArbolRNCitas::alturaNegra(NodoRN* nodo) {
    if (nodo == nullptr) {
        return 0;
    }

    int izquierda = alturaNegra(nodo->izquierdo);
    int derecha = alturaNegra(nodo->derecho);

    if (izquierda == -1 || derecha == -1 || izquierda != derecha) {
        return -1;
    }

    if (nodo->rojo) {
        return izquierda;
    }

    return izquierda + 1;
}

// Recorrido en orden
bool ArbolRNCitas::ordenCorrecto() {
    vector<CitaMatricula> citas;
    enOrden(citas);

    for (int i = 1; i < (int)citas.size(); i++) {
        if (citas[i - 1].fechaHora >= citas[i].fechaHora) {
            return false;
        }
    }

    return true;
}

// Cada hijo tiene que apuntar a su padre
bool ArbolRNCitas::padresCorrectos(NodoRN* nodo) {
    if (nodo == nullptr) {
        return true;
    }

    if (nodo->izquierdo != nullptr && nodo->izquierdo->padre != nodo) {
        return false;
    }

    if (nodo->derecho != nullptr && nodo->derecho->padre != nodo) {
        return false;
    }

    return padresCorrectos(nodo->izquierdo) && padresCorrectos(nodo->derecho);
}

// Vacio, altura y cantidad de nodos

bool ArbolRNCitas::esVacio() {
    return raiz == nullptr;
}

int ArbolRNCitas::altura() {
    return altura(raiz);
}

int ArbolRNCitas::altura(NodoRN* nodo) {
    if (nodo == nullptr) {
        return 0;
    }

    int izquierda = altura(nodo->izquierdo);
    int derecha = altura(nodo->derecho);

    if (izquierda > derecha) {
        return izquierda + 1;
    }

    return derecha + 1;
}

int ArbolRNCitas::contarNodos() {
    return contarNodos(raiz);
}

int ArbolRNCitas::contarNodos(NodoRN* nodo) {
    if (nodo == nullptr) {
        return 0;
    }

    return 1 + contarNodos(nodo->izquierdo) + contarNodos(nodo->derecho);
}
