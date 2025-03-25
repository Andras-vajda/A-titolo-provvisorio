@echo off
:: Configura le variabili
set PROGRAM=lib_sts.c
set OUTPUT=lib_sts.exe

:: Verifica che il file sorgente esista
if not exist %PROGRAM% (
    echo Errore: Il file %PROGRAM% non esiste.
    exit /b 1
)

:: Compilazione con Visual Studio CLI
echo Compilazione in corso...
cl /Zi /Od /MDd -DENABLE_STANDALONE %PROGRAM%
if errorlevel 1 (
    echo Errore durante la compilazione.
    exit /b 1
)

:: Verifica che l'eseguibile sia stato creato
if not exist %OUTPUT% (
    echo Errore: Non è stato generato alcun eseguibile.
    exit /b 1
)

:: Esecuzione di Dr. Memory
echo Avvio di Dr. Memory...
drmemory -leaks_only -- %OUTPUT%

:: Fine
echo Operazione completata.
