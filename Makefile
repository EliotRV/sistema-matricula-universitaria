# Proyecto: Sistema de Matricula Universitaria
# Estructura de Datos II
# Eliot Rojas Vargas
#
# Uso (desde la carpeta programa/):
#   make            compila el programa
#   make pruebas    compila y corre las pruebas
#   make limpiar    borra los ejecutables
# En Windows con WinLibs el comando es mingw32-make

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc -pthread

# Todos los .cpp de src/ y de cada carpeta de componente
FUENTES = $(wildcard src/*.cpp src/*/*.cpp)
FUENTES_SIN_MAIN = $(filter-out src/main.cpp, $(FUENTES))
PRUEBAS = $(wildcard pruebas/*.cpp pruebas/*/*.cpp)

ifeq ($(OS),Windows_NT)
    EXT = .exe
    BORRAR = del /Q
    # Enlace estatico en Windows
    ESTATICO = -static
else
    EXT =
    BORRAR = rm -f
    ESTATICO =
endif

PROGRAMA = programa$(EXT)
PROGRAMA_PRUEBAS = pruebas_sistema$(EXT)

.PHONY: all pruebas limpiar

all: $(PROGRAMA)

$(PROGRAMA): $(FUENTES)
	$(CXX) $(CXXFLAGS) -g $(ESTATICO) $(FUENTES) -o $@

$(PROGRAMA_PRUEBAS): $(FUENTES_SIN_MAIN) $(PRUEBAS)
	$(CXX) $(CXXFLAGS) -O2 -Ipruebas $(ESTATICO) $(PRUEBAS) $(FUENTES_SIN_MAIN) -o $@

pruebas: $(PROGRAMA_PRUEBAS)
	./$(PROGRAMA_PRUEBAS)

limpiar:
	-$(BORRAR) $(PROGRAMA) $(PROGRAMA_PRUEBAS)
