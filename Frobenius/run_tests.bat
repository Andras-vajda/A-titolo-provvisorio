@echo off
REM ================================================================
REM Esecuzione di tutti i test per il progetto Frobenius
REM ================================================================

echo ============================================================
echo Esecuzione di tutti i test per il progetto Frobenius
echo ============================================================

REM Verifica presenza della directory bin
if not exist "bin" (
    echo ERRORE: Directory 'bin' non trovata.
    echo Eseguire prima la compilazione.
    exit /b 1
)

echo Test della versione C:
echo -----------------------------------
if exist "bin\frobenius.exe" (
    bin\frobenius.exe
) else (
    echo AVVISO: Eseguibile C non trovato.
)

echo.
echo Test della versione Rust:
echo -----------------------------------
if exist "bin\frobenius_num.exe" (
    bin\frobenius_num.exe 3 5 7
    bin\frobenius_num.exe 6 9 20
    bin\frobenius_num.exe 11 13 15 17 19
) else (
    echo AVVISO: Eseguibile Rust non trovato.
)

echo.
echo Test della versione Python:
echo -----------------------------------
where python >nul 2>nul
if %ERRORLEVEL% equ 0 (
    python -c "import numpy, sympy" >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        python Frobenius_solver.py -t
    ) else (
        echo AVVISO: Dipendenze Python mancanti. Eseguire:
        echo pip install numpy sympy
    )
) else (
    echo AVVISO: Python non trovato.
)

echo.
echo Test della versione Haskell:
echo -----------------------------------
if exist "bin\frobenius_hs.exe" (
    bin\frobenius_hs.exe
) else (
    echo AVVISO: Eseguibile Haskell non trovato.
)

echo.
echo ============================================================
echo Esecuzione dei test completata.
echo ============================================================