/**
 * @file GraphCore.hpp
 * @brief Definición de la estructura de datos para grafos masivos optimizados.
 * @author David Salvador Contreras Grimaldo
 * @date Noviembre 2025
 * * Este archivo define la interfaz abstracta GrafoBase y su implementación
 * concreta GrafoDisperso utilizando el formato CSR (Compressed Sparse Row).
 */

#ifndef GRAPHCORE_HPP
#define GRAPHCORE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <queue>

/**
 * @class GrafoBase
 * @brief Clase abstracta que define el contrato para cualquier motor de grafos.
 * * Utiliza métodos virtuales puros para garantizar el Polimorfismo.
 */
class GrafoBase {
public:
    virtual ~GrafoBase() {}

    /**
     * @brief Carga un dataset desde un archivo.
     * @param filename Ruta del archivo de texto o binario.
     */
    virtual void cargarDatos(const std::string& filename) = 0;

    /**
     * @brief Calcula el nodo con mayor grado (más conexiones).
     * @return El ID del nodo crítico.
     */
    virtual int obtenerNodoCritico() = 0;

    /**
     * @brief Ejecuta una Búsqueda en Anchura (BFS).
     * @param startNode Nodo de inicio.
     * @param depth Profundidad máxima de búsqueda (niveles).
     * @return Vector de pares (origen, destino) representando las aristas visitadas.
     */
    virtual std::vector<std::pair<int, int>> bfs(int startNode, int depth) = 0;

    virtual int getNumNodos() = 0;
    virtual int getNumAristas() = 0;
};

/**
 * @class GrafoDisperso
 * @brief Implementación optimizada de grafos usando CSR (Compressed Sparse Row).
 * * Esta clase gestiona la memoria manualmente mediante vectores compactos
 * para permitir la carga de millones de nodos en RAM limitada.
 */
class GrafoDisperso : public GrafoBase {
private:
    /** @brief Vector de punteros de fila (CSR). Indica dónde inicia cada nodo en col_indices. */
    std::vector<int> row_ptr;
    
    /** @brief Vector de índices de columna (CSR). Contiene los destinos de las aristas. */
    std::vector<int> col_indices;
    
    int num_nodos;
    int num_aristas;

    /**
     * @brief Guarda el estado actual de la RAM en un archivo binario.
     * @param baseFilename Nombre base del archivo original.
     * @return true si la escritura fue exitosa.
     */
    bool guardarCache(const std::string& baseFilename);

    /**
     * @brief Carga el estado de la memoria desde un archivo binario (Caché).
     * @param baseFilename Nombre base del archivo original.
     * @return true si la lectura fue exitosa.
     */
    bool cargarDesdeCache(const std::string& baseFilename);

public:
    GrafoDisperso();
    void cargarDatos(const std::string& filename) override;
    int obtenerNodoCritico() override;
    std::vector<std::pair<int, int>> bfs(int startNode, int depth) override;
    int getNumNodos() override { return num_nodos; }
    int getNumAristas() override { return num_aristas; }
};

#endif