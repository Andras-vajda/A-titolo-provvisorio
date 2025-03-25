@echo off
set SOURCE=stsc.c
set OUTPUT=stsc.lib
set OBJ=stsc.obj
set OPTIMIZE=/Ot /Ox

REM Richiama il batch che verifica l'ambiente
call chkenv.bat
if errorlevel 1 (
    echo Errore: l'ambiente non è configurato correttamente. Controlla i messaggi precedenti.
    exit /b %errorlevel%
)

echo Compilazione del file sorgente...
cl /nologo /c %OPTIMIZE% %SOURCE%
if errorlevel 1 (
    echo Errore durante la compilazione del file sorgente.
    exit /b 1
)

echo Creazione della libreria statica...
lib /out:%OUTPUT% %OBJ%
if errorlevel 1 (
    echo Errore durante la creazione della libreria statica.
    exit /b 1
)

echo Dump del contenuto della libreria...
dumpbin /exports %OUTPUT%
if errorlevel 1 (
    echo Errore durante il dump della libreria.
    exit /b 1
)

echo Lista dei simboli della libreria...
lib /list %OUTPUT%
if errorlevel 1 (
    echo Errore durante il controllo dei simboli della libreria.
    exit /b 1
)

echo Operazione completata con successo.
