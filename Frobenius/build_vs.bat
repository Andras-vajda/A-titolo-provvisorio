@echo off
REM ================================================================
REM Compilazione del progetto Frobenius con Visual Studio
REM ================================================================
REM Questo script deve essere eseguito da "Developer Command Prompt for VS"
REM o "x64 Native Tools Command Prompt for VS"
REM ================================================================

echo ============================================================
echo Compilazione del progetto Frobenius con Visual Studio
echo ============================================================

REM Verifica esistenza delle directory necessarie
if not exist "bin" mkdir bin
if not exist "build" mkdir build

REM Verifica che Visual Studio sia nel path
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERRORE: Compilatore Visual Studio (cl) non trovato nel PATH.
    echo Eseguire questo script da "Developer Command Prompt for VS".
    exit /b 1
)

echo Compilazione della versione C...
cl /nologo /W4 /wd4996 /O2 /Zi /MD /EHsc /Fo"build\\" /Fe"bin\frobenius.exe" Frobenius.c

if %ERRORLEVEL% neq 0 (
    echo ERRORE: Compilazione della versione C fallita.
    exit /b 1
)

echo.
echo Compilazione della versione Rust...
where cargo >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo AVVISO: Cargo (Rust) non trovato. Installare Rust da https://rustup.rs/
    echo La versione Rust non verrà compilata.
) else (
    cargo build --release
    if %ERRORLEVEL% neq 0 (
        echo ERRORE: Compilazione della versione Rust fallita.
    ) else (
        copy /Y "target\release\frobenius_num.exe" "bin\" >nul
        echo Eseguibile Rust copiato in bin\frobenius_num.exe
    )
)

echo.
echo Verifica presenza Python per esecuzione script...
where python >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo AVVISO: Python non trovato. Installare Python da https://www.python.org/
    echo Lo script Python può essere eseguito manualmente dopo l'installazione.
) else (
    echo Verificando dipendenze Python...
    python -c "import numpy, sympy" >nul 2>nul
    if %ERRORLEVEL% neq 0 (
        echo AVVISO: Dipendenze Python mancanti. Eseguire:
        echo pip install numpy sympy
    ) else (
        echo Dipendenze Python verificate.
    )
)

REM Verifica presenza GHC per la compilazione Haskell
where ghc >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo AVVISO: GHC (Haskell) non trovato. Installare Haskell Platform da https://www.haskell.org/platform/
    echo La versione Haskell non verrà compilata.
) else (
    echo.
    echo Compilazione della versione Haskell...
    ghc -O2 -o bin\frobenius_hs.exe Frobenius.hs
    if %ERRORLEVEL% neq 0 (
        echo ERRORE: Compilazione della versione Haskell fallita.
    ) else (
        echo Eseguibile Haskell creato in bin\frobenius_hs.exe
    )
)

echo.
echo ============================================================
echo Compilazione completata. Gli eseguibili sono nella cartella 'bin'.
echo ============================================================