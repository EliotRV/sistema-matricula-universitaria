// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas de la cola de mensajes entre hilos y de los mensajes de los componentes

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include "Pruebas.h"
#include "mensajeria/ColaMensajes.h"
#include "app/SistemaMatricula.h"
#include "comun/Colecciones.h"

using namespace std;

// Productores y consumidor en hilos distintos

void probarColaMensajes() {
    cout << "\nCola de mensajes entre hilos\n";

    ColaMensajes cola;

    revisar(cola.estaVacia(), "cola nueva vacia");

    cola.publicar(mensajeInsertar("a", Documento(), "1"));
    cola.publicar(mensajeEliminar("b", Documento(), "2"));

    Mensaje mensaje;
    revisar(cola.esperarYSacar(mensaje) && mensaje.descripcion == "1" && mensaje.tipo == MENSAJE_INSERTAR, "sale primero el primero que entro");
    revisar(cola.esperarYSacar(mensaje) && mensaje.descripcion == "2" && cola.estaVacia(), "luego el segundo y queda vacia");

    // Consumidor esperando
    bool recibido = false;
    thread consumidor([&] {
        Mensaje llegada;
        recibido = cola.esperarYSacar(llegada) && llegada.descripcion == "tarde";
    });

    this_thread::sleep_for(chrono::milliseconds(150));
    cola.publicar(mensajeInsertar("a", Documento(), "tarde"));
    consumidor.join();

    revisar(recibido, "el consumidor espera dormido y despierta al llegar un mensaje");

    // Varios productores y un consumidor
    const int PRODUCTORES = 4;
    const int POR_PRODUCTOR = 2500;

    ColaMensajes compartida;
    vector<int> ultimoDe(PRODUCTORES, -1);
    int recibidos = 0;
    bool enOrden = true;

    thread lector([&] {
        Mensaje llegada;

        while (compartida.esperarYSacar(llegada)) {
            int productor = llegada.coleccion[0] - '0';
            int numero = atoi(llegada.descripcion.c_str());

            // Orden de cada productor
            if (numero != ultimoDe[productor] + 1) {
                enOrden = false;
            }

            ultimoDe[productor] = numero;
            recibidos++;
        }
    });

    vector<thread> productores;

    for (int p = 0; p < PRODUCTORES; p++) {
        productores.push_back(thread([&compartida, p, POR_PRODUCTOR] {
            for (int i = 0; i < POR_PRODUCTOR; i++) {
                compartida.publicar(mensajeInsertar(to_string(p), Documento(), to_string(i)));
            }
        }));
    }

    for (int p = 0; p < PRODUCTORES; p++) {
        productores[p].join();
    }

    // Cerrar la cola
    compartida.cerrar();
    lector.join();

    revisar(recibidos == PRODUCTORES * POR_PRODUCTOR, "el consumidor recibe los 10 000 mensajes de 4 hilos");
    revisar(enOrden, "los mensajes de cada productor llegan en orden");
    revisar(compartida.estaVacia(), "la cola queda vacia");

    Mensaje despues;
    revisar(!compartida.esperarYSacar(despues), "con la cola cerrada y vacia ya no se espera");
}

// Mensajes de los componentes

class Recolector : public IPublicador {
public:
    vector<Mensaje> mensajes;

    void publicar(Mensaje mensaje) override {
        mensajes.push_back(mensaje);
    }

    int contar(TipoMensaje tipo, string coleccion) {
        int total = 0;

        for (int i = 0; i < (int)mensajes.size(); i++) {
            if (mensajes[i].tipo == tipo && mensajes[i].coleccion == coleccion) {
                total++;
            }
        }

        return total;
    }
};

void probarMensajesDeComponentes() {
    cout << "\nMensajes que publican los componentes\n";

    SistemaMatricula sistema;
    Recolector recolector;
    stringstream descarte;
    streambuf* anterior = cout.rdbuf(descarte.rdbuf());

    FuenteArchivos archivos("datos/");
    sistema.cargarDatos(archivos);

    // Conectar la cola despues de cargar
    sistema.conectarCola(&recolector);
    sistema.cambiarHoraActual(armarFechaHora(2026, 12, 7, 12, 0));

    ServicioMatricula& matricula = sistema.matricula();

    matricula.matricular("2026-0105", "INF-301", 1, sistema.horaActual());
    matricula.matricular("2026-0101", "INF-301", 1, sistema.horaActual());
    matricula.unirseAEspera("2026-0104", "INF-301", 1, sistema.horaActual());

    int antesDeRetirar = recolector.mensajes.size();
    matricula.retirar("2026-0101", "INF-301");
    vector<Mensaje> retiro(recolector.mensajes.begin() + antesDeRetirar, recolector.mensajes.end());

    Estudiante nuevo;
    nuevo.carne = "2026-0300";
    nuevo.nombre = "Estudiante Nuevo";
    sistema.estudiantes().registrar(nuevo);
    sistema.eliminarEstudiante("2026-0300");

    cout.rdbuf(anterior);

    revisar(recolector.contar(MENSAJE_INSERTAR, COL_MATRICULAS) == 3, "3 matriculas nuevas: Sofia, Ana y Jose (desde la espera)");
    revisar(recolector.contar(MENSAJE_INSERTAR, COL_ESPERA) == 1 && recolector.contar(MENSAJE_ELIMINAR, COL_ESPERA) == 1,
            "Jose entra a la cola y sale de ella");

    // Orden de los mensajes al retirar
    bool ordenRetiro = retiro.size() >= 4
        && retiro[0].tipo == MENSAJE_ACTUALIZAR && retiro[0].coleccion == COL_MATRICULAS && retiro[0].datos.valor("estado") == "retirada"
        && retiro[1].tipo == MENSAJE_ELIMINAR && retiro[1].coleccion == COL_ESPERA
        && retiro[2].tipo == MENSAJE_INSERTAR && retiro[2].datos.valor("carne") == "2026-0104"
        && retiro[3].tipo == MENSAJE_NOTIFICAR;

    revisar(ordenRetiro, "retirar: actualizar a Ana, sacar a Jose de la cola, matricularlo y avisarle");
    revisar(recolector.contar(MENSAJE_NOTIFICAR, COL_NOTIFICACIONES) == 4, "4 notificaciones a estudiantes");
    revisar(recolector.contar(MENSAJE_INSERTAR, COL_ESTUDIANTES) == 1 && recolector.contar(MENSAJE_ELIMINAR, COL_ESTUDIANTES) == 1,
            "registrar y eliminar un estudiante generan su mensaje");

    // Subir todo
    recolector.mensajes.clear();
    int publicados = sistema.publicarTodo();

    revisar(publicados == (int)recolector.mensajes.size(), "publicarTodo cuenta bien sus mensajes");
    revisar(recolector.contar(MENSAJE_ELIMINAR, COL_CURSOS) == 1 && recolector.contar(MENSAJE_INSERTAR, COL_NOTAS) == 39,
            "vacia cada coleccion y vuelve a subir las 39 notas");
}
