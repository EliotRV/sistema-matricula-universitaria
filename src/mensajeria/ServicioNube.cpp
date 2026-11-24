// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Implementacion del servicio de la nube

#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdio>
#include "mensajeria/ServicioNube.h"

using namespace std;

const int MAXIMO_BITACORA = 30;

string fechaHoraDelEquipo() {
    time_t ahora = time(nullptr);
    tm partes = *localtime(&ahora);
    // Con espacio de sobra: el compilador no sabe que cada numero es corto
    char texto[80];

    snprintf(texto, sizeof(texto), "%04d-%02d-%02d %02d:%02d:%02d", partes.tm_year + 1900, partes.tm_mon + 1,
             partes.tm_mday, partes.tm_hour, partes.tm_min, partes.tm_sec);

    return texto;
}

ServicioNube::ServicioNube(ColaMensajes& cola, ClienteNube& cliente) : cola(cola), cliente(cliente) {
    corriendo = false;
    procesando = false;
    totalEnviados = 0;
}

ServicioNube::~ServicioNube() {
    detener();
}

void ServicioNube::iniciar() {
    lock_guard<mutex> bloqueo(candado);

    if (corriendo) {
        return;
    }

    corriendo = true;
    hilo = thread(&ServicioNube::trabajar, this);
}

// Cerrar la cola hace que el hilo termine cuando ya no queden mensajes
void ServicioNube::detener() {
    {
        lock_guard<mutex> bloqueo(candado);

        if (!corriendo) {
            return;
        }

        corriendo = false;
    }

    cola.cerrar();

    if (hilo.joinable()) {
        hilo.join();
    }
}

// Hilo consumidor: duerme en la cola hasta que llega un mensaje
void ServicioNube::trabajar() {
    Mensaje mensaje;

    while (true) {
        {
            lock_guard<mutex> bloqueo(candado);
            procesando = false;
        }

        if (!cola.esperarYSacar(mensaje)) {
            return;
        }

        {
            lock_guard<mutex> bloqueo(candado);
            procesando = true;
        }

        auto inicio = chrono::steady_clock::now();
        bool bien = procesar(mensaje);
        auto fin = chrono::steady_clock::now();

        long long milisegundos = chrono::duration_cast<chrono::milliseconds>(fin - inicio).count();
        string linea = nombreDeTipo(mensaje.tipo) + " " + mensaje.coleccion + ": " + mensaje.descripcion
                     + " (" + to_string(milisegundos) + " ms)";

        lock_guard<mutex> bloqueo(candado);

        if (bien) {
            totalEnviados++;
            anotar("[ok]    " + linea);
        } else {
            listaFallidos.push_back(mensaje);
            ultimoError = cliente.ultimoError();
            anotar("[FALLO] " + linea + " -> " + ultimoError);
        }
    }
}

bool ServicioNube::procesar(Mensaje& mensaje) {
    switch (mensaje.tipo) {
        case MENSAJE_INSERTAR:
            return cliente.insertar(mensaje.coleccion, mensaje.datos);

        case MENSAJE_ACTUALIZAR:
            return cliente.actualizar(mensaje.coleccion, mensaje.filtro, mensaje.datos);

        case MENSAJE_ELIMINAR:
            return cliente.eliminar(mensaje.coleccion, mensaje.filtro);

        case MENSAJE_NOTIFICAR: {
            // La hora de envio se pone al momento de atender el mensaje
            Documento datos = mensaje.datos;
            datos.poner("enviada", fechaHoraDelEquipo());
            return cliente.insertar(mensaje.coleccion, datos);
        }
    }

    return false;
}

// Se llama con el mutex tomado
void ServicioNube::anotar(string linea) {
    bitacora.push_back(linea);

    if ((int)bitacora.size() > MAXIMO_BITACORA) {
        bitacora.erase(bitacora.begin());
    }
}

bool ServicioNube::esperarVacia(int segundos) {
    auto limite = chrono::steady_clock::now() + chrono::seconds(segundos);

    while (chrono::steady_clock::now() < limite) {
        bool ocupado;

        {
            lock_guard<mutex> bloqueo(candado);
            ocupado = procesando;
        }

        if (!ocupado && cola.estaVacia()) {
            return true;
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    return false;
}

int ServicioNube::reintentarFallidos() {
    vector<Mensaje> otraVez;

    {
        lock_guard<mutex> bloqueo(candado);
        otraVez = listaFallidos;
        listaFallidos.clear();
    }

    for (int i = 0; i < (int)otraVez.size(); i++) {
        cola.publicar(otraVez[i]);
    }

    return otraVez.size();
}

int ServicioNube::enviados() {
    lock_guard<mutex> bloqueo(candado);
    return totalEnviados;
}

int ServicioNube::fallidos() {
    lock_guard<mutex> bloqueo(candado);
    return listaFallidos.size();
}

int ServicioNube::pendientes() {
    int enCola = cola.cantidad();

    lock_guard<mutex> bloqueo(candado);
    return enCola + (procesando ? 1 : 0);
}

void ServicioNube::mostrarEstado() {
    int enCola = pendientes();

    lock_guard<mutex> bloqueo(candado);

    cout << "  Hilo del servicio: " << (corriendo ? "activo" : "detenido") << "\n";
    cout << "  Mensajes enviados a la nube: " << totalEnviados << "\n";
    cout << "  Mensajes en la cola: " << enCola << (procesando ? " (enviando uno ahora)" : "") << "\n";
    cout << "  Mensajes que fallaron: " << listaFallidos.size() << "\n";

    if (listaFallidos.size() > 0) {
        cout << "  Ultimo error: " << ultimoError << "\n";
    }
}

void ServicioNube::mostrarBitacora() {
    lock_guard<mutex> bloqueo(candado);

    if (bitacora.size() == 0) {
        cout << "  Todavia no se ha enviado ningun mensaje.\n";
        return;
    }

    cout << "  Ultimos mensajes atendidos (el mas reciente al final):\n\n";

    for (int i = 0; i < (int)bitacora.size(); i++) {
        cout << "  " << bitacora[i] << "\n";
    }
}
