# distutils: language = c++
# @file neuronet.pyx
# @brief Capa de enlace entre Python y C++.
# Define cómo Python puede ver y usar la clase GrafoDisperso.

from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.string cimport string

# 1. Declaración de la interfaz externa (Header C++)
cdef extern from "GraphCore.hpp":
    cdef cppclass GrafoDisperso:
        GrafoDisperso() except +
        void cargarDatos(string filename)
        int obtenerNodoCritico()
        int getNumNodos()
        int getNumAristas()
        vector[pair[int, int]] bfs(int startNode, int depth)

# 2. Clase Wrapper para Python (PyGraph)
# Esta clase "envuelve" el puntero de C++ y gestiona su ciclo de vida.
cdef class PyGraph:
    cdef GrafoDisperso* c_graph  # Puntero raw al objeto C++

    def __cinit__(self):
        """Constructor: Asigna memoria en el Heap de C++."""
        self.c_graph = new GrafoDisperso()

    def __dealloc__(self):
        """Destructor: Libera la memoria manualmente para evitar fugas."""
        del self.c_graph

    def load_data(self, filename):
        """Convierte string Python a string C++ y llama al motor."""
        cdef string cpp_filename = filename.encode('utf-8')
        self.c_graph.cargarDatos(cpp_filename)

    def get_stats(self):
        """Retorna un diccionario Python con métricas del grafo."""
        return {
            "nodos": self.c_graph.getNumNodos(),
            "aristas": self.c_graph.getNumAristas(),
            "critico": self.c_graph.obtenerNodoCritico()
        }

    def run_bfs(self, start_node, depth):
        """
        Ejecuta BFS en C++ y convierte el vector resultante
        a una lista de tuplas de Python.
        """
        cdef vector[pair[int, int]] result = self.c_graph.bfs(start_node, depth)
        return [(p.first, p.second) for p in result]