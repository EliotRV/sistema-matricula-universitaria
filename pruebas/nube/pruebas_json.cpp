// Proyecto: Sistema de Matricula Universitaria
// Estructura de Datos II
// Eliot Rojas Vargas
// Pruebas del JSON, de las colecciones y (opcional) de la nube real

#include <iostream>
#include <cstdlib>
#include <vector>
#include "Pruebas.h"
#include "nube/Json.h"
#include "nube/ClienteNube.h"
#include "comun/Colecciones.h"

using namespace std;

void probarJson() {
    cout << "\nJSON de la API\n";

    Documento documento;
    documento.poner("carne", "2026-0101");
    documento.poner("nombre", "Ana \"la\" Mora\\Solis");
    documento.poner("carne", "2026-0102");

    revisar(documento.cantidad() == 2 && documento.valor("carne") == "2026-0102", "poner reemplaza un campo que ya existe");
    revisar(documento.aJson() == "{\"carne\":\"2026-0102\",\"nombre\":\"Ana \\\"la\\\" Mora\\\\Solis\"}", "aJson escapa comillas y barras");

    // Igual a lo que devuelve la API, con saltos de linea y un _id anidado
    string respuesta =
        "\n\n{\n    \"coleccion\": \"eliotrojas_notas\",\n    \"total\": 2,\n    \"documentos\": [\n"
        "        {\n            \"_id\": {\n                \"$oid\": \"6b04e4b64f9f6114ef019c21\"\n            },\n"
        "            \"carne\": \"2026-0101\",\n            \"nota\": 88,\n            \"aprobado\": true,\n"
        "            \"texto\": \"con \\\"comillas\\\" y \\u00e1\"\n        },\n"
        "        { \"carne\": \"2026-0102\", \"nada\": null, \"lista\": [1, 2, {\"a\": 3}] }\n    ]\n}";

    vector<Documento> documentos;
    revisar(leerDocumentos(respuesta, documentos) && documentos.size() == 2, "lee los 2 documentos de una respuesta real");
    revisar(documentos[0].valor("_id") == "6b04e4b64f9f6114ef019c21", "el _id queda como el texto del oid");
    revisar(documentos[0].valor("nota") == "88" && documentos[0].valor("aprobado") == "true", "numeros y booleanos pasan a texto");
    revisar(documentos[0].valor("texto") == "con \"comillas\" y ?", "quita los escapes y cambia lo que no es ASCII");
    revisar(documentos[1].valor("nada") == "" && documentos[1].tiene("lista"), "null queda vacio y los arreglos se saltan");

    vector<Documento> vacios;
    revisar(leerDocumentos("{\"coleccion\":\"x\",\"total\":0,\"documentos\":[]}", vacios) && vacios.size() == 0, "una coleccion vacia se lee sin documentos");

    vector<Documento> malos;
    revisar(!leerDocumentos("<html>error</html>", malos), "una pagina HTML no se acepta");
    revisar(!leerDocumentos("{\"documentos\": [ {\"a\": 1} ", malos), "un JSON cortado no se acepta");
    revisar(!leerDocumentos("{\"mensaje\":\"ok\"}", malos), "una respuesta sin documentos no se acepta");

    revisar(valorDeClave("{\"mensaje\":\"Documento eliminado\",\"eliminados\":3}", "eliminados") == "3", "lee el valor de una clave de la respuesta");
}

// Cada entidad pasa a documento y vuelve igual

void probarColecciones() {
    cout << "\nColecciones: de entidad a documento y de vuelta\n";

    Estudiante estudiante;
    estudiante.carne = "2026-0101";
    estudiante.nombre = "Ana Mora Solis";
    estudiante.correo = "amoras@ucenfotec.ac.cr";

    Documento documento = documentoDe(estudiante);
    Estudiante leido;
    revisar(leerDocumento(documento, leido) && leido.carne == estudiante.carne && leido.nombre == estudiante.nombre
            && leido.correo == estudiante.correo, "estudiante");

    CitaMatricula cita;
    cita.fechaHora = armarFechaHora(2026, 12, 7, 8, 30);
    cita.carne = "2026-0101";

    documento = documentoDe(cita);
    CitaMatricula citaLeida;
    revisar(documento.valor("fecha") == "2026-12-07" && documento.valor("hora") == "08:30", "la cita se guarda con fecha y hora legibles");
    revisar(leerDocumento(documento, citaLeida) && citaLeida.fechaHora == cita.fechaHora, "cita");

    Grupo grupo;
    grupo.codigoCurso = "INF-301";
    grupo.numero = 1;
    grupo.periodo = "2027-C1";
    grupo.dia = 1;
    grupo.horaInicio = 1800;
    grupo.horaFin = 2100;
    grupo.cupo = 2;
    grupo.matriculados = 0;

    documento = documentoDe(grupo);
    Grupo grupoLeido;
    revisar(documento.valor("dia") == "Lunes" && documento.valor("inicio") == "18:00", "el grupo se guarda con el dia y la hora legibles");
    revisar(leerDocumento(documento, grupoLeido) && grupoLeido.dia == 1 && grupoLeido.horaFin == 2100 && grupoLeido.cupo == 2, "grupo");

    RegistroNota nota;
    nota.carne = "2026-0102";
    nota.periodo = "2026-C1";
    nota.codigoCurso = "MAT-101";
    nota.nota = 60;

    documento = documentoDe(nota);
    RegistroNota notaLeida;
    revisar(documento.valor("nota") == "60" && leerDocumento(documento, notaLeida) && notaLeida.nota == 60, "nota (el numero viaja como texto)");

    Matricula matricula;
    matricula.carne = "2026-0105";
    matricula.codigoCurso = "INF-301";
    matricula.numeroGrupo = 1;
    matricula.periodo = "2027-C1";
    matricula.activa = false;

    documento = documentoDe(matricula);
    Matricula matriculaLeida;
    revisar(documento.valor("estado") == "retirada" && leerDocumento(documento, matriculaLeida) && !matriculaLeida.activa, "matricula retirada");

    Documento incompleto;
    incompleto.poner("carne", "2026-0101");
    revisar(!leerDocumento(incompleto, citaLeida) && !leerDocumento(incompleto, grupoLeido), "un documento incompleto no se acepta");

    Documento filtro = filtroMatriculaActiva("2026-0101", "INF-301", 1, "2027-C1");
    revisar(filtro.valor("estado") == "activa" && filtro.valor("grupo") == "1" && filtro.cantidad() == 5, "filtro de la matricula activa");
}

// Solo si se pide: PRUEBAS_NUBE=1 (no se corre en la integracion continua)

void probarNubeReal() {
    const char* variable = getenv("PRUEBAS_NUBE");

    if (variable == nullptr || string(variable) != "1") {
        cout << "\nNube real: se omite (para probarla: PRUEBAS_NUBE=1)\n";
        return;
    }

    cout << "\nNube real: insertar, consultar, actualizar y borrar en eliotrojas_prueba\n";

    ClienteNube cliente;
    Documento todos;

    cliente.eliminar("eliotrojas_prueba", todos);

    Documento datos;
    datos.poner("carne", "2026-0101");
    datos.poner("texto", "comillas \" y barra \\");

    revisar(cliente.insertar("eliotrojas_prueba", datos), "inserta un documento");

    vector<Documento> documentos;
    revisar(cliente.consultar("eliotrojas_prueba", documentos) && documentos.size() == 1, "lo consulta");
    revisar(documentos.size() == 1 && documentos[0].valor("texto") == "comillas \" y barra \\", "el texto vuelve igual");

    Documento cambio;
    cambio.poner("texto", "cambiado");
    revisar(cliente.actualizar("eliotrojas_prueba", filtroEstudiante("2026-0101"), cambio), "lo actualiza");

    documentos.clear();
    cliente.consultar("eliotrojas_prueba", documentos);
    revisar(documentos.size() == 1 && documentos[0].valor("texto") == "cambiado", "el cambio quedo guardado");

    revisar(cliente.eliminar("eliotrojas_prueba", todos), "lo borra");

    documentos.clear();
    revisar(cliente.consultar("eliotrojas_prueba", documentos) && documentos.size() == 0, "la coleccion queda vacia");

    ClienteNube inexistente("https://no-existe.invalid/api.php");
    revisar(!inexistente.disponible() && inexistente.ultimoError() == "sin conexion con la nube", "sin conexion se informa el error");
}
