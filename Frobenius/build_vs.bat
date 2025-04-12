@echo off
REM ====================================================================
REM Script di compilazione per implementazioni del problema di Frobenius
REM Ottimizzato per Visual Studio su Windows
REM ====================================================================

echo Compilazione di Frobenius con Visual Studio
echo.

REM Verifica Visual Studio
where cl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Visual Studio non trovato nel PATH.
    echo Eseguire questo batch da una "Developer Command Prompt for VS".
    echo Ad esempio, cercare "x64 Native Tools Command Prompt for VS 2022" nel menu Start.
    exit /b 1
)

REM Directory per i binari compilati
if not exist bin mkdir bin

REM Verifica LibTomMath
if not exist include\tommath.h (
    echo ERROR: LibTomMath non trovata.
    echo Scaricare LibTomMath da https://github.com/libtom/libtommath
    echo e inserire gli header nella cartella 'include' e le librerie in 'lib'.
    exit /b 1
)

if not exist lib\tommath.lib (
    echo ERROR: Libreria tommath.lib non trovata.
    echo Compilare LibTomMath con Visual Studio e posizionare tommath.lib nella cartella 'lib'.
    exit /b 1
)

REM Compila versione C con Visual Studio
echo Compilazione della versione C con LibTomMath...
cl /W4 /O2 /Fe:bin\frobenius_ltm.exe Frobenius_ltm.c /I"include" /link "lib\tommath.lib"
if %ERRORLEVEL% EQU 0 (
    echo Compilazione C completata con successo.
    echo Eseguibile: bin\frobenius_ltm.exe
) else (
    echo ERROR nella compilazione della versione C.
)
echo.

REM Verifica Cargo per Rust
cargo --version >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Compilazione della versione Rust...
    cargo build --release
    if %ERRORLEVEL% EQU 0 (
        echo Compilazione Rust completata con successo.
        if not exist bin mkdir bin
        copy target\release\frobenius_num.exe bin\frobenius_num.exe >nul
        echo Eseguibile: bin\frobenius_num.exe
    ) else (
        echo ERROR nella compilazione della versione Rust.
    )
) else (
    echo Cargo non trovato. La versione Rust non sara' compilata.
    echo Per compilare la versione Rust, installare Rust da https://rustup.rs/
)
echo.

REM Verifica Python
python --version >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Python trovato. La versione Python e' interpretata.
    echo Per eseguire: python Frobenius_solver.py
) else (
    echo Python non trovato. Per eseguire la versione Python, installare Python da https://www.python.org/
)
echo.

REM Pulizia file temporanei di compilazione Visual Studio
del *.obj >nul 2>&1

echo Processo di compilazione completato.
echo Gli eseguibili sono stati salvati nella cartella 'bin'
echo.