@echo off
REM ====================================================================
REM Script per generare una soluzione Visual Studio
REM ====================================================================

echo Generazione della soluzione Visual Studio per il problema di Frobenius...
echo.

REM Verifica che CMake sia installato
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake non trovato.
    echo Installare CMake da https://cmake.org/download/
    exit /b 1
)

REM Crea directory per la build se non esiste
if not exist build mkdir build

REM Genera soluzione Visual Studio
cd build
cmake .. -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Soluzione Visual Studio generata con successo.
    echo Aprire build\Frobenius.sln per sviluppare in Visual Studio.
) else (
    echo.
    echo ERROR: Generazione della soluzione fallita.
    echo Controllare che LibTomMath sia configurata correttamente.
)

cd ..
echo.