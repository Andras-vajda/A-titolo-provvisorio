@echo off
REM ================================================================
REM Benchmark delle diverse implementazioni del Problema di Frobenius
REM ================================================================

echo ============================================================
echo Benchmark delle implementazioni del Problema di Frobenius
echo ============================================================

REM Verifica presenza della directory bin
if not exist "bin" (
    echo ERRORE: Directory 'bin' non trovata.
    echo Eseguire prima la compilazione.
    exit /b 1
)

echo Case di test:
echo - Caso piccolo: [3, 5, 7]
echo - Caso medio: [6, 9, 20]
echo - Caso grande: [11, 13, 15, 17, 19]
echo - Caso complesso: [31, 41, 59, 26, 53]
echo.

REM Funzione per misurare il tempo di esecuzione
setlocal EnableDelayedExpansion

REM Verifica se disponibile powershell per misurazioni precise
where powershell >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set USE_POWERSHELL=1
) else (
    set USE_POWERSHELL=0
    echo AVVISO: PowerShell non trovato, le misurazioni potrebbero essere meno precise.
)

echo Benchmark della versione C:
echo -----------------------------------
if exist "bin\frobenius.exe" (
    echo NOTA: La versione C attuale esegue solo test predefiniti.
    call :measure_time "bin\frobenius.exe" "Test integrati"
) else (
    echo AVVISO: Eseguibile C non trovato.
)

echo.
echo Benchmark della versione Rust:
echo -----------------------------------
if exist "bin\frobenius_num.exe" (
    call :measure_time "bin\frobenius_num.exe 3 5 7" "Caso piccolo"
    call :measure_time "bin\frobenius_num.exe 6 9 20" "Caso medio"
    call :measure_time "bin\frobenius_num.exe 11 13 15 17 19" "Caso grande"
    call :measure_time "bin\frobenius_num.exe 31 41 59 26 53" "Caso complesso"
    echo.
    echo Benchmark di Rust con multithreading:
    call :measure_time "bin\frobenius_num.exe --threads=4 31 41 59 26 53" "Caso complesso (4 thread)"
) else (
    echo AVVISO: Eseguibile Rust non trovato.
)

echo.
echo Benchmark della versione Python:
echo -----------------------------------
where python >nul 2>nul
if %ERRORLEVEL% equ 0 (
    python -c "import numpy, sympy" >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        call :measure_time "python Frobenius_solver.py --benchmark" "Tutti i casi"
    ) else (
        echo AVVISO: Dipendenze Python mancanti. Eseguire:
        echo pip install numpy sympy
    )
) else (
    echo AVVISO: Python non trovato.
)

echo.
echo Benchmark della versione Haskell:
echo -----------------------------------
if exist "bin\frobenius_hs.exe" (
    call :measure_time "bin\frobenius_hs.exe" "Benchmark integrato"
) else (
    echo AVVISO: Eseguibile Haskell non trovato.
)

echo.
echo ============================================================
echo Benchmark completato.
echo ============================================================
goto :eof

:measure_time
REM Funzione per misurare il tempo di esecuzione
set cmd=%~1
set desc=%~2

echo Esecuzione %desc%...

if %USE_POWERSHELL%==1 (
    powershell -Command "$s=[Diagnostics.Stopwatch]::StartNew(); Invoke-Expression '%cmd%' | Out-Null; $s.Stop(); Write-Host ('Tempo di esecuzione: ' + $s.Elapsed.TotalSeconds + ' secondi')"
) else (
    set start_time=!time!
    %cmd% >nul
    set end_time=!time!
    
    echo Iniziato alle: !start_time!
    echo Finito alle: !end_time!
)

goto :eof