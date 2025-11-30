/**
 * @file GraphCore.cpp
 * @brief Implementación de la lógica de grafos y gestión de caché binaria.
 */

#include "GraphCore.hpp"
#include <fstream>
#include <iostream>

GrafoDisperso::GrafoDisperso() : num_nodos(0), num_aristas(0) {}

/**
 * @brief Verifica la existencia de un archivo de manera compatible con Windows/Linux.
 * @param name Ruta del archivo.
 * @return true si el archivo existe y es legible.
 */
bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

/**
 * @brief Carga el grafo. Decide inteligentemente entre Texto o Caché Binaria.
 * * Algoritmo:
 * 1. Verifica si existe la versión .bin del archivo.
 * 2. Si existe, carga directamente los vectores a RAM (milisegundos).
 * 3. Si no, lee el .txt línea por línea, construye el CSR y genera la caché.
 */
void GrafoDisperso::cargarDatos(const std::string& filename) {
    std::string cacheFile = filename + ".bin";

    // 1. Estrategia de Caché: Intentar carga rápida
    if (fileExists(cacheFile)) {
        if (cargarDesdeCache(cacheFile)) {
            std::cout << "[C++ Core] Cache cargada. (Modo optimizado)" << std::endl;
            return;
        }
    }

    // 2. Carga Lenta (Parsing de Texto)
    std::cout << "[C++ Core] Procesando texto plano..." << std::endl;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir " << filename << std::endl;
        return;
    }

    int u, v;
    int max_id = 0;
    std::string line;

    // Ignorar comentarios del dataset SNAP (líneas con #)
    while (file >> std::ws && file.peek() == '#') std::getline(file, line);
    std::streampos start = file.tellg();

    // Pasada 1: Determinar dimensiones del grafo
    while (file >> u >> v) {
        if (u > max_id) max_id = u;
        if (v > max_id) max_id = v;
        num_aristas++;
    }
    num_nodos = max_id + 1;

    // Reservar memoria exacta
    row_ptr.assign(num_nodos + 1, 0);
    col_indices.resize(num_aristas);

    // Pasada 2: Calcular grados (Histograma)
    file.clear(); 
    file.seekg(start);
    std::vector<int> counts(num_nodos, 0);
    while (file >> u >> v) counts[u]++;

    // Construir row_ptr (Suma Prefija)
    row_ptr[0] = 0;
    for (int i = 0; i < num_nodos; ++i) row_ptr[i+1] = row_ptr[i] + counts[i];

    // Pasada 3: Llenar aristas en col_indices
    std::vector<int> current = row_ptr;
    file.clear(); 
    file.seekg(start);
    while (file >> u >> v) {
        col_indices[current[u]] = v;
        current[u]++;
    }
    
    // 3. Generar Caché para el futuro
    guardarCache(cacheFile);
}

/**
 * @brief Vuelca la memoria RAM directamente al disco.
 * Serializa los vectores row_ptr y col_indices sin conversión de formato.
 */
bool GrafoDisperso::guardarCache(const std::string& cacheFile) {
    std::ofstream out(cacheFile, std::ios::binary);
    if (!out) return false;
    out.write((char*)&num_nodos, sizeof(int));
    out.write((char*)&num_aristas, sizeof(int));
    // Escritura directa de bloques de memoria (Ultra rápido)
    out.write((char*)row_ptr.data(), row_ptr.size() * sizeof(int));
    out.write((char*)col_indices.data(), col_indices.size() * sizeof(int));
    return true;
}

/**
 * @brief Lee bloques binarios del disco directo a los vectores.
 */
bool GrafoDisperso::cargarDesdeCache(const std::string& cacheFile) {
    std::ifstream in(cacheFile, std::ios::binary);
    if (!in) return false;
    in.read((char*)&num_nodos, sizeof(int));
    in.read((char*)&num_aristas, sizeof(int));
    row_ptr.resize(num_nodos + 1);
    col_indices.resize(num_aristas);
    in.read((char*)row_ptr.data(), row_ptr.size() * sizeof(int));
    in.read((char*)col_indices.data(), col_indices.size() * sizeof(int));
    return in.good();
}

/**
 * @brief Encuentra el nodo con mayor grado de salida analizando row_ptr.
 */
int GrafoDisperso::obtenerNodoCritico() {
    int max_d = -1; 
    int id = -1;
    for (int i = 0; i < num_nodos; ++i) {
        // En CSR, el grado es la diferencia entre el puntero siguiente y el actual
        int d = row_ptr[i+1] - row_ptr[i];
        if (d > max_d) { max_d = d; id = i; }
    }
    return id;
}

/**
 * @brief Algoritmo BFS nativo utilizando la estructura CSR.
 * @return Lista de aristas para visualización.
 */
std::vector<std::pair<int, int>> GrafoDisperso::bfs(int startNode, int depth) {
    std::vector<std::pair<int, int>> res;
    if (startNode >= num_nodos) return res;
    
    std::vector<int> vis(num_nodos, -1);
    std::queue<int> q;
    
    vis[startNode] = 0;
    q.push(startNode);
    
    while(!q.empty()){
        int u = q.front(); q.pop();
        if(vis[u] >= depth) continue;
        
        // Iterar vecinos usando aritmética de punteros CSR
        for(int i=row_ptr[u]; i<row_ptr[u+1]; ++i){
            int v = col_indices[i];
            res.push_back({u,v});
            if(vis[v]==-1){
                vis[v] = vis[u]+1;
                q.push(v);
            }
        }
    }
    return res;
}