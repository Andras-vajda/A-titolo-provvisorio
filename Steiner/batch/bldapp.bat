@echo off
set SOURCE=stsc_gen.c
set LIBRARY=stsc.lib
set OPTIMIZE=/Ot /Ox

REM Richiama il batch che verifica l'ambiente
call chkenv.bat
if errorlevel 1 (
    echo Errore: l'ambiente non è configurato correttamente. Controlla i messaggi precedenti.
    exit /b %errorlevel%
)

echo Compilazione del file sorgente...
cl /nologo %OPTIMIZE% %SOURCE% %LIBRARY%
if errorlevel 1 (
    echo Errore durante la compilazione del file sorgente.
    exit /b 1
)

echo Operazione completata con successo.
