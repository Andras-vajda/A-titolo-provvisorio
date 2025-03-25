@echo off

REM Controlla se cl.exe è nel PATH
where cl >nul 2>nul
if errorlevel 1 (
    echo Errore: l'ambiente non risulta configurato per Visual Studio.
    echo Assicurati di eseguire questo script dal Developer Command Prompt for Visual Studio.
    exit /b 1
)

REM Controlla la versione di cl.exe
echo Verifica versione di cl.exe...
cl
if errorlevel 1 (
    echo Errore: impossibile eseguire cl.exe.
    exit /b 2
)

REM Ambiente configurato correttamente
echo Ambiente verificato correttamente.
exit /b 0