# Sistema de Matrícula Universitaria

[![Integración continua](https://github.com/EliotRV/sistema-matricula-universitaria/actions/workflows/ci.yml/badge.svg)](https://github.com/EliotRV/sistema-matricula-universitaria/actions/workflows/ci.yml)

Proyecto final de **Estructura de Datos II**, de Eliot Rojas Vargas.

El programa gestiona la matrícula de una universidad desde la consola. Se pueden registrar estudiantes, armar el plan de estudios con sus requisitos y asignar citas de matrícula. También permite matricular grupos con cupo y lista de espera, y llevar el historial de notas. Cada componente usa la estructura de datos que mejor se ajusta a su trabajo. Los cambios se guardan en una base de datos MongoDB en la nube, enviados mediante una cola de mensajes que atiende un hilo aparte.

## Estructuras de datos

| Componente | Estructura | Para qué se usa |
|---|---|---|
| Estudiantes | **Tabla hash extensible**: directorio, cubetas de 2, profundidad global y local | Buscar un estudiante por carné en tiempo constante. La tabla crece y se encoge partiendo y uniendo cubetas. |
| Citas de matrícula | **Árbol rojo-negro** | Agenda ordenada por fecha y hora: búsqueda, rangos y validar si una cita ya está habilitada, en O(log n). |
| Historial académico | **Árbol B+**: hasta 3 claves por nodo, hojas enlazadas | Notas por clave `carne\|periodo\|curso`. El historial de un estudiante se recorre siguiendo las hojas enlazadas. |
| Plan de estudios | **Grafo dirigido acíclico**: listas de adyacencia | Requisitos entre cursos. Detecta ciclos, da el orden topológico (Kahn) y los cursos habilitados. |
| Grupos | **Cola enlazada (FIFO)** por grupo | Lista de espera: cuando se libera un cupo, lo recibe el primero de la cola. |
| Comunicación | **Cola de mensajes** con `mutex` y `condition_variable` | Productor-consumidor: los componentes publican sus cambios y un hilo los envía a la nube. |

## Organización del código

```
programa/
├── src/
│   ├── main.cpp            arma los componentes, carga los datos y abre el menú
│   ├── app/                SistemaMatricula (coordina los servicios) y Menu
│   ├── comun/              consola, lectura de archivos, documentos y colecciones
│   ├── estudiantes/        tabla hash extensible + ServicioEstudiantes
│   ├── citas/              árbol rojo-negro + ServicioCitas
│   ├── historial/          árbol B+ + ServicioHistorial
│   ├── plan/               grafo de requisitos + ServicioPlan
│   ├── matricula/          grupos, colas de espera + ServicioMatricula
│   ├── mensajeria/         mensajes, cola entre hilos + ServicioNube
│   └── nube/               cliente HTTP (curl), lector JSON y FuenteNube
├── pruebas/                pruebas automáticas, una carpeta por componente
├── datos/                  datos iniciales en archivos de texto
├── Makefile
└── .github/workflows/ci.yml
```

Cada componente expone un **servicio** que es su única puerta de entrada. `SistemaMatricula` los coordina cuando una operación toca varios componentes, por ejemplo matricular, que revisa la cita, los requisitos, el historial, los choques de horario y el cupo.

## Compilar y ejecutar

Se necesita `g++` con C++17 y `curl`, que ya viene en Windows 10/11 y en casi cualquier Linux.

**Windows (WinLibs / MinGW):**

```
mingw32-make
programa.exe
```

**Linux:**

```
make
./programa
```

El programa se ejecuta desde la carpeta `programa/`, porque ahí busca la carpeta `datos/`.

En VS Code también se puede compilar con la tarea *C++: compilar carpeta* (Ctrl+Shift+B), y correr las pruebas con la tarea *C++: correr pruebas*.

## Pruebas

```
mingw32-make pruebas     (Windows)
make pruebas             (Linux)
```

Las pruebas revisan cada estructura con operaciones al azar, comparando el resultado contra los contenedores de la biblioteca estándar. También verifican las propiedades de cada árbol y de la tabla hash, las reglas de matrícula, el escenario completo del sistema y la cola de mensajes con varios hilos. Al final cuentan las reservas y liberaciones de memoria para confirmar que no queden fugas.

La prueba contra la nube real no se ejecuta por defecto. Para incluirla:

```
set PRUEBAS_NUBE=1 && mingw32-make pruebas     (Windows, cmd)
PRUEBAS_NUBE=1 make pruebas                    (Linux)
```

## Uso

Menú principal:

```
 1. Estudiantes            (tabla hash)
 2. Plan de estudios       (grafo)
 3. Grupos y matricula     (colas de espera)
 4. Citas de matricula     (arbol rojo-negro)
 5. Historial academico    (arbol B+)
 6. Nube y cola de mensajes
 7. Cambiar hora actual
 8. Volver a cargar los datos
 0. Salir
```

Cada módulo permite insertar, buscar, modificar y eliminar, y tiene una opción para **ver la estructura por dentro**:

- el directorio y las cubetas de la tabla hash;
- el árbol rojo-negro con sus colores y propiedades;
- el árbol B+ por niveles y sus hojas enlazadas;
- las listas de adyacencia del grafo;
- las colas de espera de cada grupo.

La *hora actual* (opción 7) permite simular el momento en que un estudiante llega a matricular, para probar si su cita ya está habilitada.

## Nube y cola de mensajes

Los datos se guardan en la API MongoDB del curso (`https://paginas-web-cr.com/Api/apis/mongodb.php`), en colecciones con el prefijo `eliotrojas_`:

`eliotrojas_estudiantes`, `eliotrojas_cursos`, `eliotrojas_requisitos`, `eliotrojas_notas`, `eliotrojas_citas`, `eliotrojas_grupos`, `eliotrojas_matriculas`, `eliotrojas_espera` y `eliotrojas_notificaciones`.

1. **Al iniciar**, el programa lee las colecciones de la nube. Si no hay conexión o están vacías, usa los archivos de `datos/` y lo avisa.
2. **Cada cambio** (registrar, matricular, retirar, corregir una nota...) se publica como mensaje en la cola, y el menú sigue sin esperar a la red.
3. **El servicio de la nube**, en su propio hilo, saca los mensajes en orden y los envía con `curl`. Si un envío falla, el mensaje queda en una lista para reintentarlo desde el menú 6.
4. **Notificaciones**: algunos eventos le avisan al estudiante, por ejemplo cuando recibe un cupo que se liberó en un grupo donde estaba en la lista de espera. Esos avisos quedan en `eliotrojas_notificaciones`.
5. **Al salir**, el programa espera a que la cola termine de enviar los mensajes pendientes.

## Integración continua

Cada `push` a `main` ejecuta el flujo de GitHub Actions (`.github/workflows/ci.yml`) en Ubuntu y en Windows:

1. compila el programa con `make`;
2. corre todas las pruebas con `make pruebas` (si alguna falla, la ejecución queda en rojo);
3. publica como artefacto descargable el ejecutable listo para usar junto con la carpeta `datos/`.

Para descargar el artefacto: pestaña **Actions** → la última ejecución → **Artifacts**.
