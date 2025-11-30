"""
@file setup.py
@brief Script de construcción multiplataforma para la extensión NeuroNet.
@details Este script automatiza la compilación del código C++ y su enlace con Python.
         Detecta automáticamente el Sistema Operativo (Windows/Linux) para:
         1. Configurar las rutas del SDK de Windows 10 (si es necesario).
         2. Seleccionar las banderas de compilación adecuadas (MSVC vs GCC).
@author David Salvador Contreras Grimaldo
@date Noviembre 2025
"""

from setuptools import setup, Extension
from Cython.Build import cythonize
import platform

# -------------------------------------------------------------------------
# DETECCIÓN DE ENTORNO
# -------------------------------------------------------------------------

## @brief Nombre del sistema operativo actual (ej. 'Windows', 'Linux', 'Darwin').
sistema = platform.system()

# -------------------------------------------------------------------------
# CONFIGURACIÓN POR DEFECTO (Linux / MacOS)
# -------------------------------------------------------------------------

## @brief Banderas de compilación estándar para GCC/Clang.
# -std=c++14: Habilita características modernas de C++.
mis_args = ["-std=c++14"]

## @brief Directorios de cabeceras (.h) adicionales.
mis_include = []

## @brief Directorios de librerías (.lib/.a) adicionales.
mis_libs = []

# -------------------------------------------------------------------------
# CONFIGURACIÓN ESPECÍFICA PARA WINDOWS
# -------------------------------------------------------------------------
if sistema == "Windows":
    """
    @note En Windows, el compilador MSVC a veces no encuentra las librerías estándar.
          Aquí forzamos las rutas del Windows 10 SDK (Versión 10.0.19041.0).
    """
    
    # 1. Definición de Rutas de Inclusión (Header Files)
    ucrt_include   = r"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt"
    shared_include = r"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared"
    um_include     = r"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um"
    
    # 2. Definición de Rutas de Librerías (Linker)
    ucrt_lib = r"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\ucrt\x64"
    um_lib   = r"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64"

    # Asignación de listas
    mis_include = [ucrt_include, shared_include, um_include]
    mis_libs = [ucrt_lib, um_lib]
    
    # 3. Banderas específicas para MSVC (Visual Studio Compiler)
    # /EHsc: Manejo de excepciones síncrono estándar.
    # /Od: Desactiva optimizaciones (útil para debug y evitar errores de linker).
    mis_args = ["/std:c++14", "/EHsc", "/Od"]

# -------------------------------------------------------------------------
# DEFINICIÓN DEL MÓDULO
# -------------------------------------------------------------------------

ext_modules = [
    Extension(
        "neuronet",                     # Nombre del módulo a importar en Python
        sources=["neuronet.pyx", "GraphCore.cpp"], # Archivos fuente
        language="c++",                 # Lenguaje del motor
        include_dirs=mis_include,       # Rutas de cabeceras dinámicas
        library_dirs=mis_libs,          # Rutas de librerías dinámicas
        extra_compile_args=mis_args,    # Banderas dinámicas
    )
]

# -------------------------------------------------------------------------
# EJECUCIÓN DE LA COMPILACIÓN
# -------------------------------------------------------------------------
setup(
    name="neuronet",
    ext_modules=cythonize(ext_modules),
    description="Motor híbrido C++/Python para análisis de grafos masivos."
)