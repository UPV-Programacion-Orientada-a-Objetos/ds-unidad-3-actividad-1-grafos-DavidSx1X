"""
@file main.py
@brief Interfaz Gráfica de Usuario (GUI) para NeuroNet.
@details Utiliza Tkinter para controles y Matplotlib/NetworkX para visualización.
"""

import tkinter as tk
from tkinter import filedialog, messagebox
import neuronet  # Módulo C++ compilado
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import time

class App:
    def __init__(self, root):
        self.root = root
        self.root.title("NeuroNet - C++ Hybrid Engine")
        
        # Instancia del motor C++ (Wrapper)
        self.engine = neuronet.PyGraph()
        self.loaded = False

        # Configuración del Panel Lateral
        frame = tk.Frame(root)
        frame.pack(side=tk.LEFT, fill=tk.Y, padx=10)
        
        tk.Label(frame, text="NeuroNet", font=("Arial", 20)).pack(pady=10)
        tk.Button(frame, text="Cargar Archivo", command=self.load).pack(fill=tk.X)
        self.lbl_info = tk.Label(frame, text="Esperando...")
        self.lbl_info.pack(pady=10)
        
        # Entradas de parámetros
        tk.Label(frame, text="Nodo Inicio:").pack()
        self.entry_start = tk.Entry(frame); self.entry_start.pack()
        tk.Label(frame, text="Profundidad:").pack()
        self.entry_depth = tk.Entry(frame); self.entry_depth.pack()
        self.entry_depth.insert(0, "2")
        
        tk.Button(frame, text="Buscar Nodo Crítico", command=self.critico).pack(fill=tk.X, pady=5)
        tk.Button(frame, text="Ejecutar BFS", command=self.run_bfs, bg="green", fg="white").pack(fill=tk.X, pady=10)

        # Configuración del Canvas (Matplotlib)
        self.fig = plt.Figure(figsize=(6, 6))
        self.ax = self.fig.add_subplot(111)
        self.canvas = FigureCanvasTkAgg(self.fig, root)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

    def load(self):
        """Manejador para cargar dataset y medir tiempos."""
        f = filedialog.askopenfilename()
        if f:
            t0 = time.time()
            self.engine.load_data(f) # Llamada a C++
            stats = self.engine.get_stats()
            # Actualizar etiqueta con datos de la caché o lectura
            self.lbl_info.config(text=f"Nodos: {stats['nodos']}\nAristas: {stats['aristas']}\nTiempo: {time.time()-t0:.2f}s")
            self.loaded = True

    def critico(self):
        """Solicita el nodo crítico al backend C++."""
        if self.loaded:
            c = self.engine.get_stats()['critico']
            self.entry_start.delete(0, tk.END)
            self.entry_start.insert(0, str(c))
            messagebox.showinfo("Info", f"Nodo Crítico: {c}")

    def run_bfs(self):
        """Ejecuta simulación y dibuja el resultado."""
        if not self.loaded: return
        try:
            s = int(self.entry_start.get())
            d = int(self.entry_depth.get())
            
            # Obtener solo las aristas necesarias de C++
            edges = self.engine.run_bfs(s, d)
            
            # Dibujar con NetworkX
            self.ax.clear()
            G = nx.Graph()
            G.add_edges_from(edges)
            pos = nx.spring_layout(G, seed=42)
            nx.draw(G, pos, ax=self.ax, with_labels=True, node_size=300, node_color="skyblue", font_size=8)
            nx.draw_networkx_nodes(G, pos, ax=self.ax, nodelist=[s], node_color="red")
            self.canvas.draw()
        except Exception as e:
            print(e)

if __name__ == "__main__":
    root = tk.Tk()
    App(root)
    root.mainloop()