@echo off
REM ================================================================
REM Generazione di una soluzione Visual Studio per il progetto Frobenius
REM ================================================================
REM Questo script utilizza CMake per generare una soluzione Visual Studio
REM ================================================================

echo ============================================================
echo Generazione di una soluzione Visual Studio per il progetto Frobenius
echo ============================================================

REM Verifica la presenza di CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERRORE: CMake non trovato nel PATH.
    echo Installare CMake da https://cmake.org/download/
    exit /b 1
)

REM Verifica esistenza delle directory necessarie
if not exist "include" (
    echo AVVISO: La directory 'include' non esiste. 
    echo Si presume che LibTomMath sarà configurata in seguito.
)
if not exist "lib" (
    echo AVVISO: La directory 'lib' non esiste.
    echo Si presume che LibTomMath sarà configurata in seguito.
)

REM Crea il file CMakeLists.txt
echo Generazione del file CMakeLists.txt...
(
echo cmake_minimum_required^(VERSION 3.10^)
echo project^(Frobenius VERSION 1.0.0 LANGUAGES C CXX^)
echo.
echo # Impostazioni globali
echo set^(CMAKE_CXX_STANDARD 14^)
echo set^(CMAKE_C_STANDARD 11^)
echo set^(CMAKE_EXPORT_COMPILE_COMMANDS ON^)
echo.
echo # Directory di output
echo set^(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin^)
echo.
echo # Verifica sistema operativo
echo if^(WIN32^)
echo     add_definitions^(-D_CRT_SECURE_NO_WARNINGS^)
echo endif^(^)
echo.
echo # LibTomMath
echo find_path^(TOMMATH_INCLUDE_DIR tommath.h PATHS ${CMAKE_SOURCE_DIR}/include^)
echo find_library^(TOMMATH_LIBRARY tommath PATHS ${CMAKE_SOURCE_DIR}/lib^)
echo.
echo # Versione C con LibTomMath
echo add_executable^(frobenius_ltm Frobenius_ltm.c^)
echo if^(TOMMATH_INCLUDE_DIR AND TOMMATH_LIBRARY^)
echo     target_include_directories^(frobenius_ltm PRIVATE ${TOMMATH_INCLUDE_DIR}^)
echo     target_link_libraries^(frobenius_ltm ${TOMMATH_LIBRARY}^)
echo else^(^)
echo     message^(WARNING "LibTomMath non trovata. La versione C potrebbe non compilare."^)
echo endif^(^)
echo.
echo # Esportazione dei comandi di compilazione ^(per tooling esterno^)
echo set^(CMAKE_EXPORT_COMPILE_COMMANDS ON^)
echo.
echo # Installazione
echo install^(TARGETS frobenius_ltm DESTINATION bin^)
echo.
echo # Info di compilazione
echo message^(STATUS "Generazione configurata per ${CMAKE_SYSTEM_NAME}"^)
echo message^(STATUS "Compilatore C: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}"^)
echo message^(STATUS "Compilatore C++: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"^)
) > CMakeLists.txt

REM Crea cartella build se non esiste
if not exist "build" mkdir build

REM Genera la soluzione Visual Studio
echo Generazione della soluzione Visual Studio...
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..

if %ERRORLEVEL% neq 0 (
    echo Tentativo con Visual Studio 2022...
    cmake -G "Visual Studio 17 2022" -A x64 ..
    
    if %ERRORLEVEL% neq 0 (
        echo Tentativo con versione precedente...
        cmake -G "Visual Studio 15 2017" -A x64 ..
        
        if %ERRORLEVEL% neq 0 (
            echo ERRORE: Impossibile generare la soluzione Visual Studio.
            echo Verificare l'installazione di Visual Studio.
            cd ..
            exit /b 1
        )
    )
)

cd ..

echo.
echo ============================================================
echo Soluzione Visual Studio generata nella cartella 'build'.
echo Aprire 'build\Frobenius.sln' in Visual Studio.
echo ============================================================