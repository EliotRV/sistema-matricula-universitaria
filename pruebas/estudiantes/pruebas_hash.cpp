// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas de la tabla hash extensible de estudiantes

#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
#include "Pruebas.h"
#include "estudiantes/TablaHashEstudiantes.h"

using namespace std;

// Apoyo

Estudiante estudianteDe(string carne) {
    Estudiante estudiante;

    estudiante.carne = carne;
    estudiante.nombre = "Estudiante " + carne;
    estudiante.correo = carne + "@correo.com";

    return estudiante;
}

// Carne con formato AAAA-NNNN
string carneDe(int anio, int numero) {
    string digitos = to_string(numero);

    while (digitos.length() < 4) {
        digitos = "0" + digitos;
    }

    return to_string(anio) + "-" + digitos;
}

// Casos del diseno

void probarHashBasico() {
    cout << "\nHash extensible: insercion, busqueda y division\n";

    TablaHashEstudiantes tabla;

    revisar(tabla.esVacia() && tabla.profundidadGlobal() == 0 && tabla.contarCubetas() == 1, "tabla nueva: profundidad 0 y una cubeta");
    revisar(TablaHashEstudiantes::valorHash("2026-0105") == 20260105ULL, "el hash de 2026-0105 es 20260105");

    revisar(tabla.insertar(estudianteDe("2026-0101")) && tabla.insertar(estudianteDe("2026-0102")), "inserta 0101 y 0102 sin dividir");
    revisar(tabla.profundidadGlobal() == 0, "dos caben en la primera cubeta");

    revisar(tabla.insertar(estudianteDe("2026-0103")), "inserta 0103: la cubeta se divide");
    revisar(tabla.profundidadGlobal() == 1 && tabla.contarCubetas() == 2, "profundidad 1 y dos cubetas");

    tabla.insertar(estudianteDe("2026-0104"));
    tabla.insertar(estudianteDe("2026-0105"));

    // Igual a la figura del Entregable 1: 00 y 10 comparten [0102, 0104],
    // 01 -> [0101, 0105] y 11 -> [0103]
    revisar(tabla.profundidadGlobal() == 2 && tabla.contarCubetas() == 3, "tras 0105: profundidad 2 y tres cubetas (figura del diseno)");
    revisar(tabla.estructuraCorrecta(), "estructura correcta");

    cout << "\n";
    tabla.mostrarDirectorio();
    cout << "\n";

    revisar(!tabla.insertar(estudianteDe("2026-0103")), "rechaza un carne repetido");
    revisar(tabla.cantidad() == 5, "quedan 5 estudiantes");

    Estudiante* encontrado = tabla.buscar("2026-0104");
    revisar(encontrado != nullptr && encontrado->nombre == "Estudiante 2026-0104", "encuentra 0104");
    revisar(tabla.buscar("2026-0199") == nullptr, "no encuentra un carne que no existe");

    revisar(tabla.modificar("2026-0104", "Jose Araya Brenes", "jarayab@correo.com"), "modifica 0104");
    revisar(tabla.buscar("2026-0104")->nombre == "Jose Araya Brenes", "el nombre nuevo quedo guardado");
    revisar(!tabla.modificar("2026-0199", "Nadie", ""), "no modifica un carne que no existe");
}

void probarHashEliminacion() {
    cout << "\nHash extensible: eliminacion, union y reduccion\n";

    TablaHashEstudiantes tabla;

    for (int i = 1; i <= 5; i++) {
        tabla.insertar(estudianteDe(carneDe(2026, 100 + i)));
    }

    revisar(!tabla.eliminar("2026-0199"), "no elimina un carne que no existe");

    // Al borrar 0103 su cubeta queda vacia y se une con la de 0101 y 0105;
    // ninguna cubeta usa ya 2 bits, asi que el directorio baja a 1
    revisar(tabla.eliminar("2026-0103"), "elimina 0103");
    revisar(tabla.buscar("2026-0103") == nullptr, "0103 ya no esta");
    revisar(tabla.profundidadGlobal() == 1 && tabla.contarCubetas() == 2, "se unen cubetas y el directorio baja a profundidad 1");
    revisar(tabla.estructuraCorrecta() && tabla.cantidad() == 4, "estructura correcta con 4 estudiantes");

    tabla.eliminar("2026-0101");
    tabla.eliminar("2026-0102");

    revisar(tabla.profundidadGlobal() == 0 && tabla.contarCubetas() == 1, "con 2 estudiantes vuelve a una cubeta");

    tabla.eliminar("2026-0104");
    tabla.eliminar("2026-0105");

    revisar(tabla.esVacia() && tabla.estructuraCorrecta(), "queda vacia y correcta");
    revisar(tabla.insertar(estudianteDe("2026-0101")), "se puede volver a usar");
}

// Operaciones al azar comparadas con std::map

void probarHashAlAzar() {
    cout << "\nHash extensible: 60 000 operaciones al azar contra std::map\n";

    TablaHashEstudiantes tabla;
    map<string, string> esperado;
    srand(2027);

    bool todoBien = true;
    bool estructuraBien = true;

    for (int i = 0; i < 60000; i++) {
        string carne = carneDe(2020 + rand() % 10, rand() % 1000);
        int operacion = rand() % 4;

        if (operacion <= 1) {
            bool insertado = tabla.insertar(estudianteDe(carne));
            bool deberia = esperado.count(carne) == 0;

            if (insertado != deberia) {
                todoBien = false;
            }

            if (deberia) {
                esperado[carne] = "Estudiante " + carne;
            }
        } else if (operacion == 2) {
            bool eliminado = tabla.eliminar(carne);

            if (eliminado != (esperado.erase(carne) == 1)) {
                todoBien = false;
            }
        } else {
            string nombre = "Cambio " + to_string(i);
            bool modificado = tabla.modificar(carne, nombre, "");

            if (modificado != (esperado.count(carne) == 1)) {
                todoBien = false;
            }

            if (modificado) {
                esperado[carne] = nombre;
            }
        }

        if (i % 2000 == 0 && !tabla.estructuraCorrecta()) {
            estructuraBien = false;
        }
    }

    revisar(todoBien, "insertar, eliminar y modificar responden igual que std::map");
    revisar(estructuraBien && tabla.estructuraCorrecta(), "la estructura se mantiene correcta durante todas las operaciones");
    revisar(tabla.cantidad() == (int)esperado.size(), "misma cantidad de estudiantes");

    vector<Estudiante> lista;
    tabla.listar(lista);

    bool iguales = lista.size() == esperado.size();
    int posicion = 0;

    for (auto par : esperado) {
        if (!iguales) {
            break;
        }

        if (lista[posicion].carne != par.first || lista[posicion].nombre != par.second) {
            iguales = false;
        }

        posicion++;
    }

    revisar(iguales, "listar devuelve los mismos datos en orden de carne");

    cout << "  Al final: " << tabla.cantidad() << " estudiantes, profundidad " << tabla.profundidadGlobal()
         << ", " << tabla.contarCubetas() << " cubetas\n";
}

void probarHashGrande() {
    cout << "\nHash extensible: 100 000 estudiantes\n";

    TablaHashEstudiantes tabla;
    bool todos = true;

    for (int anio = 2000; anio < 2010; anio++) {
        for (int numero = 0; numero < 10000; numero++) {
            if (!tabla.insertar(estudianteDe(carneDe(anio, numero)))) {
                todos = false;
            }
        }
    }

    revisar(todos && tabla.cantidad() == 100000, "inserta los 100 000");
    revisar(tabla.estructuraCorrecta(), "estructura correcta");
    revisar(tabla.profundidadGlobal() <= PROFUNDIDAD_MAXIMA, "el directorio no pasa del limite");
    revisar(tabla.factorDeCarga() > 0.5, "las cubetas quedan a mas de la mitad en promedio");

    cout << "  Profundidad " << tabla.profundidadGlobal() << ", " << tabla.tamanoDirectorio() << " entradas, "
         << tabla.contarCubetas() << " cubetas, factor de carga " << tabla.factorDeCarga() << "\n";

    bool encontrados = true;

    for (int anio = 2000; anio < 2010; anio++) {
        for (int numero = 0; numero < 10000; numero += 7) {
            if (tabla.buscar(carneDe(anio, numero)) == nullptr) {
                encontrados = false;
            }
        }
    }

    revisar(encontrados, "encuentra una muestra de los insertados");

    for (int anio = 2000; anio < 2010; anio++) {
        for (int numero = 0; numero < 10000; numero++) {
            tabla.eliminar(carneDe(anio, numero));
        }
    }

    revisar(tabla.esVacia() && tabla.profundidadGlobal() == 0 && tabla.contarCubetas() == 1, "al borrar todo vuelve a profundidad 0");
}
