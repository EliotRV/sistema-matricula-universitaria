// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del arbol B+ del historial academico

#include <iostream>
#include "historial/ArbolBPlusHistorial.h"

using namespace std;

// Constructor y destructor

ArbolBPlusHistorial::ArbolBPlusHistorial() {
    raiz = nullptr;
    total = 0;
    reiniciarCambios();
}

ArbolBPlusHistorial::~ArbolBPlusHistorial() {
    liberar(raiz);
}

void ArbolBPlusHistorial::liberar(NodoBPlus* nodo) {
    if (nodo == nullptr) {
        return;
    }

    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->cantidad; i++) {
            liberar(nodo->hijos[i]);
        }
    }

    delete nodo;
}

void ArbolBPlusHistorial::vaciar() {
    liberar(raiz);
    raiz = nullptr;
    total = 0;
}

NodoBPlus* ArbolBPlusHistorial::crearNodo(bool esHoja) {
    NodoBPlus* nuevo = new NodoBPlus;

    nuevo->cantidad = 0;
    nuevo->esHoja = esHoja;
    nuevo->siguiente = nullptr;

    for (int i = 0; i < MAX_CLAVES_BPLUS + 2; i++) {
        nuevo->hijos[i] = nullptr;
    }

    return nuevo;
}

void ArbolBPlusHistorial::reiniciarCambios() {
    divisiones = 0;
    prestamos = 0;
    uniones = 0;
    cambiosDeAltura = 0;
}

// Busqueda

// Hijo por el que hay que bajar
int ArbolBPlusHistorial::posicionHijo(NodoBPlus* nodo, string clave) {
    int i = 0;

    while (i < nodo->cantidad && clave >= nodo->claves[i]) {
        i++;
    }

    return i;
}

NodoBPlus* ArbolBPlusHistorial::buscarHoja(string clave) {
    NodoBPlus* actual = raiz;

    if (actual == nullptr) {
        return nullptr;
    }

    while (!actual->esHoja) {
        actual = actual->hijos[posicionHijo(actual, clave)];
    }

    return actual;
}

NodoBPlus* ArbolBPlusHistorial::primeraHoja() {
    NodoBPlus* actual = raiz;

    if (actual == nullptr) {
        return nullptr;
    }

    while (!actual->esHoja) {
        actual = actual->hijos[0];
    }

    return actual;
}

RegistroNota* ArbolBPlusHistorial::buscar(string carne, string periodo, string codigoCurso) {
    string clave = armarClaveNota(carne, periodo, codigoCurso);
    NodoBPlus* hoja = buscarHoja(clave);

    if (hoja == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < hoja->cantidad; i++) {
        if (hoja->claves[i] == clave) {
            return &hoja->registros[i];
        }
    }

    return nullptr;
}

// Modificar la nota

bool ArbolBPlusHistorial::modificarNota(string carne, string periodo, string codigoCurso, int nota) {
    RegistroNota* registro = buscar(carne, periodo, codigoCurso);

    if (registro == nullptr) {
        return false;
    }

    registro->nota = nota;
    return true;
}

// Insercion

// Insertar ordenado en la hoja
void ArbolBPlusHistorial::insertarEnHoja(NodoBPlus* hoja, string clave, RegistroNota registro) {
    int i = hoja->cantidad - 1;

    while (i >= 0 && hoja->claves[i] > clave) {
        hoja->claves[i + 1] = hoja->claves[i];
        hoja->registros[i + 1] = hoja->registros[i];
        i--;
    }

    hoja->claves[i + 1] = clave;
    hoja->registros[i + 1] = registro;
    hoja->cantidad++;
}

// Dividir la hoja
NodoBPlus* ArbolBPlusHistorial::dividirHoja(NodoBPlus* hoja) {
    NodoBPlus* derecha = crearNodo(true);

    int mitad = hoja->cantidad / 2;

    for (int i = mitad; i < hoja->cantidad; i++) {
        derecha->claves[derecha->cantidad] = hoja->claves[i];
        derecha->registros[derecha->cantidad] = hoja->registros[i];
        derecha->cantidad++;
    }

    hoja->cantidad = mitad;

    derecha->siguiente = hoja->siguiente;
    hoja->siguiente = derecha;

    return derecha;
}

// Dividir el nodo interno
NodoBPlus* ArbolBPlusHistorial::dividirInterno(NodoBPlus* nodo, string& claveSube) {
    NodoBPlus* derecha = crearNodo(false);

    int mitad = nodo->cantidad / 2;
    claveSube = nodo->claves[mitad];

    for (int i = mitad + 1; i < nodo->cantidad; i++) {
        derecha->claves[derecha->cantidad] = nodo->claves[i];
        derecha->cantidad++;
    }

    int j = 0;

    for (int i = mitad + 1; i <= nodo->cantidad; i++) {
        derecha->hijos[j] = nodo->hijos[i];
        nodo->hijos[i] = nullptr;
        j++;
    }

    nodo->cantidad = mitad;

    return derecha;
}

// Insertar recursivo; true si el nodo se dividio
bool ArbolBPlusHistorial::insertarEn(NodoBPlus* nodo, string clave, RegistroNota registro, string& claveSube, NodoBPlus*& derecha) {
    // Caso trivial: estamos en una hoja
    if (nodo->esHoja) {
        insertarEnHoja(nodo, clave, registro);

        if (nodo->cantidad <= MAX_CLAVES_BPLUS) {
            return false;
        }

        // Sube una copia de la primera clave derecha
        derecha = dividirHoja(nodo);
        claveSube = derecha->claves[0];
        divisiones++;

        return true;
    }

    int posicion = posicionHijo(nodo, clave);

    string nuevaClave;
    NodoBPlus* nuevoHijo = nullptr;

    if (!insertarEn(nodo->hijos[posicion], clave, registro, nuevaClave, nuevoHijo)) {
        return false;
    }

    // Agregar la clave y el hijo nuevo
    for (int i = nodo->cantidad; i > posicion; i--) {
        nodo->claves[i] = nodo->claves[i - 1];
    }

    for (int i = nodo->cantidad + 1; i > posicion + 1; i--) {
        nodo->hijos[i] = nodo->hijos[i - 1];
    }

    nodo->claves[posicion] = nuevaClave;
    nodo->hijos[posicion + 1] = nuevoHijo;
    nodo->cantidad++;

    if (nodo->cantidad <= MAX_CLAVES_BPLUS) {
        return false;
    }

    derecha = dividirInterno(nodo, claveSube);
    divisiones++;

    return true;
}

bool ArbolBPlusHistorial::insertar(RegistroNota registro) {
    reiniciarCambios();

    string clave = claveDe(registro);

    if (raiz == nullptr) {
        raiz = crearNodo(true);
        insertarEnHoja(raiz, clave, registro);
        total++;
        cambiosDeAltura++;
        return true;
    }

    if (buscar(registro.carne, registro.periodo, registro.codigoCurso) != nullptr) {
        return false;
    }

    string claveSube;
    NodoBPlus* derecha = nullptr;

    // Si la raiz se divide, el arbol crece un nivel
    if (insertarEn(raiz, clave, registro, claveSube, derecha)) {
        NodoBPlus* nuevaRaiz = crearNodo(false);

        nuevaRaiz->claves[0] = claveSube;
        nuevaRaiz->cantidad = 1;
        nuevaRaiz->hijos[0] = raiz;
        nuevaRaiz->hijos[1] = derecha;

        raiz = nuevaRaiz;
        cambiosDeAltura++;
    }

    total++;
    return true;
}

// Eliminacion

// Quitar una clave del padre
void ArbolBPlusHistorial::quitarDelPadre(NodoBPlus* padre, int posicionClave, int posicionHijo) {
    for (int i = posicionClave; i < padre->cantidad - 1; i++) {
        padre->claves[i] = padre->claves[i + 1];
    }

    for (int i = posicionHijo; i < padre->cantidad; i++) {
        padre->hijos[i] = padre->hijos[i + 1];
    }

    padre->hijos[padre->cantidad] = nullptr;
    padre->cantidad--;
}

// Corregir un hijo con menos claves del minimo
void ArbolBPlusHistorial::corregirHijo(NodoBPlus* padre, int posicion) {
    NodoBPlus* hijo = padre->hijos[posicion];
    NodoBPlus* izquierdo = nullptr;
    NodoBPlus* derecho = nullptr;

    if (posicion > 0) {
        izquierdo = padre->hijos[posicion - 1];
    }

    if (posicion < padre->cantidad) {
        derecho = padre->hijos[posicion + 1];
    }

    if (hijo->esHoja) {
        // Prestamo del hermano izquierdo
        if (izquierdo != nullptr && izquierdo->cantidad > MIN_CLAVES_HOJA) {
            for (int i = hijo->cantidad; i > 0; i--) {
                hijo->claves[i] = hijo->claves[i - 1];
                hijo->registros[i] = hijo->registros[i - 1];
            }

            hijo->claves[0] = izquierdo->claves[izquierdo->cantidad - 1];
            hijo->registros[0] = izquierdo->registros[izquierdo->cantidad - 1];
            hijo->cantidad++;
            izquierdo->cantidad--;

            padre->claves[posicion - 1] = hijo->claves[0];
            prestamos++;
            return;
        }

        // Prestamo del hermano derecho
        if (derecho != nullptr && derecho->cantidad > MIN_CLAVES_HOJA) {
            hijo->claves[hijo->cantidad] = derecho->claves[0];
            hijo->registros[hijo->cantidad] = derecho->registros[0];
            hijo->cantidad++;

            for (int i = 0; i < derecho->cantidad - 1; i++) {
                derecho->claves[i] = derecho->claves[i + 1];
                derecho->registros[i] = derecho->registros[i + 1];
            }

            derecho->cantidad--;

            padre->claves[posicion] = derecho->claves[0];
            prestamos++;
            return;
        }

        // Union de hojas
        NodoBPlus* queda = hijo;
        NodoBPlus* sale = derecho;
        int posicionClave = posicion;

        if (izquierdo != nullptr) {
            queda = izquierdo;
            sale = hijo;
            posicionClave = posicion - 1;
        }

        for (int i = 0; i < sale->cantidad; i++) {
            queda->claves[queda->cantidad] = sale->claves[i];
            queda->registros[queda->cantidad] = sale->registros[i];
            queda->cantidad++;
        }

        queda->siguiente = sale->siguiente;
        delete sale;

        quitarDelPadre(padre, posicionClave, posicionClave + 1);
        uniones++;
        return;
    }

    // Prestamo en nodo interno

    if (izquierdo != nullptr && izquierdo->cantidad > MIN_CLAVES_INTERNO) {
        for (int i = hijo->cantidad; i > 0; i--) {
            hijo->claves[i] = hijo->claves[i - 1];
        }

        for (int i = hijo->cantidad + 1; i > 0; i--) {
            hijo->hijos[i] = hijo->hijos[i - 1];
        }

        hijo->claves[0] = padre->claves[posicion - 1];
        hijo->hijos[0] = izquierdo->hijos[izquierdo->cantidad];
        hijo->cantidad++;

        padre->claves[posicion - 1] = izquierdo->claves[izquierdo->cantidad - 1];
        izquierdo->hijos[izquierdo->cantidad] = nullptr;
        izquierdo->cantidad--;

        prestamos++;
        return;
    }

    if (derecho != nullptr && derecho->cantidad > MIN_CLAVES_INTERNO) {
        hijo->claves[hijo->cantidad] = padre->claves[posicion];
        hijo->hijos[hijo->cantidad + 1] = derecho->hijos[0];
        hijo->cantidad++;

        padre->claves[posicion] = derecho->claves[0];

        for (int i = 0; i < derecho->cantidad - 1; i++) {
            derecho->claves[i] = derecho->claves[i + 1];
        }

        for (int i = 0; i < derecho->cantidad; i++) {
            derecho->hijos[i] = derecho->hijos[i + 1];
        }

        derecho->hijos[derecho->cantidad] = nullptr;
        derecho->cantidad--;

        prestamos++;
        return;
    }

    // Union de nodos internos
    NodoBPlus* queda = hijo;
    NodoBPlus* sale = derecho;
    int posicionClave = posicion;

    if (izquierdo != nullptr) {
        queda = izquierdo;
        sale = hijo;
        posicionClave = posicion - 1;
    }

    queda->claves[queda->cantidad] = padre->claves[posicionClave];
    queda->cantidad++;

    int primerHijo = queda->cantidad;

    for (int i = 0; i < sale->cantidad; i++) {
        queda->claves[queda->cantidad] = sale->claves[i];
        queda->cantidad++;
    }

    for (int i = 0; i <= sale->cantidad; i++) {
        queda->hijos[primerHijo + i] = sale->hijos[i];
    }

    // Borrar el nodo vacio
    delete sale;

    quitarDelPadre(padre, posicionClave, posicionClave + 1);
    uniones++;
}

// Devuelve true si encontro y borro la clave
bool ArbolBPlusHistorial::eliminarEn(NodoBPlus* nodo, string clave) {
    // Caso trivial: estamos en una hoja
    if (nodo->esHoja) {
        int posicion = -1;

        for (int i = 0; i < nodo->cantidad; i++) {
            if (nodo->claves[i] == clave) {
                posicion = i;
            }
        }

        if (posicion == -1) {
            return false;
        }

        for (int i = posicion; i < nodo->cantidad - 1; i++) {
            nodo->claves[i] = nodo->claves[i + 1];
            nodo->registros[i] = nodo->registros[i + 1];
        }

        nodo->cantidad--;
        return true;
    }

    int posicion = posicionHijo(nodo, clave);

    if (!eliminarEn(nodo->hijos[posicion], clave)) {
        return false;
    }

    NodoBPlus* hijo = nodo->hijos[posicion];
    int minimo = MIN_CLAVES_INTERNO;

    if (hijo->esHoja) {
        minimo = MIN_CLAVES_HOJA;
    }

    if (hijo->cantidad < minimo) {
        corregirHijo(nodo, posicion);
    }

    return true;
}

bool ArbolBPlusHistorial::eliminar(string carne, string periodo, string codigoCurso) {
    reiniciarCambios();

    if (raiz == nullptr) {
        return false;
    }

    if (!eliminarEn(raiz, armarClaveNota(carne, periodo, codigoCurso))) {
        return false;
    }

    total--;

    // El arbol baja un nivel
    if (raiz->cantidad == 0) {
        NodoBPlus* vieja = raiz;

        if (raiz->esHoja) {
            raiz = nullptr;
        } else {
            raiz = raiz->hijos[0];
        }

        delete vieja;
        cambiosDeAltura++;
    }

    return true;
}

// Consultas por rango

// Historial de un carne por las hojas enlazadas
int ArbolBPlusHistorial::historial(string carne, vector<RegistroNota>& salida) {
    string prefijo = carne + "|";
    NodoBPlus* hoja = buscarHoja(prefijo);
    int hojasLeidas = 0;

    while (hoja != nullptr) {
        hojasLeidas++;

        for (int i = 0; i < hoja->cantidad; i++) {
            string clave = hoja->claves[i];

            if (clave < prefijo) {
                continue;
            }

            if (clave.compare(0, prefijo.length(), prefijo) != 0) {
                return hojasLeidas;
            }

            salida.push_back(hoja->registros[i]);
        }

        hoja = hoja->siguiente;
    }

    return hojasLeidas;
}

// Notas de un carne entre dos periodos
int ArbolBPlusHistorial::rangoPeriodos(string carne, string desde, string hasta, vector<RegistroNota>& salida) {
    string inicio = carne + "|" + desde;
    string fin = carne + "|" + hasta + "|~";
    NodoBPlus* hoja = buscarHoja(inicio);
    int hojasLeidas = 0;

    while (hoja != nullptr) {
        hojasLeidas++;

        for (int i = 0; i < hoja->cantidad; i++) {
            string clave = hoja->claves[i];

            if (clave < inicio) {
                continue;
            }

            if (clave > fin) {
                return hojasLeidas;
            }

            salida.push_back(hoja->registros[i]);
        }

        hoja = hoja->siguiente;
    }

    return hojasLeidas;
}

void ArbolBPlusHistorial::todos(vector<RegistroNota>& salida) {
    NodoBPlus* hoja = primeraHoja();

    while (hoja != nullptr) {
        for (int i = 0; i < hoja->cantidad; i++) {
            salida.push_back(hoja->registros[i]);
        }

        hoja = hoja->siguiente;
    }
}

// Datos del arbol

int ArbolBPlusHistorial::cantidad() {
    return total;
}

bool ArbolBPlusHistorial::esVacio() {
    return raiz == nullptr;
}

int ArbolBPlusHistorial::altura() {
    if (raiz == nullptr) {
        return 0;
    }

    // Todas las hojas estan al mismo nivel
    int niveles = 1;
    NodoBPlus* actual = raiz;

    while (!actual->esHoja) {
        actual = actual->hijos[0];
        niveles++;
    }

    return niveles;
}

int ArbolBPlusHistorial::contarHojas() {
    int hojas = 0;
    NodoBPlus* actual = primeraHoja();

    while (actual != nullptr) {
        hojas++;
        actual = actual->siguiente;
    }

    return hojas;
}

// Revision de las propiedades del B+

// Revisar un nodo y su subarbol
bool ArbolBPlusHistorial::revisarNodo(NodoBPlus* nodo, string minimo, bool hayMinimo, string maximo, bool hayMaximo,
                                      int nivel, int& nivelHojas, vector<NodoBPlus*>& hojas) {
    if (nodo == nullptr || nodo->cantidad > MAX_CLAVES_BPLUS || nodo->cantidad < 1) {
        return false;
    }

    if (nodo != raiz) {
        int requerido = MIN_CLAVES_INTERNO;

        if (nodo->esHoja) {
            requerido = MIN_CLAVES_HOJA;
        }

        if (nodo->cantidad < requerido) {
            return false;
        }
    }

    for (int i = 0; i < nodo->cantidad; i++) {
        if (i > 0 && nodo->claves[i - 1] >= nodo->claves[i]) {
            return false;
        }

        if (hayMinimo && nodo->claves[i] < minimo) {
            return false;
        }

        if (hayMaximo && nodo->claves[i] >= maximo) {
            return false;
        }
    }

    if (nodo->esHoja) {
        for (int i = 0; i < nodo->cantidad; i++) {
            if (nodo->claves[i] != claveDe(nodo->registros[i])) {
                return false;
            }
        }

        if (nivelHojas == -1) {
            nivelHojas = nivel;
        }

        hojas.push_back(nodo);
        return nivelHojas == nivel;
    }

    for (int i = 0; i <= nodo->cantidad; i++) {
        string minimoHijo = minimo;
        bool hayMinimoHijo = hayMinimo;
        string maximoHijo = maximo;
        bool hayMaximoHijo = hayMaximo;

        if (i > 0) {
            minimoHijo = nodo->claves[i - 1];
            hayMinimoHijo = true;
        }

        if (i < nodo->cantidad) {
            maximoHijo = nodo->claves[i];
            hayMaximoHijo = true;
        }

        if (!revisarNodo(nodo->hijos[i], minimoHijo, hayMinimoHijo, maximoHijo, hayMaximoHijo, nivel + 1, nivelHojas, hojas)) {
            return false;
        }
    }

    return true;
}

bool ArbolBPlusHistorial::estructuraCorrecta() {
    if (raiz == nullptr) {
        return total == 0;
    }

    int nivelHojas = -1;
    vector<NodoBPlus*> hojas;

    if (!revisarNodo(raiz, "", false, "", false, 0, nivelHojas, hojas)) {
        return false;
    }

    // Revisar la lista enlazada de hojas
    NodoBPlus* actual = primeraHoja();
    int suma = 0;

    for (int i = 0; i < (int)hojas.size(); i++) {
        if (actual != hojas[i]) {
            return false;
        }

        suma += actual->cantidad;
        actual = actual->siguiente;
    }

    return actual == nullptr && suma == total;
}

// Vista interna

// Clave corta para la vista
string ArbolBPlusHistorial::claveCorta(string clave) {
    int primera = clave.find('|');
    int segunda = clave.find('|', primera + 1);

    if (primera < 0 || segunda < 0) {
        return clave;
    }

    string carne = clave.substr(0, primera);
    string periodo = clave.substr(primera + 1, segunda - primera - 1);
    string curso = clave.substr(segunda + 1);

    if (carne.length() > 5) {
        carne = carne.substr(5);
    }

    if (periodo.length() > 5) {
        periodo = periodo.substr(5);
    }

    return carne + " " + periodo + " " + curso;
}

void ArbolBPlusHistorial::imprimirNodo(NodoBPlus* nodo) {
    cout << "[ ";

    for (int i = 0; i < nodo->cantidad; i++) {
        cout << claveCorta(nodo->claves[i]);

        if (nodo->esHoja) {
            cout << " (" << nodo->registros[i].nota << ")";
        }

        if (i < nodo->cantidad - 1) {
            cout << " | ";
        }
    }

    cout << " ]";
}

void ArbolBPlusHistorial::imprimirNivel(NodoBPlus* nodo, int nivelActual, int nivelBuscado) {
    if (nodo == nullptr) {
        return;
    }

    if (nivelActual == nivelBuscado) {
        cout << "    ";
        imprimirNodo(nodo);
        cout << "\n";
        return;
    }

    for (int i = 0; i <= nodo->cantidad; i++) {
        imprimirNivel(nodo->hijos[i], nivelActual + 1, nivelBuscado);
    }
}

void ArbolBPlusHistorial::mostrarNiveles() {
    if (raiz == nullptr) {
        cout << "  El arbol esta vacio.\n";
        return;
    }

    int niveles = altura();

    for (int nivel = 0; nivel < niveles; nivel++) {
        cout << "  Nivel " << nivel;

        if (nivel == 0) {
            cout << " (raiz)";
        }

        if (nivel == niveles - 1) {
            cout << " (hojas con las notas)";
        } else {
            cout << " (indice)";
        }

        cout << "\n";
        imprimirNivel(raiz, 0, nivel);
    }

    cout << "\n  Niveles: " << niveles << ", hojas: " << contarHojas()
         << ", registros: " << total << "\n";
}

void ArbolBPlusHistorial::mostrarHojas() {
    if (raiz == nullptr) {
        cout << "  El arbol esta vacio.\n";
        return;
    }

    NodoBPlus* actual = primeraHoja();
    int numero = 1;

    while (actual != nullptr) {
        cout << "  Hoja " << numero << ": ";
        imprimirNodo(actual);

        if (actual->siguiente != nullptr) {
            cout << " ->";
        }

        cout << "\n";
        actual = actual->siguiente;
        numero++;
    }
}

int ArbolBPlusHistorial::mostrarRuta(string carne, string periodo, string codigoCurso) {
    string clave = armarClaveNota(carne, periodo, codigoCurso);
    NodoBPlus* actual = raiz;
    int saltos = 0;

    if (actual == nullptr) {
        cout << "  El arbol esta vacio.\n";
        return 0;
    }

    while (!actual->esHoja) {
        saltos++;
        cout << "  Salto " << saltos << " (indice): ";
        imprimirNodo(actual);
        cout << "\n";

        actual = actual->hijos[posicionHijo(actual, clave)];
    }

    saltos++;
    cout << "  Salto " << saltos << " (hoja):   ";
    imprimirNodo(actual);
    cout << "\n";

    return saltos;
}

void ArbolBPlusHistorial::resumenCambios() {
    cout << "  Divisiones: " << divisiones
         << "   Prestamos: " << prestamos
         << "   Uniones: " << uniones
         << "   Cambios de altura: " << cambiosDeAltura << "\n";
}
