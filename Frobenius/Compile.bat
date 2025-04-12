@echo off
REM ====================================================================
REM Script di compilazione per implementazioni del problema di Frobenius
REM ====================================================================

echo Compilazione di tutte le implementazioni del problema di Frobenius
echo.

REM Directory per i binari compilati
if not exist bin mkdir bin

REM Compila versione C (con LibTomMath)
echo Compilazione della versione C con LibTomMath...
if not exist include\tommath.h (
    echo ERRORE: LibTomMath non trovata.
    echo Scaricare LibTomMath da https://github.com/libtom/libtommath
    echo e inserire i file in una cartella 'include' e 'lib'.
    echo.
) else (
    gcc -o bin\frobenius_ltm Frobenius_ltm.c -Iinclude -Llib -ltommath -O2
    if %ERRORLEVEL% EQU 0 (
        echo Compilazione C completata con successo.
    ) else (
        echo ERRORE nella compilazione della versione C.
    )
)
echo.

REM Compila versione Rust
echo Compilazione della versione Rust...
cargo build --release --manifest-path Cargo.toml
if %ERRORLEVEL% EQU 0 (
    echo Compilazione Rust completata con successo.
    copy target\release\frobenius_num.exe bin\frobenius_num.exe
) else (
    echo ERRORE nella compilazione della versione Rust.
    echo Assicurarsi che Rust e Cargo siano installati.
)
echo.

REM Verifica che Python sia installato
echo Verifica della versione Python...
python --version >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Python trovato. La versione Python e' interpretata e non richiede compilazione.
    echo Per eseguire: python Frobenius_solver.py
) else (
    echo AVVISO: Python non trovato. Installare Python per eseguire la versione Python.
)
echo.

echo Processo di compilazione completato.
echo I file binari sono stati salvati nella cartella 'bin'
echo.